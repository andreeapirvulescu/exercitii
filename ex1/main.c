#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"

#define THREAD_COUNT 3

pthread_barrier_t barrier;
list_t *list;

void* thread_info(void *arg)
{
	int thread_id = (intptr_t)arg;
	int rc;

	printf("Hello from thread %i\n", thread_id);

	rc = pthread_barrier_wait(&barrier);	

	if (thread_id == 0)
	{
		add_node(list, 2, thread_id);
		usleep(1);
		add_node(list, 4, thread_id);
		usleep(1);
		add_node(list, 10, thread_id);
		usleep(1);
		delete_node(list, 2, thread_id);
		usleep(1);
		sort_list(list, thread_id);
		usleep(1);
		delete_node(list, 10, thread_id);
		usleep(1);
		delete_node(list, 5, thread_id);
		usleep(1);
	}

	if (thread_id == 1)
	{
		add_node(list, 11, thread_id);
		usleep(1);
		add_node(list, 1, thread_id);
		usleep(1);
		delete_node(list, 11, thread_id);
		usleep(1);
		add_node(list, 8, thread_id);
		usleep(1);
		print_list(list, thread_id);
	}

	if (thread_id == 2)
	{
		add_node(list, 30, thread_id);
		usleep(1);
		add_node(list, 25, thread_id);
		usleep(1);
		add_node(list, 100, thread_id);
		usleep(1);
		sort_list(list, thread_id);
		usleep(1);
		print_list(list, thread_id);
		usleep(1);
		delete_node(list, 100, thread_id);

	}

	return NULL;
}

int main()
{
	list = new_list();

	long i;
	pthread_t ids[THREAD_COUNT];

	pthread_barrier_init(&barrier, NULL, THREAD_COUNT);

	for (i = 0; i < THREAD_COUNT; i++)
	{
		pthread_create(&ids[i], NULL, thread_info,(void *)i);
	}

	for (i = 0; i < THREAD_COUNT; i++)
	{
		pthread_join(ids[i], NULL);
	}

	pthread_barrier_destroy(&barrier);

	print_list(list, 0);

	flush_list(list);

	free(list);
	return 0;
}
