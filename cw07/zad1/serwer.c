#include "header.h"
//-----------------------------------------------------
int semafor;
int memory;
key_t semaforKey;
key_t memoryKey;
void* pamiec;
int dzialaj;
//-----------------------------------------------------
void signalHandler(int signal);
//-----------------------------------------------------
int main(int argc, char* argv[]){

    if(argc != 1){
        printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa_programu \n");
        return 1;
    }
    if(signal(SIGTSTP, signalHandler) == SIG_ERR){
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    dzialaj = 1;

    if((semaforKey = ftok(".", 1)) == -1){
        printf("ftok(semaforKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    if((memoryKey = ftok(".", 2)) == -1){
        printf("ftok(memoryKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    if((semafor = semget(semaforKey, MAXTASKS + 1, IPC_CREAT | IPC_EXCL | 0666)) == -1){
        if((semafor = semget(semaforKey, MAXTASKS + 1, 0)) == -1){
            printf("semget(semaforKey): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    if((memory = shmget(memoryKey, sizeof(struct sharedMemo), IPC_CREAT | 0666)) == -1){
        printf("shmget(memoryKey): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    if((pamiec = shmat(memory, NULL, 0)) < 0){
        printf("shmat(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    union semun semum1;
    semum1.val = 1;
    int i;

    for(i = 0; i < MAXTASKS + 1; i++){
        if(semctl(semafor, i, SETVAL, semum1) == -1){
            printf("semctl(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    printf("Serwer uruchomiony\nDo zakonczenia uzyj: CTRL + Z\n");

    while(dzialaj);

    shmctl(memory, IPC_RMID, NULL);
    semctl(semafor, 0, IPC_RMID);
    shmdt(pamiec);

    return 0;
}
//-----------------------------------------------------
void signalHandler(int signal){
    dzialaj = 0;
    printf("\n");
}
//-----------------------------------------------------
