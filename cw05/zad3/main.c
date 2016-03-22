#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
     FILE *fpin;
     FILE *file;
     int size = 128;
     char buf[size];

     fpin = popen("ls -l | grep ^-", "r");
     file = fopen("folders.txt", "a");
     while (fgets(buf, size, fpin)){
       fputs(buf, file);
     }

     fclose(fpin);
     fclose(file);
     return 0;
}
