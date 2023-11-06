final.elf : main.o 1.o 2.o 3.o
	gcc main.o 1.o -o final.elf

main.o : main.c
	gcc -c main.c

1.o : 1.c
	gcc -c 1.c

2.o : 2.c
	gcc -c 2.c

3.o : 3.o
	gcc -c 3.c	
