/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ics.h"
#include "emalloc.h"
#include "listy.h"


node_t *new_node(event_t *val) {
    assert( val != NULL);

    node_t *temp = (node_t *)emalloc(sizeof(node_t));

    temp->val = val;
    temp->next = NULL;

    return temp;
}


node_t *add_front(node_t *list, node_t *new) {
    new->next = list;
    return new;
}


node_t *add_end(node_t *list, node_t *new) {
    node_t *curr;

    if (list == NULL) {
        new->next = NULL;
        return new;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = new;
    new->next = NULL;
    return list;
}


node_t *peek_front(node_t *list) {
    return list;
}


node_t *remove_front(node_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}



void apply(node_t *list,
           void (*fn)(node_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}

/* Function: compare
 * -----------------
 *  decide the order of two events
 *
 *  a, b:    the 2 events to compare
 *
 *  returns: -1 if a has earlier start date, or a, b have the same start date
 *              but a has smaller summary with respect to the dictionary order;
 *            0 if a and b have same start date and same summary
 *            1 otherwise 
 */
int compare(const event_t *a, const event_t *b) {
	if (strcmp(a->dtstart, b->dtstart) != 0) {
		return strcmp(a->dtstart, b->dtstart);
	}
	else {
		return strcmp(a->summary, b->summary);
	}
}

/* Function: add_inorder
 * ---------------------
 *  add a node to an ordered list, the order of the resulting list is maintained
 *
 *  list:    the list to insert in
 *  new:     the node to insert
 *
 *  returns: the head of the resulting list 
 */
node_t *add_inorder(node_t *list, node_t *new) {
    node_t *curr;

    if (list == NULL) {
        new->next = list;
        return new;
    }

    if (compare(new->val, list->val) < 0) {
        new->next = list;
        return new;
    }

    curr = list; 
    while (curr->next != NULL && compare(curr->next->val, new->val) < 0) { 
        curr = curr->next; 
    } 
    new->next = curr->next; 
    curr->next = new;
    return list;
}
