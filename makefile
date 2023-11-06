CC = gcc
CFLAGS = -Wall
SRC = main.c 1.c 2.c 3.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = final.elf

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXECUTABLE)
