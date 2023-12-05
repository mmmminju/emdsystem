all: libMyPeri.a

libMyPeri.a: button.o led.o buzzer.o
	arm-linux-gnueabihf-ar rc libMyPeri.a led.o button.o buzzer.o
button.o: button.h button.c
	arm-linux-gnueabihf-gcc -c button.c -o button.o
led.o: led.h led.c
	arm-linux-gnueabihf-gcc -c led.c -o led.o
buzzer.o: buzzer.h buzzer.c
	arm-linux-gnueabihf-gcc -c buzzer.c -o buzzer.o
clean: 
	rm -f *.o *.a
