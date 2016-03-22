#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int counter = 0;

pid_t fork();

void handler(int signum){
  counter++;
  printf("Counter: %d\n", counter);
}

int main(){
  pid_t pid;

  //pid = fork();
  if((pid = fork()) == 0){
    if(signal(SIGUSR1, handler) == SIG_ERR) {
        printf("Parent: Unable to create handler for SIGUSR1\n");
    }
    for( ;; );
  } else {
    for(;;){
      sleep(1);
      kill(pid, SIGUSR1);
    }
  }
  return 0;
}
