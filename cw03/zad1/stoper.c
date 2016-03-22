#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "stoper.h"

//Czasy rzeczywiste
clock_t start_real;
clock_t prev_real;
clock_t now_real;

//Czasy system i user
struct tms start;
struct tms prev;
struct tms now;

struct rusage usage;

void startpoint(){
  start_real = clock();
  times(&start);

  now_real = start_real;
  now = start;
}

void middlepoint(){
  prev_real = now_real;
  prev = now;

  now_real = clock();
  times(&now);
}

void checkpoint(char* title){
  prev_real = now_real;
  prev = now;

  now_real = clock();
  times(&now);

  /*printf("Od poczatku dzialania:\n");
  printf("  Real: %g\n", (double)(now_real - start_real)/CLOCKS_PER_SEC);
  printf("  User: %g\n", (double)(now.tms_utime - start.tms_utime)/CLOCKS_PER_SEC);
  printf("  Sys: %g\n", (double)(now.tms_stime - start.tms_utime)/CLOCKS_PER_SEC);
  */
  printf("%s:\n", title);
  printf("  Real: %g [s]\n", (double)(now_real - prev_real)/CLOCKS_PER_SEC);
  printf("  User: %g [ms]\n", (double)1000*(now.tms_utime - prev.tms_utime)/CLOCKS_PER_SEC);
  printf("  Sys: %g [ms]\n", (double)1000*(now.tms_stime - prev.tms_stime)/CLOCKS_PER_SEC);
  printf("  User (child): %g [ms]\n", (double)1000*(now.tms_cutime - prev.tms_cutime)/CLOCKS_PER_SEC);
  printf("  Sys (child): %g [ms]\n", (double)1000*(now.tms_cstime - prev.tms_cstime)/CLOCKS_PER_SEC);
  printf("---------------------\n");
}
