FLAGS=-c -Wall

start: libraries pack main clear
libraries:
	gcc -fPIC $(FLAGS) lib.c -o lib.o
	gcc -fPIC $(FLAGS) sys.c -o sys.o
	gcc -fPIC $(FLAGS) stoper.c -o stoper.o

pack:
	gcc -Wall -shared -fPIC -o libzad1.so lib.o sys.o stoper.o -Wl,-rpath=`pwd`

main:
	gcc main.c -Wall -o main -L. -lzad1 -Wl,-rpath=`pwd`
	./main

clear:
	rm -f *.o *.so main file file2
