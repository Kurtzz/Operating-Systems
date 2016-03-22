#include "header.h"
//-----------------------------------------------------
sem_t *semafory[MAXTASKS + 1];
int dzialaj;
int fd;
struct sharedMemo *shm;
char *semName;
pid_t pid;
//-----------------------------------------------------
int isPrime(int);
void signalHandler(int);
char* getTime(void);
void produce(void);
void consume(void);
//-----------------------------------------------------
int main(int argc, char* argv[]){
  int i;
  semName = calloc(strlen(SEMNAME) + 3, sizeof(char));

  if(argc != 2){
      printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa_programu -k / -p \n");
      return 1;
  }

  if((strcmp(argv[1], "-p") != 0) && (strcmp(argv[1], "-k") != 0)) {
    printf("Blad w linii komend\n Bledny parametr programu!\n");
    return 1;
  }

  if(signal(SIGTSTP, signalHandler) == SIG_ERR){
      printf("signal(): %d: %s\n", errno, strerror(errno));
      exit(-1);
  }
  dzialaj = 1;

  for(i = 0; i < MAXTASKS + 1; i++) {
    sprintf(semName, "%s%d", SEMNAME, i);
    if((semafory[i] = sem_open(semName, 0)) == SEM_FAILED) { //druga wersja sem_open?
      printf("sem_open(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  if((fd = shm_open(SHMNAME, O_RDWR, 0)) == -1) {
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

  srand(time(NULL));
  pid = getpid();

  if(strcmp(argv[1], "-p") == 0){
    produce();
  } else if (strcmp(argv[1], "-k") == 0) {
    consume();
  }

  return 0;
}

//-----------------------------------------------------
void produce() {
  int i, randNum;

  while(dzialaj){
      for(i=0 ; i<MAXTASKS ; i++){

          if(sem_wait(semafory[i]) == 0){
              if(shm->tasks[i].readdy == 0){
                  randNum = (int) rand()%100 + 1;
                  shm->tasks[i].number = randNum;
                  shm->tasks[i].readdy = 1;

                  if(sem_wait(semafory[MAXTASKS]) == 0) {
                    shm->numOfAvailableTasks++;
                  }
                  sem_post(semafory[MAXTASKS]);

                  printf("(%d %s) Dodałem liczbę %d. Liczba zadań oczekujących %d\n",
                          pid, getTime(), randNum, shm->numOfAvailableTasks);
              }
              sem_post(semafory[i]);
          } else {
            printf("semop() %d: %s\n", errno, strerror(errno));
          }
          usleep(100000);
      }
  }
}
//-----------------------------------------------------
void consume() {
  int i;

  while(dzialaj){
      i = rand()%MAXTASKS;

      if(sem_wait(semafory[i]) == 0){
          if(shm->tasks[i].readdy == 1){

              if(sem_wait(semafory[MAXTASKS]) == 0) {
                shm->numOfAvailableTasks--;
              }
              sem_post(semafory[MAXTASKS]);

              if(isPrime(shm->tasks[i].number) == 0){
                printf("(%d %s) Sprawdziłem liczbę %d - pierwsza. Pozostało zadań oczekujących %d\n",
                        pid, getTime(), shm->tasks[i].number, shm->numOfAvailableTasks);
              } else {
                printf("(%d %s) Sprawdziłem liczbę %d - złożona. Pozostało zadań oczekujących %d\n",
                        pid, getTime(), shm->tasks[i].number, shm->numOfAvailableTasks);
              }
              shm->tasks[i].readdy = 0;
          }
          sem_post(semafory[i]);
      }
      usleep(100000);
  }
}
//-----------------------------------------------------
void signalHandler(int signal){
    dzialaj = 0;
    printf("\n");
}
//-----------------------------------------------------
int isPrime(int num){
    int i;
    double sqrtt = sqrt((double)num);
    if(num % 2 == 0) {
      return 1;
    }
    for(i = 3; i < sqrtt; i+=2){
      if(num % i == 0){
        return 1;
      }
    }
    return 0;
}
//-----------------------------------------------------
char* getTime(){
  struct timeval tp;
  gettimeofday(&tp, 0);
  time_t curtime = tp.tv_sec;
  struct tm *t = localtime(&curtime);
  char* data = calloc(30, sizeof(char));
  sprintf(data, "%d-%d-%d %d:%d:%d.%ld\n",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);

  return data;
}
