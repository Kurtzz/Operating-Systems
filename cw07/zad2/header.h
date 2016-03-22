#ifndef HEADER_H
#define HEADER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h> //
#include <semaphore.h> //semafory
#include <sys/mman.h> //shared memory
#include <fcntl.h> //O_constans
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
//-----------------------------------------------------
#define MAXTASKS 10
#define N 3
#define SEMNAME "/semaphore"
#define SHMNAME "/sharedmemory"
//-----------------------------------------------------
struct task{
    int readdy;
    int number;
};

struct sharedMemo {
  struct task tasks[MAXTASKS];
  int numOfAvailableTasks;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

#endif
