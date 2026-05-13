CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

TARGET = main

SRC = src/main.c src/player.c src/tiro.c src/objetos.c src/bruxa.c src/cogumelo.c src/ranking.c

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

