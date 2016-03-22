#include "header.h"
//------------------------------------------------------------------------------
int doYourJob;
pid_t pid;
//------------------------------------------------------------------------------
void stopHandler(int);
//------------------------------------------------------------------------------
int main() {
  srand(time(NULL));

  char* supervisor = calloc(1, sizeof(pid_t)); //pid supervisora
  char* priority = calloc(1, sizeof(char)); //priorytet pociągu
  char* line = calloc(1, sizeof(char)); //numer lini, na którą zmierza
  sprintf(supervisor, "%d", getppid());
  doYourJob = 1;

  //OBSLUGA SYGNALOW
  struct sigaction sastp;

  sastp.sa_handler = stopHandler;
  sastp.sa_flags = 0;

  err(sigaction(SIGTSTP, &sastp, NULL) == -1, "sigaction()");

  //GENERATOR ROZPOCZYNA PRACE
  while(doYourJob) {
    sprintf(priority, "%d",rand() % 3 + 1); // rand [1,3]
    sprintf(line, "%d", rand() % NLINES); // rand [0, NLINES - 1]
    err((pid = fork()) == -1, "fork()");
    if(pid == 0) {
      err(execl("./train", "train", priority, line, supervisor, NULL) == -1, "execl");
    }
    sleep(rand() % 2 + 1); //sleep [1,2] s
  }

  //CLEAN
  free(supervisor);
  free(priority);
  free(line);

  exit(0);
}

void stopHandler(int signum) {
  doYourJob = 0;
  //exit()
}
