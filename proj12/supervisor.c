#include "header.h"
//------------------------------------------------------------------------------
sem_t *semaphores[NLINES]; //semafory blokujące wjazd do tunelu z torów
sem_t *shmSem; //semafor pamięci wspólnej
int fd; //deskryptor dla shm_open
struct railwayLine* shm; //pamięc wspólna (stan lini)
mqd_t* linesQueues; //kolejki na liniach
pid_t generator;
//------------------------------------------------------------------------------
void tunnelIsFree(int);
void stopHandler(int);
void atexitfun(void);
//------------------------------------------------------------------------------
int main() {
  atexit(atexitfun);

  //TWORZENIE SEMAFOROW DLA LINII KOLEJOWYCH I PAMIĘCI WSPÓLNEJ
  int i;
  char* semName = calloc(strlen(SEMNAME) + 3, sizeof(char));
  for(i = 0; i < NLINES; i++) {
    sprintf(semName, "%s%d", SEMNAME, i);
    err((semaphores[i] = sem_open(semName, O_CREAT | O_RDWR, 0660, 0)) == SEM_FAILED, "sem_open()");
  }
  err((shmSem = sem_open(SEMNAME, O_CREAT | O_RDWR, 0660, 1)) == SEM_FAILED, "sem_open()");

  //TWORZENIE PAMIĘCI WSPÓLNEJ
  err((fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0660)) == -1, "shm_open()");
  err(ftruncate(fd, sizeof(struct railwayLine)) == -1, "ftruncate()");
  err((shm = (struct railwayLine*)mmap(NULL, sizeof(struct railwayLine),
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED, "mmap()")

  //oznaczam wszystkie linie i tunel jako "wolne" == -1
  sem_wait(shmSem);
  for(i = 0; i < NLINES; i++) {
    shm->lines[i] = -1;
  }
  shm->tunnel = -1;
  sem_post(shmSem);

  //TWORZENIE KOLEJEK KOMUNIKATÓW DLA KAŻDEJ LINII KOLEJOWEJ
  struct mq_attr attr;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 1024;
  attr.mq_flags = O_NONBLOCK;

  linesQueues = calloc(NLINES, sizeof(mqd_t));
  char* mqName = calloc(strlen(MSGQUEUE) + 1, sizeof(char));
  for(i = 0; i < NLINES; i++) {
    sprintf(mqName, "%s%d", MSGQUEUE, i);
    err((linesQueues[i] = mq_open(mqName, O_RDWR | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr)) == -1, "mq_open()")
  }

  //OBSLUGA SYGNALOW
  //SIGUSR1 == TUNEL JEST WOLNY, sygnal wysyla pociag do nadzorcy
  //SIGTSTP == "ZAMKNIJ TUNEL", kończenie programu
  struct sigaction sa, sastp;
	sa.sa_handler = tunnelIsFree;
	sa.sa_flags = 0;

  sastp.sa_handler = stopHandler;
  sastp.sa_flags = 0;

  err(sigaction(SIGUSR1, &sa, NULL) == -1, "sigaction()");
  err(sigaction(SIGTSTP, &sastp, NULL) == -1, "sigaction()");

  //URUCHAMIAMY GENERATOR POCIĄGÓW
  err((generator = fork()) == -1, "fork()");
  if(generator == 0) {
    err(execl("./generator", "generator", NULL) == -1, "execl()");
  }


  // V tablica nr pid pociągów, które stoją na liniach kolejowych
  char* msg[NLINES];
  for(i = 0; i < NLINES; i++) {
    msg[i] = calloc(1024, sizeof(char));
  }

  unsigned int highestPriority; //Najwyższy priorytet spośród pociągów oczekujących
  int highestPriorityLine; //Linia na której stoi pociąg z najwyższym priorytetem

  //NADZORCA ROZPOCZYNA PRACE
  sleep(1);
  while(1) {
    //Na puste linie, wpuszczamy pociąg o najwyższym priorytecie
    //i najdłuższym czasie oczekiwania z kolejki
    for(i = 0; i < NLINES; i++) {
      sem_wait(shmSem);
      if(shm->lines[i] == -1) {
        if(mq_receive(linesQueues[i], msg[i], sizeof(char) * 1024, &shm->priorities[i]) > -1) {
          kill(atoi(msg[i]), SIGUSR1); //Zezwalamy pociągowi zająć tor
          shm->lines[i] = atoi(msg[i]); //Tor zajęty
        }
      }
      sem_post(shmSem);
    }
    sleep(1); //dla programu preview

    //Szukamy pociągu o najwyższym priorytecie na torach
    highestPriority = 0;
    highestPriorityLine = -1;
    sem_wait(shmSem);
    for(i = 0; i < NLINES; i++) {
      if(shm->priorities[i] > highestPriority) {
        highestPriority = shm->priorities[i];
        highestPriorityLine = i;
      }
    }
    sem_post(shmSem);

    if(highestPriority <= 0) {
      //Jeżeli nie było wcale pociągów, to kolejny obieg pętli
      continue;
    }

    sem_post(semaphores[highestPriorityLine]); //Otwieram semafor, wpuszczam do tunelu

    //UPDATE sytuacji w pamięci wspólnej
    sem_wait(shmSem);
    shm->lines[highestPriorityLine] = -1;
    shm->tunnel = atoi(msg[highestPriorityLine]);
    shm->tunnelPrio = shm->priorities[highestPriorityLine];
    shm->priorities[highestPriorityLine] = 0; //Na tym torze nie stoi już żaden pociąg
    sem_post(shmSem);

    pause(); //Czekamy aż pociąg wyjedzie z tunelu

    sem_wait(shmSem);
    shm->tunnel = -1; //Tunel jest wolny
    sem_post(shmSem);

    err(sem_wait(semaphores[highestPriorityLine]) == -1, "sem_wait()"); //Blokuję z powrotem wjazd do tunelu
  }

  exit(0);
}
//------------------------------------------------------------------------------
void tunnelIsFree(int signum) {}
//------------------------------------------------------------------------------
void stopHandler(int signum) {
  kill(generator, SIGTSTP);
  waitpid(generator, NULL, 0);
  exit(0);
}
//------------------------------------------------------------------------------
void atexitfun() {
  int i;
  //USUWANIE SEMAFORÓW
  sem_close(shmSem);
  sem_unlink(SEMNAME);
  char* semName = calloc(strlen(SEMNAME) + 3, sizeof(char));
  for(i = 0; i < NLINES; i++) {
    sprintf(semName, "%s%d", SEMNAME, i);
    sem_close(semaphores[i]);
    sem_unlink(semName);
  }

  //USUWANIE PAMIĘCI WSPÓLNEJ
  munmap(shm, sizeof(struct railwayLine));
  shm_unlink(SHMNAME);

  //USUWANIE KOLEJEK KOMUNIKATÓW
  char* mqName = calloc(strlen(MSGQUEUE) + 1, sizeof(char));
  for(i = 0; i < NLINES; i++) {
    sprintf(mqName, "%s%d", MSGQUEUE, i);
    mq_close(linesQueues[i]);
    mq_unlink(mqName);
  }
}
