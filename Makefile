CC=g++
LDFLAGS=-lglut -lGLU -lSDL
CFLAGS=-Wall -pedantic -W -g
CFLAGS=-Wall -pedantic -W -g -DNDEBUG
SRC=*.cpp *.h Makefile

all: itu_demo uinput

itu_demo: main.cpp mice2mouse.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@


clean:
	rm -f itu_demo joytest uinput
	rm -f *.o

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: doc
