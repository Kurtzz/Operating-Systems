FLAGS=-c -Wall -ansi -pedantic

static: mylib.c
	gcc $(FLAGS) mylib.c -o mylib.o
	ar crs libmylib.a mylib.o
	rm -f *.o

dynamic: mylib.c
	gcc -fPIC $(FLAGS) mylib.c -o mylib.o
	gcc -Wall -shared -fPIC -o libmylib.so mylib.o -Wl,-rpath=`pwd`
	rm -f *.o
