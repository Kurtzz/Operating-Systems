#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <inttypes.h>
#include "stoper.h"

clock_t start_real;
clock_t prev_real;
clock_t now_real;
struct tms start;
struct tms prev;
struct tms now;

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

void checkpoint(FILE *fp){
  prev_real = now_real;
  prev = now;

  now_real = clock();
  times(&now);

  //printf("---------------------\n");
  /*printf("Od poczatku dzialania:\n");
  printf("  Real: %g\n", (double)(now_real - start_real)/CLOCKS_PER_SEC);
  printf("  User: %g\n", (double)(now.tms_utime - start.tms_utime)/CLOCKS_PER_SEC);
  printf("  Sys: %g\n", (double)(now.tms_stime - start.tms_utime)/CLOCKS_PER_SEC);
  */
  //printf("%s:\n", title);
  //printf("  Real: %g [s]\n", (double)(now_real - prev_real)/CLOCKS_PER_SEC);
  //printf("  User: %g [ms]\n", (double)1000*(now.tms_utime - prev.tms_utime)/CLOCKS_PER_SEC);
  //printf("  Sys: %g [ms]\n", (double)1000*(now.tms_stime - prev.tms_stime)/CLOCKS_PER_SEC);
  //--fprintf(fp, "%s:\n", title);
  //fprintf(fp, "  Real: %g [s]\n", (double)(now_real - prev_real)/CLOCKS_PER_SEC);
  fprintf(fp, "\t\t\t\tUser: %g [ms]\n", (double)1000*(now.tms_utime - prev.tms_utime)/CLOCKS_PER_SEC);
  fprintf(fp, "\t\t\t\tSys: %g [ms]\n", (double)1000*(now.tms_stime - prev.tms_stime)/CLOCKS_PER_SEC);
}
