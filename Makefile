all: myls clean

myls: myls.o options.o
	gcc myls.o options.o -o myls

myls.o: myls.c lsheader.h
	gcc -c myls.c -o myls.o 

options.o: options.c lsheader.h
	gcc -c  options.c -o options.o

clean:
	rm -f *.o
