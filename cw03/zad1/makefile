FLAGS = -c -Wall -std=c99


main:
		gcc -fPIC $(FLAGS) stoper.c -o stoper.o
		gcc -Wall -std=c99 -shared -fPIC -o liblib.so stoper.o -Wl,-rpath=`pwd`
		gcc main.c -Wall -o main -L. -llib -Wl,-rpath=`pwd`
		./main 100000
		rm -f *.o *.so main
