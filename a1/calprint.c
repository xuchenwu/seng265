/*
 * calprint.c
 *
 * Starter file provided to students for Assignment #1, SENG 265,
 * Summer 2019.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LEN 80
#define MAX_EVENTS 500


/*
 * A struct type you are welcome to use, but are not
 * required to use.
 */

typedef struct event_t
{
	char dt_start[MAX_LINE_LEN];
	char dt_end[MAX_LINE_LEN];
	char summary[MAX_LINE_LEN];
	char location[MAX_LINE_LEN];
	char dt_repeat_until[MAX_LINE_LEN];
} event_t;


/*
 * Some global variables (as few as possible). Again, you
 * are not required to use these -- but ensure that what is
 * **not** used is deleted.
 */

event_t entry[MAX_EVENTS];
int     num_entries = 0;

/*----------------------------------------------------------------------------*/
/* copied from timeplay.c                                                     */
/*----------------------------------------------------------------------------*/
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
	struct tm temp_time;
	time_t    full_time;
	//char      temp[5];

	memset(&temp_time, 0, sizeof(struct tm));
	sscanf(dt_time, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	full_time = mktime(&temp_time);
	strftime(formatted_time, len, "%B %d, %Y (%a)",
	localtime(&full_time));
}

/*----------------------------------------------------------------------------*/
/* copie from timeplay.c                                                      */
/*----------------------------------------------------------------------------*/
void dt_increment(char *after, const char *before, int const num_days)
{
	struct tm temp_time, *p_temp_time;
	time_t    full_time;
	char      temp[5];

	memset(&temp_time, 0, sizeof(struct tm));
	sscanf(before, "%4d%2d%2d", &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
	temp_time.tm_year -= 1900;
	temp_time.tm_mon -= 1;
	temp_time.tm_mday += num_days;

	full_time = mktime(&temp_time);
	after[0] = '\0';

	strftime(after, 9, "%Y%m%d", localtime(&full_time));
	strncpy(after + 8, before + 8, MAX_LINE_LEN - 8);
	after[MAX_LINE_LEN - 1] = '\0';
}

/*----------------------------------------------------------------------------*/
/* define the order of event_t elements for sorting                           */
/*----------------------------------------------------------------------------*/
int compare(const void *a, const void *b)
{
	event_t *pa = (event_t *)a;
	event_t *pb = (event_t *)b;

	if (strcmp((*pa).dt_start, (*pb).dt_start) != 0) {
		return strcmp((*pa).dt_start, (*pb).dt_start);
	}
	else {
		return strcmp((*pa).summary, (*pb).summary);
	}
}

void read(const char *filename, int date_from, int date_to)
{
	FILE *cal;
	char buffer[MAX_LINE_LEN];

	cal = fopen(filename, "r");

	while (fgets(buffer, MAX_LINE_LEN, cal)) {
		if (strncmp(buffer, "BEGIN:VEVENT", 12) == 0) {
			event_t cur_ev;
			int date_start, date_end;

			fgets(buffer, MAX_LINE_LEN, cal);
			sscanf(buffer, "DTSTART:%s", cur_ev.dt_start);

			fgets(buffer, MAX_LINE_LEN, cal);
			sscanf(buffer, "DTEND:%s", cur_ev.dt_end);

			fgets(buffer, MAX_LINE_LEN, cal);
			if (strncmp(buffer, "RRULE", 5) == 0) {
				sscanf(buffer, "RRULE:FREQ=WEEKLY;WKST=MO;UNTIL=%15s", cur_ev.dt_repeat_until);
				fgets(buffer, MAX_LINE_LEN, cal);
				strncpy(cur_ev.location, &buffer[9], strlen(buffer));
				cur_ev.location[strlen(cur_ev.location) - 1] = '\0';
				fgets(buffer, MAX_LINE_LEN, cal);
				strncpy(cur_ev.summary, &buffer[8], strlen(buffer));
				cur_ev.summary[strlen(cur_ev.summary) - 1] = '\0';
			}
			else {
				strncpy(cur_ev.dt_repeat_until, cur_ev.dt_start, MAX_LINE_LEN);
				strncpy(cur_ev.location, &buffer[9], strlen(buffer));
				cur_ev.location[strlen(cur_ev.location) - 1] = '\0';
				fgets(buffer, MAX_LINE_LEN, cal);
				strncpy(cur_ev.summary, &buffer[8], strlen(buffer));
				cur_ev.summary[strlen(cur_ev.summary) - 1] = '\0';
			}

			sscanf(cur_ev.dt_start, "%8d", &date_start);
			sscanf(cur_ev.dt_end, "%8d", &date_end);
			if ((date_start >= date_from) && (date_end <= date_to)) {
				entry[num_entries] = cur_ev;
				num_entries++;
			}

			/* recording repeated events within the range */
			while (strncmp(cur_ev.dt_start, cur_ev.dt_repeat_until, 15) < 0) {
				char after[MAX_LINE_LEN];
				dt_increment(after, cur_ev.dt_start, 7);
				strncpy(cur_ev.dt_start, after, 15);
				dt_increment(after, cur_ev.dt_end, 7);
				strncpy(cur_ev.dt_end, after, 15);
				if (strncmp(cur_ev.dt_start, cur_ev.dt_repeat_until, 15) <= 0) {
					sscanf(cur_ev.dt_start, "%8d", &date_start);
					sscanf(cur_ev.dt_end, "%8d", &date_end);
					if ((date_start >= date_from) && (date_end <= date_to)) {
						entry[num_entries] = cur_ev;
						num_entries++;
					}
				}
			}
		}
	}

	fclose(cal);

	if (num_entries > 0) {
		qsort(entry, num_entries, sizeof(event_t), compare);
	}
}

void print() {
	for (int n = 0; n < num_entries; n++) {
		int date_start, hour_start, minute_start;
		int date_end, hour_end, minute_end;
		char f_time[MAX_LINE_LEN];

		if ((n == 0) || ((n > 0) && (strncmp(entry[n - 1].dt_start, entry[n].dt_start, 8) != 0))) {
			dt_format(f_time, entry[n].dt_start, MAX_LINE_LEN);
			if (n > 0) {
				printf("\n");
			}
			printf("%s\n", f_time);
			for (int m = 0; m < strlen(f_time); m++) {
				printf("-");
			}
			printf("\n");
		}

		sscanf(entry[n].dt_start, "%8dT%2d%2d", &date_start, &hour_start, &minute_start);
		sscanf(entry[n].dt_end, "%8dT%2d%2d", &date_end, &hour_end, &minute_end);

		if (hour_end < 12) {
			printf("%2d:%02d am to %2d:%02d am: %s [%s]\n",
				hour_start, minute_start,
					hour_end, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start > 12) {
			printf("%2d:%02d pm to %2d:%02d pm: %s [%s]\n",
				hour_start - 12, minute_start,
					hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start < 12 && hour_end > 12) {
			printf("%2d:%02d am to %2d:%02d pm: %s [%s]\n",
				hour_start, minute_start,
					hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start < 12 && hour_end == 12) {
			printf("%2d:%02d am to %2d:%02d pm: %s [%s]\n",
				hour_start, minute_start,
					hour_end, minute_end, entry[n].summary, entry[n].location);
		} else if (hour_start == 12 && hour_end > 12) {
			printf("%2d:%02d pm to %2d:%02d pm: %s [%s]\n",
				hour_start, minute_start,
					hour_end - 12, minute_end, entry[n].summary, entry[n].location);
		} else {
			printf("%2d:%02d pm to %2d:%02d pm: %s [%s]\n",
				hour_start, minute_start,
					hour_end, minute_end, entry[n].summary, entry[n].location);
		}
	}
}

int main(int argc, char *argv[])
{
	int from_y = 0, from_m = 0, from_d = 0;
	int to_y = 0, to_m = 0, to_d = 0;
	int date_from;
	int date_to;
	char *filename = NULL;

	for (int i = 0; i < argc; i++) {
		if (strncmp(argv[i], "--from=", 7) == 0) {
			sscanf(argv[i], "--from=%d/%d/%d", &from_d, &from_m, &from_y);
		} else if (strncmp(argv[i], "--to=", 5) == 0) {
			sscanf(argv[i], "--to=%d/%d/%d", &to_d, &to_m, &to_y);
		} else if (strncmp(argv[i], "--file=", 7) == 0) {
			filename = argv[i]+7;
		}
	}

	if (from_y == 0 || to_y == 0 || filename == NULL) {
		fprintf(stderr, "usage: %s --from=dd/mm/yyyy --to=dd/mm/yyyy --file=icsfile\n", argv[0]);
		exit(1);
	}

	date_from = from_y * 10000 + from_m * 100 + from_d;
	date_to = to_y * 10000 + to_m * 100 + to_d;

	read(filename, date_from, date_to);
	print();

	exit(0);
}
