#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
  pid_t pid;
  pid = fork();
  if(pid == 0){
    execl("./hello", "hello", NULL);
  }
  return 0;
}
