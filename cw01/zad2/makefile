FLAGS=-c -Wall -ansi -pedantic

static: mylib.c
	gcc $(FLAGS) mylib.c -o mylib.o
	ar crs libmylib.a mylib.o
	gcc -Wall main.c -o main -L. -lmylib
	./main
	rm -f *.o *.a main

dynamic: mylib.c
	gcc -fPIC $(FLAGS) mylib.c -o mylib.o
	gcc -Wall -shared -fPIC -o libmylib.so mylib.o -Wl,-rpath=`pwd`
	gcc main.c -Wall -o main -L. -lmylib -Wl,-rpath=`pwd`
	./main
	rm -f *.o *.so main

DLL: mylib.c
	gcc -fPIC  $(FLAGS) mylib.c -o mylib.o
	gcc -Wall -shared -fPIC -o libmylib.so mylib.o -Wl,-rpath=`pwd`
	gcc -Wall main.c -o main -L. -ldl -D DLL
	./main
	rm -f *.o *.so main
