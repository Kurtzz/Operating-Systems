#include "header.h"
//-----------------------------------------------------
sem_t *semafory[MAXTASKS + 1];
int dzialaj;
int fd;
struct sharedMemo *shm;
char *semName;
//-----------------------------------------------------
void signalHandler(int);
//-----------------------------------------------------
int main(int argc, char * argv[]) {
  int i;

  if(argc != 1){
      printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa_programu \n");
      return 1;
  }
  if(signal(SIGTSTP, signalHandler) == SIG_ERR){
      printf("signal(): %d: %s\n", errno, strerror(errno));
      exit(-1);
  }

  //------- INIT -------//
  semName = calloc(strlen(SEMNAME) + 3, sizeof(char));
  for(i = 0; i < MAXTASKS + 1; i++) {
    sprintf(semName, "%s%d", SEMNAME, i);
    if((semafory[i] = sem_open(semName, O_CREAT | O_RDWR, 0660, 1)) == SEM_FAILED) { //druga wersja sem_open?
      printf("sem_open(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  if((fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0660)) == -1) {
    printf("shm_open(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  if(ftruncate(fd, sizeof(struct sharedMemo)) == -1) {
    printf("ftruncate(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  if((shm = (struct sharedMemo*)mmap(NULL, sizeof(struct sharedMemo),
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
    printf("mmap(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  //------- MAIN -------//
  dzialaj = 1;
  printf("Serwer uruchomiony\nDo zakonczenia uzyj: CTRL + Z\n");
  while(dzialaj);

  //------- CLEAN UP -------//
  for(i = 0; i < MAXTASKS + 1; i++) {
    if(sem_close(semafory[i]) == -1) {
      printf("sem_close(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
    sprintf(semName, "%s%d", SEMNAME, i);
    if(sem_unlink(semName) == -1) {
      printf("sem_unlink(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }
  if(munmap(shm, sizeof(struct sharedMemo)) == -1) {
    printf("munmap(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
  if(shm_unlink(SHMNAME) == -1) {
    printf("shm_unlink(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  return 0;
}
//-----------------------------------------------------
void signalHandler(int signal){
    dzialaj = 0;
    printf("\n");
}
//-----------------------------------------------------
