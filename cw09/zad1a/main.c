#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
//------------------------------------------------------------------------------
int size =  5;
pthread_t* philosophers;
pthread_mutex_t* mutexes;
int STARTJOB;
//------------------------------------------------------------------------------
void* thread_function(void*);
void pickUpFork(int);
void putDownFork(int);
void atexitFunction(void);
//------------------------------------------------------------------------------
int main(int argc, char const *argv[]) {
  atexit(atexitFunction);
  int i;
  int IDs[size];
  STARTJOB = 1;

  philosophers = calloc(size, sizeof(pthread_t));
  for (i = 0; i < size; i++) {
    IDs[i] = i;
    if(pthread_create(&philosophers[i], NULL, thread_function, IDs + i) != 0) {
      printf("pthread_create(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  mutexes = calloc(size, sizeof(pthread_mutex_t));
  for (i = 0; i < size; i++) {
    if(pthread_mutex_init(&mutexes[i], NULL) !=0){
      printf("pthread_mutex_init(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  STARTJOB = 0;

  for(i = 0; i < size; i++) {
    pthread_join(philosophers[i], NULL);
  }

  free(philosophers);
  return 0;
}
//------------------------------------------------------------------------------
void* thread_function(void* arg) {
  int philosopherID = *((int*) arg);
  int left_fork = philosopherID;
  int right_fork = (philosopherID + 1) % size;

  while(STARTJOB);
  srand(time(NULL));

  while(1){
    printf("Philosopher %d is thinking\n", philosopherID);
    usleep(rand()%1500000 +1000000);

    if(philosopherID % 2 == 0) {
      pickUpFork(left_fork);
      pickUpFork(right_fork);
    } else {
      pickUpFork(right_fork);
      pickUpFork(left_fork);
    }

    printf("Philosopher %d is eating\n", philosopherID);
    usleep(100000);

    putDownFork(left_fork);
		putDownFork(right_fork);
  }

  return NULL;
}
//------------------------------------------------------------------------------
void pickUpFork(int whichFork) {
  pthread_mutex_lock(&mutexes[whichFork]);
}
//------------------------------------------------------------------------------
void putDownFork(int whichFork) {
  pthread_mutex_unlock(&mutexes[whichFork]);
}
//------------------------------------------------------------------------------
void atexitFunction() {
  int i;
  for(i = 0; i < size; i++) {
    pthread_cancel(philosophers[i]);
  }
}
