#include "header.h"

volatile sig_atomic_t stopCalled = 0;

void signalHandler(int signalNo) {
    if(signalNo == SIGTSTP) {
        fprintf(stdout, "\nReceived SIGTSTOP signal. Waiting for all clients to quit!\n");
        stopCalled = 1;
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stdout, "Bad call to program. Use: ./server {FIFO name}\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGTSTP, signalHandler) == SIG_ERR) {
        perror("signal() error");
    }
    char *pipeName = argv[1];
    int pipeDescriptor;

    TRY(mkfifo(pipeName, 0666), "mkfifo() error");
    TRY((pipeDescriptor = open(pipeName, O_RDONLY)), "open() error");

    char message[BUFFER_SIZE];
    char pid_s[8];
    time_t timer;
    struct tm *tm_p;

    while(!stopCalled) {
        if(read(pipeDescriptor, pid_s, sizeof(pid_s)) == 0
          || read(pipeDescriptor, message, BUFFER_SIZE) == 0) {
            continue;
        }

        timer = time(0);
        tm_p = localtime(&timer);
        fprintf(stdout, "Time gotten message: %.2d:%.2d:%.2d\n",
                tm_p->tm_hour,
                tm_p->tm_min,
                tm_p->tm_sec);
        fprintf(stdout, "Client PID: %s\n", pid_s);
        timer = time(0);
        tm_p = localtime(&timer);
        fprintf(stdout, "Time written message: %.2d:%.2d:%.2d\n",
                tm_p->tm_hour,
                tm_p->tm_min,
                tm_p->tm_sec);
        fprintf(stdout, "Message: %s\n", message);
    }

    TRY(close(pipeDescriptor), "close() error");
    TRY(unlink(pipeName), "unlink() error");
    fprintf(stdout, "FIFO closed!\n");

    return 0;
}
