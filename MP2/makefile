# makefile

all: memtest

ackerman.o: ackerman.c
	g++ -c -g ackerman.c -w -fpermissive

my_allocator.o : my_allocator.c
	g++ -c -g my_allocator.c -w -fpermissive

memtest.o : memtest.c
	g++ -c -g memtest.c -w -fpermissive

memtest: memtest.o ackerman.o my_allocator.o
	g++ -o memtest memtest.o ackerman.o my_allocator.o -w -fpermissive
