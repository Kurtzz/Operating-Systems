#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//-------------------------------
int counter = 0;
void handler1(int);
void handler2(int);
//-------------------------------

int main(int argc, char **argv) {
  struct sigaction sa1, sa2;
	sa1.sa_handler = handler1;
	sa1.sa_flags = 0;
	sa2.sa_handler = handler2;
	sa2.sa_flags = 0;
	if(sigaction(SIGUSR1, &sa1, NULL) == -1 || sigaction(SIGUSR2, &sa2, NULL) == -1) {
		printf("Blad sigaction (child)!\n");
		exit(1);
	}
	sigset_t pusty;
	sigemptyset(&pusty);

	while(1) sigsuspend(&pusty);

	return 0;
}

void handler1(int signum){
  counter++;
}

void handler2(int signum){
  pid_t ppid = getppid();
  while(counter) {
    if((kill(ppid, SIGUSR1)) != 0){
      printf("Błąd funkcji kill (child)!\n");
      exit(1);
    }
    counter--;
  }
  if(kill(ppid, SIGUSR2) != 0){
		printf("Błąd funkcji kill (child)");
		exit(1);
	}
  exit(0);
}
