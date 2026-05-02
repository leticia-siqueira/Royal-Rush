CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = main

SRC = src/main.c src/objetos.c

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

