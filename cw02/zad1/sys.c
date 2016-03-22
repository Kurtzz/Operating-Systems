#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

void generujSys(char *filename, int size, int num){
  int fp;
  unsigned char *rekord = calloc(size, sizeof(char));
  int i,j;

  if((fp=open(filename, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR	| S_IWUSR	| S_IXUSR)) == -1){
    printf("Nie mogę otworzyć pliku %s do zapisu!\n", filename);
    close(fp);
    return;
  }
  srand((int)time(NULL));
  for(i=0; i<num; i++){
    for(j=0; j<size; j++){
      rekord[j]= rand() % (125 - 33) + 33; //myrandom(33,125);
    }
    write(fp, rekord, size);
  }
  close(fp);
}

void bubbleSortSys(char *filename, int size){
  char *rec1 = calloc(size, sizeof(char));
  char *rec2 = calloc(size, sizeof(char)); //tablice na rekordy z pliku
  int p, pmin, pmax, i, currPos; //zmienne pomocnicze sortowania

  int fp;
  if((fp=open(filename, O_RDWR)) == -1){
    printf("Nie mogę otworzyć pliku %s!\n", filename);
    close(fp);
    return;
  }

  pmax = lseek(fp, -size, SEEK_END);  //gorna granica pozycji sortowania
  pmin = lseek(fp, 0, SEEK_SET);  //dolna granica pozycji sortowania

  do {
    p = -1; //numer pozycji zmiany elementów
    for(i = pmin; i < pmax; i+=size) {
      currPos = lseek(fp, i, SEEK_SET);

      read(fp, rec1, size); //odczytujemy pierwszy rekord
      read(fp, rec2, size); //odczytujemy drugi rekord

      if(rec1[0] > rec2[0]) {
        lseek(fp, currPos, SEEK_SET); //pozycja pierwszego z elementów
        write(fp, rec2, size);
        write(fp, rec1, size); //Zapisujemy elementy
        if(p < 0) pmin = i;
        p = i;
      }
    }
    if(pmin) pmin -= size;
    pmax = p;
  } while(p >= 0);
}

void printFileSys(char *filename, int size, int num){
  int i;
  int fp = open(filename, O_RDONLY);
  unsigned char *data = calloc(size, sizeof(char));
  printf("\nCZYTAM PLIK\n");
  for(i=0; i < num; i++){
    read(fp, data, size);
    printf("%s\n", data);
  }
  close(fp);
}
