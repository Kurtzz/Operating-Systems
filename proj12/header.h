#ifndef HEADER_H
#define HEADER_H
//------------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h> //signals?
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <semaphore.h> //semafory
#include <sys/mman.h> //shared memory
#include <fcntl.h> //O_constans
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <mqueue.h>

//------------------------------------------------------------------------------
#define err(X, Y) if(X) {printf("%s: %d: %s\n", Y, errno, strerror(errno));exit(-1);}
#define SEMNAME "/semaphore"
#define SHMNAME "/linesstate"
#define MSGQUEUE "/msgqueue"
#define NLINES 6
//------------------------------------------------------------------------------

struct railwayLine {
  int lines[NLINES];
  unsigned int priorities[NLINES];
  int tunnel;
  unsigned int tunnelPrio;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};
//------------------------------------------------------------------------------
#endif
