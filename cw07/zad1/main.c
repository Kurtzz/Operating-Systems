#include "header.h"
//-----------------------------------------------------
int semafor;
int memory;
key_t semaforKey;
key_t memoryKey;
void* pamiec;
int error;
int dzialaj;
struct sharedMemo *shm;
pid_t pid;
//-----------------------------------------------------
int isPrime(int);
void signalHandler(int);
char* getTime(void);
void produce(void);
void consume(void);
//-----------------------------------------------------
int main(int argc, char* argv[]){
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

    if((semaforKey = ftok(".", 1)) == -1) {
        printf("ftok(semaforKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    if((memoryKey = ftok(".", 2)) == -1) {
        printf("ftok(memoryKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    if((semafor = semget(semaforKey, MAXTASKS + 1, 0)) == -1){
        printf("semget(semaforKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    if((memory = shmget(memoryKey, sizeof(struct sharedMemo), 0)) == -1){
        printf("shmget(memoryKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    if((pamiec = shmat(memory, NULL, 0)) == (void*)-1) {
        printf("shmat(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    shm = (struct sharedMemo*) pamiec;
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

  struct sembuf producent;
  producent.sem_flg = 0;

  struct sembuf availableTasks;
  availableTasks.sem_flg = 0;
  availableTasks.sem_num = MAXTASKS;

  while(dzialaj){
      for(i=0 ; i<MAXTASKS ; ++i){
          producent.sem_op = -1;
          producent.sem_num = i;
          availableTasks.sem_op = -1;

          if(semop(semafor, &producent, 1) == 0){
              if(shm->tasks[i].readdy == 0){
                  randNum = (int) rand()%100 + 1;
                  shm->tasks[i].number = randNum;
                  shm->tasks[i].readdy = 1;

                  if(semop(semafor, &availableTasks, 1) == 0) {
                    shm->numOfAvailableTasks++;
                  }
                  availableTasks.sem_op = 1;
                  semop(semafor, &availableTasks, 1);

                  printf("(%d %s) Dodałem liczbę %d. Liczba zadań oczekujących %d\n",
                          pid, getTime(), randNum, shm->numOfAvailableTasks);
              }
              producent.sem_op = 1;
              semop(semafor, &producent, 1);
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
  struct sembuf producent;
  struct sembuf availableTasks;

  while(dzialaj){
      i = rand()%MAXTASKS;
      producent.sem_flg = IPC_NOWAIT;
      producent.sem_op = -1;
      producent.sem_num = i;

      availableTasks.sem_flg = 0;
      availableTasks.sem_op = -1;
      availableTasks.sem_num = MAXTASKS;

      if(semop(semafor, &producent, 1) == 0){
          if(shm->tasks[i].readdy == 1){

              if(semop(semafor, &availableTasks, 1) == 0) {
                shm->numOfAvailableTasks--;
              }
              availableTasks.sem_op = 1;
              semop(semafor, &availableTasks, 1);

              if(isPrime(shm->tasks[i].number) == 0){
                printf("(%d %s) Sprawdziłem liczbę %d - pierwsza. Pozostało zadań oczekujących %d\n",
                        pid, getTime(), shm->tasks[i].number, shm->numOfAvailableTasks);
              } else {
                printf("(%d %s) Sprawdziłem liczbę %d - złożona. Pozostało zadań oczekujących %d\n",
                        pid, getTime(), shm->tasks[i].number, shm->numOfAvailableTasks);
              }
              shm->tasks[i].readdy = 0;
          }
          producent.sem_num = i;
          producent.sem_op = 1;
          producent.sem_flg = 0;
          semop(semafor, &producent, 1);
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
