HEADERS = helper.h

default: main-serial main-parallel

main-serial.o: serial.c $(HEADERS)
	gcc -c serial.c -o main-serial.o


main-parallel: 
	mpicc -c main.c; mpicc -o main-parallel main.o -lm; mpicc -o main-parallel main.c -lm

main-serial: main-serial.o
	gcc main-serial.o -o main-serial -lm

clean:
	-rm -f main-serial.o
	-rm -f main-serial
	-rm -f main.o
	-rm -f main-parallel
	
