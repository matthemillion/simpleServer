#ifndef THREADPOOL_H_INCLUDED
#define THREADPOOL_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct {
	void *generic;
	void * (* f)(void *);
} Task;

typedef struct {
	int done;
	pthread_mutex_t mutex;
	pthread_cond_t condition;

	int worker_number;
	pthread_t *workers;

	int capacity;
	int task_number;
	int front;
	int back;
	Task *tasks;
} ThreadPool;

int ThreadPoolStartUp(ThreadPool *, int, int);

int ThreadPoolScheduleTask(ThreadPool *, void * (*)(void *), void *);

void ThreadPoolShutDown(ThreadPool *);

#endif // THREADPOOL_H_INCLUDED
