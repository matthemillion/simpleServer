#include "ThreadPool.h"


static void * ThreadLoop(void *pool) {
	ThreadPool *TP = (ThreadPool *) pool;

	void *generic;
	void * (* f)(void *);
	
	while(!TP->done) {

		// lock mutex
		pthread_mutex_lock(&(TP->mutex));

		while(!TP->done && TP->task_number == 0)
			pthread_cond_wait(&(TP->condition), &(TP->mutex));

		// test for task availability
		if(TP->task_number == 0) {

			// unlock mutext if no task was found
			pthread_mutex_unlock(&(TP->mutex));

		} else {

			// get a task 
			generic = TP->tasks[TP->front].generic;
			f = TP->tasks[TP->front].f;

			// increment front
			TP->front = (TP->front + 1) % TP->capacity;

			// decrement task number
			--(TP->task_number);

			// unlock mutex
			pthread_mutex_unlock(&(TP->mutex));

			// work task
			f(generic);
		}
	}

	return NULL;
}


int ThreadPoolStartUp(ThreadPool *TP, int work_num, int cap) {
	TP->done = 0;

	// initialize mutex & condition variables
	if(pthread_mutex_init(&(TP->mutex), NULL) != 0
	|| pthread_cond_init(&(TP->condition), NULL) != 0)
		return 0;

	TP->worker_number = work_num;

	// get memory for worker threads
	if((TP->workers = malloc(work_num * sizeof(pthread_t))) == NULL)
		return 0;

	TP->capacity = cap;

	// get memory for tasks
	if((TP->tasks = malloc(cap * sizeof(Task))) == NULL) {
		free(TP->workers);
		return 0;
	}

	TP->task_number = 0;
	TP->front = 0;
	TP->back = cap - 1;

	// start worker threads
	for(int i = 0; i < work_num; ++i) {		
		if(pthread_create(&(TP->workers[i]), NULL, ThreadLoop, TP) != 0) {
			for(int j = 0; j < i; ++j)
				pthread_join(TP->workers[j], NULL);

			free(TP->workers);
			free(TP->tasks);
			return 0;
		}
	}

	return 1;
}


int ThreadPoolScheduleTask(ThreadPool *TP, void * (* f)(void *), void *generic) {
	if(TP->done || TP->task_number == TP->capacity) {
		f(generic);
		return 0;
	}

	// lock mutex
	pthread_mutex_lock(&(TP->mutex));

	// increment TPueue back
	TP->back = (TP->back + 1) % TP->capacity;

	// push task onto TPueue
	TP->tasks[TP->back].f = f;
	TP->tasks[TP->back].generic = generic;

	// increment number of tasks
	++(TP->task_number);

	// unlock mutex
	pthread_mutex_unlock(&(TP->mutex));

	// awaken a thread
	pthread_cond_signal(&(TP->condition));

	return 1;
}


void ThreadPoolShutDown(ThreadPool *TP) {
	void *generic;
	void * (* f)(void *);

	TP->done = 1;

	// broadcast signal to each thread that is waiting
	pthread_cond_broadcast(&(TP->condition));

	// join each thread
	for(int i = 0; i < TP->worker_number; ++i)
		pthread_join(TP->workers[i], NULL);

	// do remaining tasks in TPueue
	while(TP->task_number > 0) {

		// get a task from TPueueu 
		generic = TP->tasks[TP->front].generic;
		f = TP->tasks[TP->front].f;

		// increment TPueue front
		TP->front = (TP->front + 1) % TP->capacity;

		// decrement TPueue task number
		--(TP->task_number);

		// work task
		f(generic);
	}

	free(TP->workers);
	free(TP->tasks);

	pthread_mutex_destroy(&(TP->mutex));
	pthread_cond_destroy(&(TP->condition));
}

