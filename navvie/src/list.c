/**
 * @file list.c
 * @brief General purpose Singly Linked List Library
 * @author Scott Moyers
 * @date 2008
 */

#include <stdlib.h>
#include "list.h"

int push_back_list(Node ** a, void *b)
{
	Node *work;
	Node *prev;
	Node *curr;

	curr = (Node *) malloc(sizeof(Node));
	if (curr == NULL)
		return (-1);

	curr->next = NULL;
	curr->data = b;

	if (*a == NULL) {
		*a = curr;
	} else {
		work = *a;
		prev = NULL;

		while (work != NULL) {
			prev = work;
			work = work->next;
		}

		if (prev == NULL) {
			*a = curr;
			curr->next = work;
		} else {
			curr->next = prev->next;
			prev->next = curr;
		}
	}
	return (0);
}

int push_front_list(Node ** a, void *b)
{
	Node *curr;

	curr = (Node *) malloc(sizeof(Node));
	if (curr == NULL)
		return (-1);

	curr->next = NULL;
	curr->data = b;

	if (*a != NULL)
		curr->next = *a;

	*a = curr;

	return (0);
}

int init_list(Node ** a)
{
	*a = NULL;
	return (0);
}

int length_list(Node * a)
{
	int count = 0;
	Node *work = a;

	while (work != NULL) {
		count++;
		work = work->next;
	}
	return (count);
}

int exists_list(Node * a, void *data)
{
	Node *work = a;
	while (work != NULL) {
		if (work->data == data)
			return 1;
		work = work->next;
	}
	return 0;
}

void remove_list(Node ** a, void *b)
{
	Node *work = *a;
	Node *prev = NULL;
	while (work != NULL && work->data != b) {
		prev = work;
		work = work->next;
	}

	if (work != NULL) {
		if (prev == NULL) {
			*a = work->next;
		} else {
			prev->next = work->next;
		}
		free(work);
	}
}

void *remove_pos_list(Node ** a, unsigned int pos)
{
	Node *work = *a;
	Node *prev = NULL;
	void *data;
	while (work != NULL && pos != 0) {
		prev = work;
		work = work->next;
		pos--;
	}

	if (work != NULL) {
		if (prev == NULL) {
			*a = work->next;
		} else {
			prev->next = work->next;
		}
		data = work->data;
		free(work);
		return data;
	}
	return NULL;
}

void *at_list(Node * a, unsigned int pos)
{
	Node *work = a;
	while (work != NULL && pos != 0) {
		pos--;
		work = work->next;
	}

	if (work == NULL) {
		return NULL;
	} else {
		return work->data;
	}
}

void delete_list(Node * a)
{
	Node *work = a;
	while (work != NULL) {
		work = a->next;
		free(a);
		a = work;
	}
}

/* Ripped off from slist.c in the c-algorithms package on sourceforge
   Function used internally for sorting */
static Node *sort_list_internal(Node **list,
				ListCompareFunc compare_func)
{
	Node *pivot;
	Node *rover;
	Node *less_list, *more_list;
	Node *less_list_end, *more_list_end;

	/* If there are less than two entries in this list, it is
	   already sorted */
	if (*list == NULL || (*list)->next == NULL) {
		return *list;
	}

	/* The first entry is the pivot */
	pivot = *list;

	/* Iterate over the list, starting from the second entry. Sort
	   all entries into the less and more lists based on comparisons
	   with the pivot */
	less_list = NULL;
	more_list = NULL;
	rover = (*list)->next;

	while (rover != NULL) {
		Node *next = rover->next;
		if (compare_func(rover->data, pivot->data) < 0) {
			/* Place this in the less list */
			rover->next = less_list;
			less_list = rover;
		} else {
			/* Place this in the more list */
			rover->next = more_list;
			more_list = rover;
		}
		rover = next;
	}
	/* Sort the sublists recursively */
	less_list_end = sort_list_internal(&less_list, compare_func);
	more_list_end = sort_list_internal(&more_list, compare_func);

	/* Create the new list starting from the less list */
	*list = less_list;

	/* Append the pivot to the end of the less list. If the less list
	   was empty, start from the pivot */
	if (less_list == NULL) {
		*list = pivot;
	} else {
		less_list_end->next = pivot;
	}

	/* Append the more list after the pivot */
	pivot->next = more_list;

	/* Work out what the last entry in the list is. If the more list was
	   empty, the pivot was the last entry. Otherwise, the end of the
	   more list is the end of the total list. */
	if (more_list == NULL) {
		return pivot;
	} else {
		return more_list_end;
	}
}

void sort_list(Node **list, ListCompareFunc compare_func)
{
	sort_list_internal(list, compare_func);
}

void cat_list(Node **a, Node *b)
{
	Node *work;
	Node *prev;

	if (*a == NULL) {
		*a = b;
	} else {
		work = *a;
		prev = NULL;

		while (work != NULL) {
			prev = work;
			work = work->next;
		}

		if (prev == NULL) {
			*a = b;
		} else {
			prev->next = b;
		}
	}
}

Node *split_list(Node *a, unsigned int pos)
{
	Node *work = a;
	Node *prev = NULL;
	Node *l2 = NULL;
	while(work != NULL && pos != 0) {
		prev = work;
		work = work->next;
		pos--;
	}

	if (prev != NULL) {
		l2 = prev->next;
		prev->next = NULL;
	}
	return l2;
}

void reverse_list(Node **a)
{
	Node *cur = *a;
	*a = NULL;
	while(cur != NULL) {
		Node *old = cur->next;
		cur->next = *a;
		*a = cur;
		cur = old;
	}
}
