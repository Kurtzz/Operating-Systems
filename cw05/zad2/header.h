#ifndef HEADER_H
#define HEADER_H

#define TRY(CALL, message) \
if((CALL) == -1) { \
    perror((message)); \
    exit(EXIT_FAILURE); \
}

#define BUFFER_SIZE 2048
#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#endif
