#include <stdio.h>
#include <stdlib.h> //atoi
#include <sched.h> //task struct
#include <linux/sched.h> //clone_vm, clone_vfork
#include <unistd.h> //fork
#include <sys/wait.h>
#define _GNU_SOURCE //CLONE_VM
#include "stoper.h"
#include <time.h>
#include <sys/types.h>

#include <sys/time.h>
#include <sys/resource.h>
//------------------------------------------
int counter;
int forkT(int);
int vforkT(int);
int cloneForkT(int);
int cloneVForkT(int);
void function(void);
//------------------------------------------

int main(int argc, char** argv){
  //----------------------------
  startpoint();
  if(forkT(atoi(argv[1])) != 0){
    return 1;
  }
  checkpoint("fork Test");
  //----------------------------
  //middlepoint();
  if(vforkT(atoi(argv[1])) != 0){
    return 1;
  }
  checkpoint("vfork Test");
  //----------------------------
  //middlepoint();
  if(cloneForkT(atoi(argv[1])) != 0){
    return 1;
  }
  checkpoint("clone fork Test");
  //----------------------------
  //middlepoint();
  if(cloneVForkT(atoi(argv[1])) != 0){
    return 1;
  }
  checkpoint("clone vfork Test");
  //----------------------------
  return 0;
}

int forkT(int n){
  pid_t pid;
  int i;
  int status;
  //clock_t start;
  //double rtime = 0;

  for(i=0; i<n; i++){
    if((pid = fork()) < 0){
      printf("Blad\n");
      return 1;
    } else if(pid == 0) {
      //start = clock();
      counter+=1;
      exit(0);
    }
    waitpid(pid, &status, 0);
    //rtime += WEXITSTATUS(&status);
  }
  printf("Wartosc licznika przy funkcji fork: %d\n", counter);
  //printf("Czas dzieciów: %g", rtime/CLOCKS_PER_SEC);
  return 0;
}

int vforkT(int n){
  pid_t pid;
  int i;
  int status;

  for(i=0; i<n; i++){
    if((pid = vfork()) < 0){
      printf("Blad\n");
      return 1;
    } else if(pid == 0) {
      counter+=1;
      _exit(0);
    }
    waitpid(pid, &status, 0);
  }
  printf("Wartosc licznika przy funkcji vfork: %d\n", counter);
  return 0;
}

void function(){
  counter += 1;
  _exit(0);
}

int cloneForkT(int n){
  pid_t pid;
  int i, status;
  void* childStack;
  childStack = malloc(16384);
  childStack += 16384;
  counter = 0;

  for(i = 0; i<n; i++){
    if((pid = clone(function, childStack, SIGCHLD, NULL)) < 0){
      printf("Blad przy obiegu nr: %d", i);
      return -1;
    }
    waitpid(pid, &status, 0);
  }
  //free(childStack);
  printf("Wartosc licznika przy funkcji clone (fork): %d\n", counter);
  return 0;
}

int cloneVForkT(int n){
  pid_t pid;
  int i, status;
  void* childStack;
  childStack = malloc(16384);
  childStack += 16384;
  counter = 0;

  for(i = 0; i<n; i++){
    if((pid = clone(function, childStack, SIGCHLD | CLONE_VM | CLONE_VFORK, NULL)) < 0){
      printf("Blad przy obiegu nr: %d", i);
      return -1;
    }
    waitpid(pid, &status, 0);
  }
  //free(childStack);
  printf("Wartosc licznika przy funkcji clone (vfork): %d\n", counter);
  return 0;
}
