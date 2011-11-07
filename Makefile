CC=g++
LDFLAGS=-lglut -lGLU

all: main

main: main.cpp mice2mouse.o
	$(CC) $(LDFLAGS) $^
