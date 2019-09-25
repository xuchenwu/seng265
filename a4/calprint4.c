/*
 * calprint4.c
 *
 * Starter file provided to students for Assignment #4, SENG 265,
 * Summer 2019.
 */

// use this to eliminate the warnings when calling **getline()** and **strdup()**
// see https://stackoverflow.com/questions/26284110/strdup-confused-about-warnings-implicit-declaration-makes-pointer-with
#define _POSIX_C_SOURCE 200809L
// this is where **ssize_t** defined
#include <sys/types.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"

/* Function: dt_format
 * -------------------
 *  convertes a datetime string of the form "yyyymmddThhmmss"
 *  into the form "<month text> <day>, <year> (<day of week>)",
 *  copied from "timeplay.c" provided by Dr. Zastre in A#1
 *  
 *  formatted_time: the destination address
 *  dt_time:        the source address
 *  len:            the maxmum length of the output string
 */
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
	struct tm temp_time;
	time_t    full_time;

	memset(&temp_time, 0, sizeof(struct tm));
	sscanf(dt_time, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	full_time = mktime(&temp_time);
	strftime(formatted_time, len, "%B %d, %Y (%a)",
	localtime(&full_time));
}

/* Function: dt_increment
 * ----------------------
 *  increments a datetime string by **num_days** days,
 *  copied from "timeplay.c" provided by Dr. Zastre in A#1
 *  with some modifications
 *
 *  after:    the destination address
 *  before:   the source address
 *  num_days: the number of days to increment
 */
void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time;
    time_t    full_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 9, "%Y%m%d", localtime(&full_time));
    strncpy(after + 8, before + 8, 7);
    after[15] = '\0';
}

/* Function: print_time
 * --------------------
 *  prints formatted time
 *  the format is <hh>:<mm> <am/pm>
 *
 *  time: the numerical value of the time to print
 */
void print_time(const int time) {
    int hour = time / 100;
    int minute = time % 100;
    char *m = (char *)emalloc(sizeof(char) * 3);
    (hour >= 12) ? strncpy(m, "pm", 3) : strncpy(m, "am", 3);
    hour = (hour > 12) ? (hour - 12) : hour;
    hour = (hour == 0) ? 12 : hour;
    printf("%2d:%02d %s", hour, minute, m);
    free(m);
}

/* Function: print_event
 * ---------------------
 *  prints the formatted event contained in a single list node.
 *  For all events,
 *  "<start hour>:<start minute> <am/pm> to <end hour>:<end minute> <am/pm>: <summary> [<location>]"
 *  must be printed
 *  For those events whose start date is not the same as the start date of its next event, also print
 *  "<blank line>
 *   <formatted date of the next event>
 *   ----------------------------------"
 *
 *  n:   the node where the event to print is contained
 *  arg: not used here
 */
void print_event(node_t *n, void *arg) {
    char *f_time = NULL; // to contain the formatted date
    int t_start, t_end;

    sscanf(n->val->dtstart + 9, "%4d", &t_start);
    sscanf(n->val->dtend + 9, "%4d", &t_end);

    print_time(t_start);
    printf(" to ");
    print_time(t_end);
    printf(": ");
    printf("%s ", n->val->summary);
    printf("[%s]\n", n->val->location);

    if (n->next != NULL && strncmp(n->val->dtstart, n->next->val->dtstart, 8) != 0) {
        printf("\n");
        f_time = (char *)emalloc(sizeof(char) * 80);
        dt_format(f_time, n->next->val->dtstart, 80);
        printf("%s\n", f_time);
        for (char *p = f_time; *p != '\0'; p++) {
            printf("-");
        }
        printf("\n");
        free(f_time); 
    } // print something about the next event if its date is different from current event
}

/* Function: print_list
 * --------------------
 *  prints the formatted events contained in the whole list.
 *
 *  head: the head of the list to print
 */
void print_list(node_t *head) {
    if (head) {
        char *f_time = (char *)emalloc(sizeof(char) * 80); // print the formatted date of the first event
        dt_format(f_time, head->val->dtstart, 80);
        printf("%s\n", f_time);
        for (char *p = f_time; *p != '\0'; p++) {
            printf("-");
        }
        printf("\n");
        apply(head, print_event, NULL); // traverse the list and print each event by calling print_event() function
        free(f_time);
    }
}

/* Function: free_list
 * -------------------
 *  free all the heap memories in the list
 *  this is a recursive function
 *
 *  head: the head of the list to free
 */
void free_list(node_t *head) {
	if (head->next) {
		free_list(head->next);
	}
	free(head->val); // first free the event in the node
	free(head);      // then free the node
}

/* Function: make_event
 * --------------------
 *  initialize an event
 *
 *  event:  the event to initialize
 *  dtstart, dtend, summary, location, rrule:
 *          the strings to initialize the 5
 *          attributes of the event
 */
void make_event(event_t *event, char *dtstart, char *dtend, 
        char *summary, char *location, char *rrule) {
    strncpy(event->dtstart, dtstart, strlen(dtstart) + 1);
	strncpy(event->dtend, dtend, strlen(dtend) + 1);
	strncpy(event->summary, summary, strlen(summary) + 1);
	strncpy(event->location, location, strlen(location) + 1);
	strncpy(event->rrule, rrule, strlen(rrule) + 1);
}

/* Function: read_file
 * -------------------
 *  read contents from a file, store the events within the range to a list
 *
 *  filename:   the name of the file to read
 *  date_from:  the start date of the range
 *  date_to:    the end date of the range
 *  head:       the head of the list where the events to store
 *
 *  returns:    the head of the list after insertion
 */
node_t *read_file(const char *filename, const int date_from, const int date_to, node_t *head)
{
	FILE *cal;
	char *line = NULL;
    size_t len = 0;
    ssize_t read;
    event_t *curr_ev = NULL;
    node_t *curr_node = NULL;
    char *after = (char *)emalloc(sizeof(char) * 16); // a char array to contain the date after incrementing by 7 days
    char *dtstart;      // a char array to contain the extracted start date and time
    char *dtend;        // a char array to contain the extracted end data and time
    char *summary;      // a char array to contain the extracted summry information
    char *location;     // a char array to contain the extracted location information
    char *rrule;        // a char array to contain the extracted repeat-until-datetime
    int date_start;     // to container for the numeric value of the start date, to compare with the range


	cal = fopen(filename, "r");
    if (cal == NULL) {
        printf("Cannot open file: %s\n", filename);
        exit(0);
    }

	while ((read = getline(&line, &len, cal)) != -1) {
		if (strncmp(line, "BEGIN:VEVENT", 12) == 0) {
			getline(&line, &len, cal);
			if (line[strlen(line) - 1] == '\n') {
				line[strlen(line) - 1] = '\0';
			} // strip the last nextline character
			dtstart = strdup(line + 8);

			getline(&line, &len, cal);
			if (line[strlen(line) - 1] == '\n') {
				line[strlen(line) - 1] = '\0';
			}
			dtend = strdup(line + 6);

			getline(&line, &len, cal);
			if (line[strlen(line) - 1] == '\n') {
				line[strlen(line) - 1] = '\0';
			}
			if (strncmp(line, "RRULE", 5) == 0) {
				line[47] = '\0';
				rrule = strdup(line + 32);

				getline(&line, &len, cal);
				if (line[strlen(line) - 1] == '\n') {
					line[strlen(line) - 1] = '\0';
				}
            } // if its a repeating event, extract the repeat-until-datetime
			else {
                rrule = strdup("");
            }

			location = strdup(line + 9);

			getline(&line, &len, cal);
			if (line[strlen(line) - 1] == '\n') {
				line[strlen(line) - 1] = '\0';
			}
			summary = strdup(line + 8);

			sscanf(dtstart, "%8d", &date_start);
			if ((date_start >= date_from) && (date_start <= date_to)) {
				curr_ev = (event_t *)emalloc(sizeof(event_t)); // allocate heap memory only if it's to be stored in list
                make_event(curr_ev, dtstart, dtend, summary, location, rrule);
				curr_node = new_node(curr_ev);
                head = add_inorder(head, curr_node);
			} // store the event if its in the range


            if (strcmp(rrule, "") != 0) {
                dt_increment(after, dtstart, 7);
                strncpy(dtstart, after, strlen(after) + 1);
                while (strcmp(dtstart, rrule) <= 0) {
                    sscanf(dtstart, "%8d", &date_start);
                    if ((date_start >= date_from) && (date_start <= date_to)) {
						curr_ev = (event_t *)emalloc(sizeof(event_t));
						make_event(curr_ev, dtstart, dtend, summary, location, rrule);
						curr_node = new_node(curr_ev);
		                head = add_inorder(head, curr_node);
        			}
					dt_increment(after, dtstart, 7);
	                strncpy(dtstart, after, strlen(after) + 1);
                }
            } // store all the event within the range up to the repeat-until-date 
			free(dtstart);
		    free(dtend);
		    free(location);
		    free(summary);
		    free(rrule);
		}
	}

    if (line) {
        free(line); // since getline() allocates heap memory, it must be freed when finishing using it
    }

	fclose(cal);
    free(after);

    return head;
}

int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i;
    int date_from, date_to;
    node_t *head = NULL;

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 7) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_d, &from_m, &from_y);
        } else if (strncmp(argv[i], "--end=", 5) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_d, &to_m, &to_y);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr,
            "usage: %s --start=dd/mm/yyyy --end=dd/mm/yyyy --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    // convert the range dates to numeric for later comparison
    date_from = from_y * 10000 + from_m * 100 + from_d;
	date_to = to_y * 10000 + to_m * 100 + to_d;

    head = read_file(filename, date_from, date_to, head);
    print_list(head);

    if (head) {
		free_list(head);
	}

    exit(0);
}
