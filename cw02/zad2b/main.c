#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <sys/stat.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>

//-------------------------------------------------------
char *rights;
int permissioncmp(mode_t, char*);
char *getFilename(const char*);
int ftwfunction(const char*, const struct stat*, int);
//-------------------------------------------------------

int main(int argc, char **argv){
  if(argc != 3 || strlen(argv[2])!=9) {
    printf("zla liczba argumentow!\n");
    return 1;
  }
  rights = calloc(9, sizeof(char));
  rights = argv[2];
	printf("\nNazwa pliku    Rozmiar pliku  Data ostatniej modyfikacji\n");
  ftw(argv[1], ftwfunction, 1);
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

char *getFilename(const char *path){
  char *first = strrchr(path, 47);
  char *filename = calloc(20, sizeof(char));
  strcpy(filename, first+1);
  return filename;
}

int ftwfunction(const char *path, const struct stat *info, int type){
  if((type == FTW_F) && (permissioncmp(info->st_mode, rights))==0){
    printf("%-15s%-15jd%-10s", getFilename(path), (intmax_t)info->st_size, ctime(&info->st_mtime));
  }
  return 0;
}
