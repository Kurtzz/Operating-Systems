#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define WAITING_TIME 15

void printStats(char *path, int own, int sum) {
	if(path == NULL) return;
	printf("Sciezka: %s\n\tLiczba plikow wlasnych: %d\n\tLaczna liczba plikow: %d\n",
		path, own, sum);
}

int main(int argc, char ** argv) {
	if(argc > 3) _exit(-1);

	char *ptb; //Path To Browse
  //setenv("PATH_TO_BROWSE", "/home/kurtz/Pulpit/", 0);
	if((ptb = getenv("PATH_TO_BROWSE")) == NULL) ptb = "./";

	int print = 0, waitt = 0;
	if(argc == 2) {
		if(strcmp(argv[1], "-p") == 0) print = 1;
		else if(strcmp(argv[1], "-w") == 0) waitt = 1;
	}
	else if(argc == 3)
		if((strcmp(argv[1], "-p") == 0 && strcmp(argv[2], "-w") == 0)
			|| (strcmp(argv[2], "-p") == 0 && strcmp(argv[1], "-w") == 0)) {
			print = 1; waitt = 1;
		}


	DIR * dp;
	if((dp = opendir(ptb)) == NULL) _exit(-2);

	struct dirent * ep;
	struct stat statbuf;
	pid_t pid;

	int files = 0, subdirs = 0, filesInSubdirs = 0;

	while((ep = readdir(dp)) != NULL) {
		char str[strlen(ptb)+strlen(ep->d_name)+3];
		strcpy(str, ptb);

		if(str[strlen(ptb)-1] != '/')
			strcat(str, "/");

		strcat(str, ep->d_name);
		if(stat(str, &statbuf) == -1)
			_exit(-3);

		if(strcmp(ep->d_name,".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;

		if(S_ISDIR(statbuf.st_mode)) {
			subdirs++;

			if((pid = fork()) == -1)
			    _exit(-4);
			else if(pid == 0) {
				setenv("PATH_TO_BROWSE", str, 1);
				if(print && waitt) {
					if(execlp(argv[0], argv[0], "-p", "-w", NULL) == -1)
						_exit(-5);
				}
				else if(print) {
					if(execlp(argv[0], argv[0], "-p", NULL) == -1)
						_exit(-6);
				}
				else if(waitt) {
					if(execlp(argv[0], argv[0], "-w", NULL) == -1)
						_exit(-7);
				}
				else
					if(execlp(argv[0], argv[0], NULL) == -1)
						_exit(-8);
			}
		}

		else if(S_ISREG(statbuf.st_mode))
		 		files++;
	}
	closedir(dp);
	if(waitt) sleep(WAITING_TIME);

	int tmp, numOfFiles;
	while(subdirs > 0) {
		wait(&tmp);
		if((numOfFiles = WEXITSTATUS(tmp)) < 0)
			_exit(numOfFiles);
		filesInSubdirs += numOfFiles;
		subdirs--;
	}

	if(print)
	    printStats(ptb, files, filesInSubdirs + files);
	_exit(files + filesInSubdirs);
}
