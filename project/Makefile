CC=arm-linux-gnueabi-gcc --static
AR=arm-linux-gnueabi-ar

all: ronaldo.out
	scp *.out *.bmp ecube@172.20.10.4:/home/ecube
	
ronaldo.out: ronaldo.c libMyPeri.a
	$(CC) ronaldo.c -o ronaldo.out -lMyPeri -L. -lpthread -lm

clean: 
	rm -f *.o *.a *.out
	rm -f $(TARGET)
