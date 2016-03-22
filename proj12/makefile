main: clean
	gcc -Wall -o generator generator.c
	gcc -Wall -o supervisor supervisor.c -lrt -lpthread
	gcc -Wall -o train train.c -lrt -lpthread
	gcc -Wall -o preview preview.c -lrt -lpthread


clean:
	rm -f generator supervisor train preview
