#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <inttypes.h>
#include <dlfcn.h>
#include "mylib.h"

clock_t start_real;
clock_t prev_real;
clock_t now_real;
struct tms start;
struct tms prev;
struct tms now;

void checkpoint()
{
  prev_real = now_real;
  //prev.tms_utime = now.tms_utime;
  //prev.tms_stime = now.tms_stime;
  prev.tms_utime = now_real;
  prev.tms_stime = now_real;

  //now_real = times(&now);
  now_real = clock();

  printf("---------------------\n");
  printf("Od poczatku dzialania:\n");
  printf("  Real: %g\n", (double)(now_real - start_real)/CLOCKS_PER_SEC);
  printf("  User: %g\n", (double)(now.tms_utime - start.tms_utime)/CLOCKS_PER_SEC);
  printf("  Sys: %g\n", (double)(now.tms_stime - start.tms_utime)/CLOCKS_PER_SEC);
  printf("Od poprzedniego checkpointu:\n");
  printf("  Real: %g\n", (double)(now_real - prev_real)/CLOCKS_PER_SEC);
  printf("  User: %g\n", (double)(now.tms_utime - prev.tms_utime)/CLOCKS_PER_SEC);
  printf("  Sys: %g\n", (double)(now.tms_stime - prev.tms_utime)/CLOCKS_PER_SEC);
}

void mysleep(){
  int i=0;
  for(i=0; i<100000000; i++){
    i++; i--; i++; i--;
  }
}

int main()
{
  #ifdef DLL
    void *handle = dlopen("./libmylib.so", RTLD_LAZY);
    void (*addContact)(DCList*, Node*) = dlsym(handle, "addContact");
    Node* (*createElement)(char*, char*, int, int, int, char*, char*, char*) = dlsym(handle, "createElement");
    void (*sortlist)(DCList*) = dlsym(handle, "sortlist");
    void (*find)(DCList*, char*, char*) = dlsym(handle, "find");
    DCList* (*createList)(void) = dlsym(handle, "createList");
    void (*removeContact)(DCList*, Node*) = dlsym(handle, "removeContact");
  #endif

  DCList *list = createList();

  //start_real = times(&start);
  start_real = clock();
  now_real = start_real;
  //now.tms_utime = start.tms_utime;
  //now.tms_stime = start.tms_stime;
  now.tms_utime = start_real;
  now.tms_stime = start_real;
  printf("\nCZAS START!\n");
  /*#########################
  ## DODAWANIE 7 KONTAKTÓW ##
  #########################*/
  checkpoint();
  printf("\nDODAWANIE KONTAKTÓW\n");
  addContact(list, createElement("Jan", "Kowalski", 21, 1, 1994,  "adres1", "mail1@example.com", "217486548"));
  addContact(list, createElement("Zbyszek", "Kowalski", 4, 12, 2015, "adres2", "mail2@example.com", "666777444"));
  addContact(list, createElement("Robert", "Downey Jr", 11, 4, 2015, "adres3", "mail3@example.com", "111"));
  addContact(list, createElement("Aleks", "Zabawa", 2, 4, 1789, "adres4", "mail4@example.com", "45679"));
  addContact(list, createElement("Jakub", "Nowicki", 7, 4, 3003, "adres5", "mail5@example.com", "7893456"));
  addContact(list, createElement("Martyna", "Jaworska", 24, 10, 1996, "adres6", "mail6@example.com", "4125679"));
  addContact(list, createElement("Jakub", "Nowicki", 12, 5, 4004, "adres7", "mail7@example.com", "45978679"));
  checkpoint();

  printf("\nBREAK\n");
  mysleep();
  checkpoint();
  /*####################
  ## SORTOWANIE LISTY ##
  ####################*/
  printf("\nSORTOWANIE KONTAKTÓW\n");
  sortlist(list);
  checkpoint();

  printf("\nBREAK\n");
  mysleep();
  checkpoint();
  /*#########################
  ## WYSZUKIWANIE ELEMENTU ##
  #########################*/
  printf("\nSZUKANIE KONTAKTU\n");
  find(list, "Jakub", "Nowicki");
  checkpoint();

  printf("\nBREAK\n");
  mysleep();
  checkpoint();
  /*###########################
  ## USUWANIE ELEMENTU LISTY ##
  ###########################*/
  printf("\nUSUWANIE KONTAKTU\n");
  removeContact(list, list->head->next->next);
  checkpoint();

  #ifdef DLL
    dlclose(handle);
  #endif
return 0;
}
