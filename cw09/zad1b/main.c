#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2
//------------------------------------------------------------------------------
int size =  5;
pthread_t* philosophers;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t* conds;
int* philStatus;
int STARTJOB;
//------------------------------------------------------------------------------
void* thread_function(void*);
void pickUpFork(int);
void putDownFork(int);
void atexitFunction(void);
void eat(int);
//------------------------------------------------------------------------------
int main(int argc, char const *argv[]) {
  atexit(atexitFunction);
  int i;
  int IDs[size];
  STARTJOB = 1;
  philStatus = calloc(size, sizeof(int));

  philosophers = calloc(size, sizeof(pthread_t));
  for (i = 0; i < size; i++) {
    IDs[i] = i;
    if(pthread_create(&philosophers[i], NULL, thread_function, IDs + i) != 0) {
      printf("pthread_create(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  conds = calloc(size, sizeof(pthread_cond_t));
  for (i = 0; i < size; i++) {
    if(pthread_cond_init(&conds[i], NULL) !=0){
      printf("pthread_cond_init(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  STARTJOB = 0;

  for(i = 0; i < size; i++) {
    pthread_join(philosophers[i], NULL);
  }

  free(philosophers);
  exit(0);
}
//------------------------------------------------------------------------------
void* thread_function(void* arg) {
  int philosopherID = *((int*) arg);

  while(STARTJOB);
  srand(time(NULL));

  while(1){
    printf("Philosopher %d is thinking\n", philosopherID);
    usleep(rand()%1500000 +1000000);
    eat(philosopherID);
  }

  return NULL;
}
//------------------------------------------------------------------------------
void eat(int id) {
  philStatus[id] = HUNGRY;
  int left = (id - 1) % size;
  int right = (id + 1) % size;
  pthread_mutex_lock(&mutex);
  while(philStatus[left] == EATING && philStatus[right] == EATING) {
    pthread_cond_wait(&conds[left], &mutex);
  }
  printf("Philosopher %d is eating\n", id);

  philStatus[id] = THINKING;
  pthread_cond_signal(&conds[id]);
  pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------
void atexitFunction() {
  int i;
  for(i = 0; i < size; i++) {
    pthread_cancel(philosophers[i]);
  }
}
