CC=g++
LDFLAGS=-lglut -lGLU
CFLAGS=-Wall -pedantic -W -g
#CFLAGS=-Wall -pedantic -W -g -DNDEBUG
SRC=*.cpp *.h Makefile

all: itu_demo

# GLUT version
itu_demo: main.cpp mice2mouse.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@


# SDL version
CApp: CEvent.o CApp.cpp CApp.h
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@
CEvent.o:	CEvent.cpp CEvent.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` $< -o $@


clean:
	rm -f itu_demo joytest uinput CApp
	rm -f *.o

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: doc clean
