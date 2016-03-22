#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generujF(char *filename, int size, int num){
  FILE *fp;
  unsigned char *rekord = calloc(size, sizeof(char));
  int i,j;

  if((fp=fopen(filename,"a")) == NULL){
    printf("Nie mogę otworzyć pliku %s do zapisu!\n", filename);
    fclose(fp);
    return;
  }
  srand((int)time(NULL));
  for(i=0; i<num; i++){
    for(j=0; j<size; j++){
      rekord[j]= rand() % (125 - 33) + 33; //myrandom(33,125);
    }
    fwrite(rekord, sizeof(char), size, fp);
  }
  fclose(fp);
}

void bubbleSortF(char *filename, int size){
  char *rec1 = calloc(size, sizeof(char));
  char *rec2 = calloc(size, sizeof(char)); //tablice na rekordy z pliku
  int p, pmin, pmax, i, currPos; //zmienne pomocnicze sortowania

  FILE *fp;
  if((fp=fopen(filename,"r+")) == NULL){
    printf("Nie mogę otworzyć pliku %s!\n", filename);
    fclose(fp);
    return;
  }

  fseek(fp, -size, 2); pmax = ftell(fp); //gorna granica pozycji sortowania
  fseek(fp, 0, 0); pmin = ftell(fp); //dolna granica pozycji sortowania

  do {
    p = -1; //numer pozycji zmiany elementów
    for(i = pmin; i < pmax; i+=size) {
      fseek(fp, i, 0);

      currPos = ftell(fp); //aktualna pozycja pierwszego elementu
      fread(rec1, sizeof(char), size, fp); //odczytujemy pierwszy rekord
      fread(rec2, sizeof(char), size, fp); //odczytujemy drugi rekord

      if(rec1[0] > rec2[0]) {
        fseek(fp, currPos, 0); //pozycja pierwszego z elementów
        fwrite(rec2, sizeof(char), size, fp);
        fwrite(rec1, sizeof(char), size, fp); //Zapisujemy elementy
        if(p < 0) pmin = i;
        p = i;
      }
    }
    if(pmin) pmin-=size;
    pmax = p;
  } while(p >= 0);
}

void printFileF(char *filename, int size, int num){
  int i;
  FILE *fp = fopen(filename, "r");
  unsigned char *data = calloc(size, sizeof(char));
  printf("\nCZYTAM PLIK\n");
  for(i=0; i < num; i++){
    fread(data, sizeof(char), size, fp);
    printf("%s\n", data);
  }
  fclose(fp);
}
