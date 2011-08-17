/**
 * @file list.h
 * @brief General purpose Singly Linked List functions
 * @author Scott Moyers
 * @date 2008
 */

#ifndef MY_LIST_H
#define MY_LIST_H

/**
 * @struct NodeS A list node for a general purpose list
 */
typedef struct NodeS {
	void *data;		/**< Pointer to the data */
	struct NodeS *next;	/**< Pointer to the next member in the list */
} Node;

/**
 * Callback function used to compare values in a list when sorting.
 * @return   A negative value if value1 should be sorted before value2, 
 *           a positive value if value1 should be sorted after value2, 
 *           zero if value1 and value2 are equal.
 */
typedef int (*ListCompareFunc)(void *value1, void *value2);

/**
 * Adds a node to the end of the list (On complexity :( )
 * @param a pointer to the Head pointer of the linked list
 * @param b element to add
 * @return success (0) or failure to add (-1)
 */
int push_back_list(Node ** a, void *b);

/**
 * Adds a node to the front of the list (O1 complexity)
 * @param a pointer to the Head pointer of the linked list
 * @param b element to add
 * @return success (0) or failure to add (-1)
 */
int push_front_list(Node ** a, void *b);

/**
 * Initialises the singly linked list
 * @param a pointer to the Head pointer of the linked list
 * @return success (0) or failure (-1)
 */
int init_list(Node ** a);

/**
 * Gets the length of the list
 * @param a Head pointer to the linked list
 * @return the number of elements in the list
 */
int length_list(Node * a);

/**
 * Determines wether a given item exists in the list
 * @param a Head pointer to the linked list
 * @param data element to look for
 * @retval 1 if the item exists in the list, 0 otherwise
 */
int exists_list(Node * a, void *data);

/**
 * Remove a node from the list (assumes only 1 element exists that matches b)
 * @param a pointer to the Head pointer of the linked list
 * @param b element to remove
 */
void remove_list(Node ** a, void *b);

/**
 * Remove data from a position in a linked list
 * @param a pointer to the head pointer of the list
 * @param b position to return the data from
 * @return the data that had been removed
 */
void *remove_pos_list(Node ** a, unsigned int pos);

/**
 * Return the data from a position in the linked list
 * @param a head pointer of the list
 * @param pos the position in the list to return
 * @return data at position pos
 */
void *at_list(Node * a, unsigned int pos);

/**
 * deletes a list
 * @param a Head pointer to the list to delete
 */
void delete_list(Node * a);

/**
 * sorts a list using the given compare function
 * code was completely ripped off from the c-algorithms
 * sourceforge package.
 * @param list pointer to the list head pointer to sort
 * @param compare_func the function used to compare sort values
 */
void sort_list(Node **list, ListCompareFunc compare_func);

/**
 * concatonates list b onto list a
 * @param a first list to append to
 * @param b second list
 */
void cat_list(Node **a, Node *b);

/**
 * splits a list at the given position
 * @return a pointer to the second node
 */
Node *split_list(Node *a, unsigned int pos);

/**
 * reverses a list
 */
void reverse_list(Node **a);

typedef Node List;

#endif
