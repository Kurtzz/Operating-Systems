#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define TABSIZE 512
//------------------------------------------------------------------------------
int nWriters, nReaders;
pthread_t* writers;
pthread_t* readers;
sem_t writersSem;
sem_t* readersSems;
int fd;
int* shm;
int STARTJOB;
int doYourJob;
//------------------------------------------------------------------------------
void* writing(void*);
void* reading(void*);
void signalHandler(int);
void atexitFunction(void);
//------------------------------------------------------------------------------
int main(int argc, char const *argv[]) {

  if(argc != 3) {
    printf("Bledna liczba argumentow!\n"
            "Prawidlowe wywolanie [Nazwa programu] [liczba pisarzy] [liczba czytelnikow]\n");
    exit(-1);
  }
  if(signal(SIGTSTP, signalHandler) == SIG_ERR){
      printf("signal(): %d: %s\n", errno, strerror(errno));
      exit(-1);
  }
  atexit(atexitFunction);

  nWriters = atoi(argv[1]);
  nReaders = atoi(argv[2]);
  STARTJOB = 1;
  doYourJob = 1;

  //THREADS
  writers = calloc(nWriters, sizeof(pthread_t));
  readers = calloc(nReaders, sizeof(pthread_t));
  int i;
  for(i = 0; i < nWriters; i++) {
    if(pthread_create(&writers[i], NULL, writing, NULL) != 0) {
      printf("pthread_create(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  int IDs[nReaders];
  for(i = 0; i < nReaders; i++) {
    IDs[i] = i;
    if(pthread_create(&readers[i], NULL, reading, IDs + i) != 0) {
      printf("pthread_create(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  //SEMAPHORES
  readersSems = calloc(nReaders, sizeof(sem_t));
  if(sem_init(&writersSem, 0, 1) == -1){
    printf("sem_init(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
  for (i = 0; i < nReaders; i++) {
    if(sem_init(&readersSems[i], 0, 1) == -1){
      printf("sem_init(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  //SHARED MEMORY
  if((fd = shm_open("/shm", O_CREAT | O_RDWR, 0660)) == -1) {
    printf("shm_open(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  if(ftruncate(fd, sizeof(int)*TABSIZE) == -1) {
    printf("ftruncate(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  if((shm = (int*)mmap(NULL, sizeof(int)*TABSIZE,
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    printf("mmap(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  STARTJOB = 0;
  for(i = 0; i < nWriters; i++) {
    pthread_join(writers[i], NULL);
  }
  for(i = 0; i < nReaders; i++) {
    pthread_join(readers[i], NULL);
  }

  exit(0);
}
//------------------------------------------------------------------------------
void* writing(void* unused){
  int howMany, position, number;
  while(STARTJOB);

  int i,j;
  srand(time(NULL));
  while(doYourJob) {
    usleep(rand()%1500000 + 500000);
    howMany = (int) rand()%(TABSIZE / 4);

    sem_wait(&writersSem);
    for(j = 0; j < nReaders; j++) {
      sem_wait(&readersSems[j]);
    }

    for (i = 0; i < howMany; i++) {
      position = rand()%TABSIZE;
      number = rand()%100;
      shm[position] = number;
    }
    printf("Pisarz zapisal %d liczb\n", howMany);

    for(j = 0; j < nReaders; j++) {
      sem_post(&readersSems[j]);
    }
    sem_post(&writersSem);
  }

  return NULL;
}
//------------------------------------------------------------------------------
void* reading(void* arg){
  srand(time(NULL));
  int id = *((int*) arg);
  int number = rand()%100;
  int howMany, i;
  while(STARTJOB);

  while(doYourJob) {
    usleep(rand()%1500000 + 500000);
    sem_wait(&readersSems[id]);

    howMany = 0;
    for(i = 0; i < TABSIZE; i++) {
      if(shm[i] == number) {
        howMany++;
      }
    }
    printf("Czytelnik znalazl %d liczb o wartosci %d\n", howMany, number);

    sem_post(&readersSems[id]);
  }
  return NULL;
}
//------------------------------------------------------------------------------
void atexitFunction() {
  sleep(1);
  int i;
  for(i = 0; i < nWriters; i++) {
    pthread_cancel(*(writers + i));
  }
  for(i = 0; i < nReaders; i++) {
    pthread_cancel(readers[i]);
    sem_destroy(&readersSems[i]);
  }
  sem_destroy(&writersSem);

  munmap(shm, sizeof(int)*TABSIZE);
  shm_unlink("/shm");

  free(writers);
  free(readers);
  free(readersSems);
  free(shm);
}
//------------------------------------------------------------------------------
void signalHandler(int signal){
    doYourJob = 0;
}
