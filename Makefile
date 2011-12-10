CC=g++
LDFLAGS=-lglut -lGLU
CFLAGS=-Wall -pedantic -W -g
#CFLAGS=-Wall -pedantic -W -g -DNDEBUG
SRC=*.cpp *.h Makefile

all: itu_demo DrawingDemo m2m_device joytest

# GLUT version
itu_demo: main.cpp mice2mouse.o hunt_log.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

joytest: joytest.c
	$(CC) $(CFLGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@

# SDL version
DrawingDemo: CEvent.o DrawingDemo.cpp DrawingDemo.h
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@
CEvent.o:	CEvent.cpp CEvent.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` $< -o $@


clean:
	rm -f itu_demo joytest m2m_device DrawingDemo
	rm -f *.o

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: doc clean
