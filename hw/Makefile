CC = arm-linux-gnueabi-gcc --static

all: libMyPeri.a ledtest buzzertest buttontest textlcdtest fndtest colorledtest accelMagGyrotest temperatureTest

ledtest : ledtest.c libMyPeri.a
	$(CC) ledtest.c -o ledtest -l MyPeri -L. -lpthread
	
buzzertest : buzzertest.c libMyPeri.a
	$(CC) buzzertest.c -o buzzertest -l MyPeri -L. -lpthread
buttontest : buttontest.c libMyPeri.a
	$(CC) buttontest.c -o buttontest -l MyPeri -L. -lpthread
textlcdtest : textlcdtest.c libMyPeri.a
	$(CC) textlcdtest.c -o textlcdtest -l MyPeri -L. -lpthread
fndtest1 : fndtest.c libMyPeri.a
	$(CC) fndtest.c -o fndtest1 -l MyPeri -L. -lpthread

colorledtest : colorledtest.c libMyPeri.a
	$(CC) colorledtest.c -o colorledtest -l MyPeri -L. -lpthread
accelMagGyrotest : accelMagGyrotest.c libMyPeri.a
	$(CC) accelMagGyrotest.c -o accelMagGyrotest -l MyPeri -L. -lpthread
temperatureTest : temperatureTest.c libMyPeri.a
	$(CC) temperatureTest.c -o temperatureTest -l MyPeri -L. -lpthread

libMyPeri.a: button.o led.o buzzer.o fnd.o textlcd.o colorled.o temperature.o accelMagGyro.o
	arm-linux-gnueabi-ar rc libMyPeri.a led.o button.o buzzer.o fnd.o textlcd.o colorled.o temperature.o accelMagGyro.o
button.o: button.h button.c
	arm-linux-gnueabi-gcc --static -c button.c -o button.o
led.o: led.h led.c
	arm-linux-gnueabi-gcc --static -c led.c -o led.o
buzzer.o: buzzer.h buzzer.c
	arm-linux-gnueabi-gcc --static -c buzzer.c -o buzzer.o
fnd.o: fnd.h fnd.c
	arm-linux-gnueabi-gcc --static -c fnd.c -o fnd.o
textlcd.o: textlcd.h textlcd.c
	arm-linux-gnueabi-gcc --static -c textlcd.c -o textlcd.o
colorled.o: colorled.h colorled.c
	arm-linux-gnueabi-gcc --static -c colorled.c -o colorled.o
temperature.o: temperature.h temperature.c
	arm-linux-gnueabi-gcc --static -c temperature.c -o temperature.o
accelMagGyro.o: accelMagGyro.h accelMagGyro.c
	arm-linux-gnueabi-gcc --static -c accelMagGyro.c -o accelMagGyro.o
clean: 
	rm -f *.o *.a
