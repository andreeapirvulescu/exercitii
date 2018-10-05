
#include "list.h"

node_t *new_node(int val, node_t* next, void (*f)(node_t *))
{
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if (!node)
    {
        return NULL;
    }

    node->val = val;
    node->next = next;
    node->callback_print = (void *)f;
    pthread_mutex_init(&(node->mutex), NULL);
    return node;
}

list_t *new_list()
{
    list_t *list = (list_t*)malloc(sizeof(list_t));
    if (!list)
    {
        return NULL;
    }

    list->head = NULL;  
    return list;
}

void add_node(list_t *list, int val, int id)
{   
    printf("Thread %i adds %i\n", id, val);
    node_t *current = list->head;
    
    if (current == NULL)
    {
        // list is empty
        current = new_node(val, NULL, &print_node);
        pthread_mutex_lock(&(current->mutex));
        list->head = current;
        pthread_mutex_unlock(&(current->mutex));
    } else {
        while (current->next != NULL)
        {
            current = current->next;
        }
        pthread_mutex_lock(&(current->mutex));
        current->next = new_node(val, NULL, &print_node);
        pthread_mutex_unlock(&(current->mutex));
    }
}

int delete_node(list_t *list, int val, int id)
{   
    printf("Thread %i deletes %i\n", id, val);
    node_t *previous = list->head;
    pthread_mutex_lock(&(previous->mutex));
    if (previous->next == NULL)
    {
        pthread_mutex_unlock(&(previous->mutex));
        return 1;
    }

    if (previous->val == val)
    {
        list->head = previous->next;
        pthread_mutex_unlock(&(previous->mutex));
        pthread_mutex_destroy(&(previous->mutex));
        free(previous);       
        pthread_mutex_unlock(&(previous->mutex));
        return 0;
    }

    node_t *current = previous->next;
    pthread_mutex_lock(&(current->mutex));
    while(current->next != NULL)
    {
        if(current->val == val)
        {      
            previous->next = current->next;
            pthread_mutex_unlock(&(current->mutex));
            pthread_mutex_destroy(&(current->mutex));
            free(current);  
            pthread_mutex_unlock(&(previous->mutex));
            return 0;
        }
        pthread_mutex_unlock(&(previous->mutex));                           
        previous = current;             
        current = current->next;
        pthread_mutex_lock(&(current->mutex));     
    }

    if (current->val == val)
    {
        previous->next = current->next;
        pthread_mutex_unlock(&(previous->mutex));
        pthread_mutex_destroy(&(current->mutex));
        free(current);          
        pthread_mutex_unlock(&(previous->mutex));
        return 0;
    }

    pthread_mutex_unlock(&(current->mutex));
    pthread_mutex_unlock(&(previous->mutex));
    return 0;                
}

void print_node(node_t *node)
{
    printf("%i ", node->val);
}

int print_list(list_t *list, int id)
{
    printf("Thread %i prints the list \n", id);
    if (list->head == NULL)
    {
        printf("The list is empty.\n");
        return 0;
    }

    node_t *current = list->head;

    while (current->next != NULL)
    {
        current->callback_print(current);
        current = current->next;
    }
    current->callback_print(current);
    printf("\n");
    return 1;
}

void swap(node_t *a, node_t *b)
{
    int temp = a->val;
    a->val = b->val;
    b->val = temp;
}

void sort_list(list_t *list, int id)
{
    printf("Thread %i sorts the list\n", id);
    if (list->head == NULL)
    {
        return;
    }
    int swapped;
    node_t *current;
    node_t *tmp = NULL; 
    do
    {
        swapped = 0;
        current = list->head;
        while(current->next != tmp)
        {
            if (current->val > current->next->val)
            {
                swap(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
        tmp = current;
    }
    while(swapped);
}

void set_free(void **ptr)
{
    if(*ptr != NULL)
    {
        free(*ptr);
        *ptr = NULL;
    }
}

void flush_list(list_t *list)
{   
    if (list->head == NULL)
    {
        return;
    }

    node_t *current = list->head;
    node_t *next = current;

    while (current != NULL)
    {
        next = current->next;
        set_free((void*)&current);
        current = next;
    }
    set_free((void*)&(list->head));
}
