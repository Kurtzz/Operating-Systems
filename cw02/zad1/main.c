#include <stdio.h>
#include "sys.h"
#include "lib.h"
#include "stoper.h"

//-------------------------------------------------------------
void copy(char*, char*);
void test(int, int, FILE*);
//-------------------------------------------------------------

int main() {
  int size;
  int num;
  /*
  4, 4000
  512, 3500
  4096, 3000
  8192, 2500
  */
  FILE *fp;
  fp = fopen("wynik.txt", "a+");
  startpoint();

  size = 4;
  num = 4000;
  test(size, num, fp);

  size = 4;
  num = 3500;
  test(size, num, fp);

  //-------------------------------------------------------------

  size = 512;
  num = 3500;
  test(size, num, fp);

  size = 512;
  num = 3000;
  test(size, num, fp);

  //-------------------------------------------------------------

  size = 4096;
  num = 3000;
  test(size, num, fp);

  size = 4096;
  num = 2500;
  test(size, num, fp);

  //-------------------------------------------------------------

  size = 8192;
  num = 2500;
  test(size, num, fp);

  size = 8192;
  num = 2000;
  test(size, num, fp);

  //-------------------------------------------------------------

  fclose(fp);
  return 0;
}

void copy(char *filename1, char *filename2){
  FILE *fp1, *fp2;
  fp1 = fopen("file", "r");
  fp2 = fopen("file2", "a");
  char ch = NULL;
  while(ch != EOF){
    ch = fgetc(fp1);
    putc(ch, fp2);
  }
  fclose(fp1);
  fclose(fp2);
}

void test(int size, int num, FILE *fp){
  fprintf(fp, "SIZE = %d, NUM = %d\n", size, num);
  generujSys("file", size, num);
  copy("file", "file2");
  middlepoint();
  fprintf(fp, "\t\tFunckje systemowe:\n");
  bubbleSortSys("file", size);
  checkpoint(fp);
  fprintf(fp, "\t\tFunckje biblioteczne:\n");
  bubbleSortF("file2", size);
  checkpoint(fp);
  fprintf(fp, "\n");
}
