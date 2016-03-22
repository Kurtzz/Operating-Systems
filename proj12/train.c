#include "header.h"
//------------------------------------------------------------------------------
int priority, line;
pid_t myPid, supervisor;
sem_t* lineSemaphor;
sem_t* shmSemaphor;
mqd_t queue;
int fd;
struct railwayLine* shm;
//------------------------------------------------------------------------------
void handler(int);
void stopHandler(int);
void atexitfun(void);
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
  atexit(atexitfun);
  if(argc != 4) {
    exit(-1);
  }
  priority = atoi(argv[1]);
  line = atoi(argv[2]);
  supervisor = atoi(argv[3]);
  myPid = getpid();

  printf("Pociąg [%d] typu %d nadjeżdża na linie %d\n", myPid, priority, line);

  //OBSLUGA SYGNALOW
  struct sigaction sa, sastp;
	sa.sa_handler = handler;
	sa.sa_flags = 0;

  sastp.sa_handler = stopHandler;
  sastp.sa_flags = 0;

	err(sigaction(SIGUSR1, &sa, NULL) == -1, "sigaction()");
  err(sigaction(SIGTSTP, &sastp, NULL) == -1, "sigaction()");

  //OTWIERANIE SEMAFOROW
  char* semName = calloc(strlen(SEMNAME) + 3, sizeof(char));
  sprintf(semName, "%s%d", SEMNAME, line);

  err((lineSemaphor = sem_open(semName, O_RDWR)) == SEM_FAILED, "sem_open()");
  err((shmSemaphor = sem_open(SEMNAME, O_RDWR)) == SEM_FAILED, "sem_open()");

  //OTWIERANIE PAMIECI WSPOLNEJ
  err((fd = shm_open(SHMNAME, O_RDWR, 0660)) == -1, "shm_open()");
  err(ftruncate(fd, sizeof(struct railwayLine)) == -1, "ftruncate()");
  err((shm = (struct railwayLine*)mmap(NULL, sizeof(struct railwayLine),
      PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED, "mmap()")

  //OTWIERANIE KOLEJKI KOMUNIKATOW
  struct mq_attr attr;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = 3000;
  attr.mq_flags = O_NONBLOCK;
  char* mqName = calloc(strlen(MSGQUEUE) + 1, sizeof(char));
  sprintf(mqName, "%s%d", MSGQUEUE, line);
  err((queue = mq_open(mqName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attr)) == -1, "mq_open()")

  //Pociag ustawia się w kolejce do wyznaczonego toru (line)
  char* msg = calloc(1, sizeof(pid_t));
  sprintf(msg, "%d", myPid);
  err(mq_send(queue, msg, strlen(msg), priority) == -1, "mq_send()");

  pause();   //Czekam na pozwolenie na wjazd na tor (line)
  printf("[%d] Wjechałem na tor %d\n", myPid, line);

  sem_wait(lineSemaphor); //Czekam na opuszczenie semafora i  wjazd do tunelu
  printf("[%d] Jade przez tunel\n", myPid);
  sleep((3%priority + 1) + 1); //Predkosc dla priorytetow [1,2,3] odpowiednie [4,3,2]

  kill(supervisor, SIGUSR1); //Nadzorco, przejechałem przez tunel!
  err(sem_post(lineSemaphor) == -1, "sem_post"); //Zwalniam semafor

  exit(0);
}
//------------------------------------------------------------------------------
void handler(int signum) {

}

void stopHandler(int signum) {
  sem_close(lineSemaphor);
  sem_close(shmSemaphor);

  munmap(shm, sizeof(struct railwayLine));

  mq_close(queue);

  exit(0);
}

void atexitfun() {
  stopHandler(0);
}
