#ifndef _LIST_H
#define _LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct node
{
	int val;
	struct node* next;
	void (*callback_print)(void *);
	pthread_mutex_t mutex;
} node_t;

typedef struct list
{
	node_t *head;
} list_t;

node_t *new_node(int val, node_t *next, void (*f)(node_t *));
list_t *new_list();
void add_node(list_t *list, int val, int id);
int delete_node(list_t *list, int val, int id);
void print_node(node_t *node);
int print_list(list_t *list, int id);
void swap(node_t *a, node_t *b);
void sort_list(list_t *list, int id);
void set_free(void **ptr);
void flush_list();

#endif