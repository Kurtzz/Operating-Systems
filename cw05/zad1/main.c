#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

//-----------------
int pid;
int pipe1[2];
int pipe2[2];
//-----------------
void ls(void);
void grep(void);
void wc(void);
//-----------------

int main() {
  if (pipe(pipe1) == -1) {
    perror("pipe(pipe1) error");
    exit(1);
  }

  if ((pid = fork()) == -1) {
    perror("bad fork1");
    exit(1);
  } else if (pid == 0) {
    // input from stdin (already done)
    dup2(pipe1[1], 1); // output to pipe1
    close(pipe1[0]); // close fds
    close(pipe1[1]);
    execlp("ls", "ls", "-l", NULL);
  }

  if (pipe(pipe2) == -1) {
    perror("pipe(pipe2) error");
    exit(1);
  }

  if ((pid = fork()) == -1) {
    perror("fork() error");
    exit(1);
  } else if (pid == 0) {
    dup2(pipe1[0], 0); // input from pipe1
    dup2(pipe2[1], 1); // output to pipe2
    close(pipe1[0]); // close fds
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    execlp("grep", "grep", "^d", NULL);
  }

  close(pipe1[0]);
  close(pipe1[1]);

  if ((pid = fork()) == -1) {
    perror("fork() error");
    exit(1);
  } else if (pid == 0) {
    dup2(pipe2[0], 0); // input from pipe2
    // output to stdout (already done)
    close(pipe2[0]); // close fds
    close(pipe2[1]);
    execlp("wc", "wc", "-l", NULL);
  }

  return 0;
}
