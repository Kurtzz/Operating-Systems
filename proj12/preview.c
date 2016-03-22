#include "header.h"
//-
int fd;
struct railwayLine* shm;
sem_t* shmSemaphor;
//-
int main() {

  err((shmSemaphor = sem_open(SEMNAME, O_RDWR)) == SEM_FAILED, "sem_open()");

  //pamięć wspólna
  err((fd = shm_open(SHMNAME, O_RDWR, 0660)) == -1, "shm_open()");
  err(ftruncate(fd, sizeof(struct railwayLine)) == -1, "ftruncate()");
  err((shm = (struct railwayLine*)mmap(NULL, sizeof(struct railwayLine),
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED, "mmap()")

  int i;
  while(1) {
    system("clear");
    char* nothing = calloc(4, sizeof(char));
    strcpy(nothing, "-------");
    sem_wait(shmSemaphor);

    for(i = 0; i < NLINES - 1; i+=2) {
      if(shm->lines[i] == -1) printf("%s", nothing);
      else printf("%d(%d)", shm->lines[i], shm->priorities[i]);

      if(i == 2) {
        if(shm->tunnel == -1) printf("%12s", nothing);
        else printf("%12d(%d)", shm->tunnel, shm->tunnelPrio);

        if(shm->lines[i+1] == -1) printf("%12s", nothing);
        else printf("%12d(%d)", shm->lines[i+1], shm->priorities[i+1]);

      } else {
        if(shm->lines[i+1] == -1) printf("%24s", nothing);
        else printf("%24d(%d)", shm->lines[i+1], shm->priorities[i+1]);
      }

      printf("\n");
    }

    sem_post(shmSemaphor);
    usleep(500000);
    printf("\n\n");
  }
}
