#include <stdio.h> //printf
#include <stdlib.h> //calloc
#include <dirent.h> //struct dirent
#include <sys/stat.h> //stat()
#include <string.h> //strlen, strcmp
#include <sys/types.h>
#include <inttypes.h> //intmax_t
#include <time.h> //ctime()

//-------------------------------------------------------------
int permissioncmp(mode_t, char*);
void printFiles(char*, char*);
//-------------------------------------------------------------

int main(int argc, char **argv){
  if(argc != 3) {
    printf("Zla liczba argumentow!\n");
    return 1;
  }
	printf("\nNazwa pliku    Rozmiar pliku  Data ostatniej modyfikacji\n");
  printFiles(argv[1], argv[2]);
	return 0;
}

int permissioncmp(mode_t permissions, char * givenpermissions) {
	if(givenpermissions == NULL || strlen(givenpermissions) != 9)
		return -1;
	char permissionsStr[] = {
		permissions & S_IRUSR ? 'r' : '-',
		permissions & S_IWUSR ? 'w' : '-',
		permissions & S_IXUSR ? 'x' : '-',
		permissions & S_IRGRP ? 'r' : '-',
		permissions & S_IWGRP ? 'w' : '-',
		permissions & S_IXGRP ? 'x' : '-',
		permissions & S_IROTH ? 'r' : '-',
		permissions & S_IWOTH ? 'w' : '-',
		permissions & S_IXOTH ? 'x' : '-',
		'\0'
	};
	if(strcmp(permissionsStr, givenpermissions) == 0)
		return 0;
	else return 1;
}

void printFiles(char *path, char *givenpermissions) {
	DIR *dir; //handler katalogu w ktorym sie znajdujemy
  struct dirent *entry;
	struct stat info;
  char *epath = calloc(1024, sizeof(char)); //pelna sciezka do entry

 	if((dir = opendir(path)) == NULL){ //Otwieramy katalog
     printf("Nie mogę otworzyć");
     return;
   }
   while((entry = readdir(dir)) != NULL){ //przegladamy kolejne pozycje
     strcpy(epath, path);
     strcat(epath, "/");
     strcat(epath, entry->d_name);
		 if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
		   continue;
		 }
     if(lstat(epath, &info) < 0){ //probujemy zdobyc info o pozycji
       printf("error stat()");
       return;
     }

     if(S_ISDIR(info.st_mode)){
			 //jezeli pozycja jest katalogiem wchodzimy glebiej
       printFiles(epath, givenpermissions);
     } else if (S_ISREG(info.st_mode) && (permissioncmp(info.st_mode, givenpermissions)) == 0){
					//jezeli jest to plik regularny && prawa sie zgadzaja - drukujemy
					printf("%-15s%-15jd%-10s", entry->d_name, (intmax_t)info.st_size, ctime(&info.st_mtime));
     }
   }
   closedir(dir);
}
