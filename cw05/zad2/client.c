#include "header.h"

volatile sig_atomic_t stopCalled = 0;

void signalHandler(int signalNo) {
    if(signalNo == SIGTSTP) {
        stopCalled = 1;
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stdout, "Bad call to program. Use: ./client {FIFO name}\n");
        exit(EXIT_FAILURE);
    }
    if(signal(SIGTSTP, signalHandler) == SIG_ERR) {
        perror("signal() error");
    }
    char *pipeName = argv[1];
    int pipeDescriptor;

    TRY((pipeDescriptor = open(pipeName, O_WRONLY)), "open() error");

    char message[BUFFER_SIZE];
    char pid_s[8];

    snprintf(pid_s, sizeof(pid_s), "%d", (int)getpid());
    while(!stopCalled) {
        fgets(message, sizeof message, stdin);
        if(write(pipeDescriptor, pid_s, sizeof(pid_s)) != sizeof(pid_s)
        || write(pipeDescriptor, message, BUFFER_SIZE) != BUFFER_SIZE) {
            perror("write() error");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "Sent!\n");
    }

    TRY(close(pipeDescriptor), "close() error");
    return 0;
}
