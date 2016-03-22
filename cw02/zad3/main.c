#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
//-------------------------------------------------------------
void help(void);
int readLock(int fileDesc, int offset);
int writeLock(int fileDesc, int offset);
int unlock(int fileDesc, int offset);
int listOfLocks(int fileDesc);
char readChar(int fileDesc, int offset);
int writeChar(int fileDesc, int offset, char sign[]);
int lock_reg(int fileDesc, int cmd, int type, off_t offset, int whence, off_t len);
//-------------------------------------------------------------
int main(int argc, char* argv[]){
    int end, fileDesc;
    char* line;
    int offset;
    char ch;
    offset = 0;
    end=1;
    line = calloc(1, 200);

    if((fileDesc = open(argv[1], O_RDWR)) == -1){
        printf("Blad przy otwieraniu pliku: \"%s\"\terrno: %d\n",strerror(errno),errno);
        free(line);
        return 1;
    }

    while(end){
        printf("locker> ");
        scanf("%s", line);

        if(!strcmp(line, "help")){
            help();
        }else if(!strcmp(line, "readLock")){
            printf("Podaj offset: ");
            scanf("%d", &offset);
            if((readLock(fileDesc, offset)) == -1){
                end = 1;
            }
        }else if(!strcmp(line, "writeLock")){
            printf("Podaj offset: ");
            scanf("%d", &offset);
            if((writeLock(fileDesc, offset)) == -1){
                end = 1;
            }
        }else if(!strcmp(line, "unlock")){
            printf("Podaj offset: ");
            scanf("%d", &offset);
            if((unlock(fileDesc, offset)) == -1){
                end = 1;
            }
        }else if(!strcmp(line, "listOfLocks")){
            if(listOfLocks(fileDesc) == -1){
                end = 1;
            }
        }else if(!strcmp(line, "readChar")){
            printf("Podaj offset: ");
            scanf("%d", &offset);
            if((ch = readChar(fileDesc, offset)) == -1){
                end = 1;
            }else{
                printf("%c\n", ch);
            }
        }else if(!strcmp(line, "writeChar")){
	          char znak[2];
	          printf("Podaj offset: ");
            scanf("%d", &offset);

            printf("Podaj znak: ");
            scanf("%1s", znak);

            if((writeChar(fileDesc, offset, znak)) == -1){
                end = 1;
            }

        }else if(!strcmp(line, "exit")){
            end = 0;
        }else{
            printf("Bledna komenda\n");
            printf("Wpisz 'help' jezeli potrzebujesz liste komend\n");
        }
    }
    free(line);
    close(fileDesc);
    return 0;
}

//-------------------------------------------------------------
int writeChar(int fileDesc, int offset, char sign[]){
    struct flock lock;

    lock.l_len = 1;
    lock.l_start = offset;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    if(fcntl(fileDesc, F_GETLK, &lock) == -1){
        printf("Blad w funkcji writeChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if(lock.l_type != F_UNLCK){
        printf("Na ten znak zalozona jest blokada i nie da sie zapisywac\n");
        return -1;
    }

    if(lseek(fileDesc, offset, SEEK_SET) == -1){
        printf("Blad w funkcji writeChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if(write(fileDesc, sign, 1) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    return 0;
}

char readChar(int fileDesc, int offset){
    struct flock lock;
    char ch[2];

    lock.l_len = 1;
    lock.l_start = offset;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;

    if(fcntl(fileDesc, F_GETLK, &lock) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if(lock.l_type == F_WRLCK){
        printf("Na ten znak zalozona jest blokada i nie da sie go odczytac\n");
        return -1;
    }

    if(lseek(fileDesc, offset, SEEK_SET) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }

    if((read(fileDesc, &ch, 1)) == -1){
        printf("Blad w funkcji readChar: \"%s\"\terrno: %d\n",strerror(errno),errno);
        return -1;
    }
	//printf("%c\n", znak);
    return ch[0];
}

int listOfLocks(int fileDesc){
    int i;
    struct flock lock;
    int size;

    size = lseek(fileDesc, 0, SEEK_END);
    lseek(fileDesc, 0, SEEK_SET);

    for(i=0 ; i<size ; ++i){
        //test rygla odczytu
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fileDesc, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }

        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade odczytu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
        //test rygla zapisu
        lock.l_len = 1;
        lock.l_start = i;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;

        if(fcntl(fileDesc, F_GETLK, &lock) == -1){
            printf("Blad w funkcji listOfLocks: \"%s\"\terrno: %d\n",strerror(errno),errno);
            return -1;
        }

        if(lock.l_type != F_UNLCK){
            printf("PID %d: ma blokade zapisu na bajcie %d\n", (int)lock.l_pid, (int)lock.l_start);
        }
    }

    return 0;
}

int unlock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_UNLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System zdjal juz blokade z tego bajtu\n");
        }else{
            printf("Blad w funkcji unlock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}


int writeLock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_RDLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System przekazal juz blokade odczytu na dany znak pliku\n");
        }else{
            printf("Blad w funkcji writeLock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}

int readLock(int fileDesc, int offset){
    if((lock_reg(fileDesc, F_SETLK, F_WRLCK, offset, SEEK_SET, 1)) == -1){
        if(errno == EACCES || errno == EAGAIN){
            printf("System przekazal juz blokade zapisu na dany znak pliku\n");
        }else{
            printf("Blad w funkcji readLock: \"%s\"\terrno: %d\n", strerror(errno), errno);
            return -1;
        }
    }
    return 0;
}

void help(){
    printf("\nKomendy:\n\n"
                "readLock\t: ustawienie rygla do odczytu na dany znak\n"
                "writeLock\t: ustawienie rygla do zapisu na dany znak\n"
                "unlock\t\t: zwolnienie wybranego rygla\n"
                "listOfLocks\t: wyswietla liste zaryglowanych znakow\n"
                "readChar\t: odczyt wybranego znaku pliku\n"
                "writeChar\t: zmiana wybranego znaku pliku\n"
                "exit\t\t: opuszczenei programu\n"
                "help\t\t: wyswietla liste dostepnych komend\n"
                "\n");
}

int lock_reg(int fileDesc, int cmd, int type, off_t offset, int whence, off_t len){
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return fcntl(fileDesc, cmd, &lock);
}
