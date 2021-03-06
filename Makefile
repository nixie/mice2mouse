##
## --------------------------------------------------------
## Autor:          Paulik Miroslav
##                 xpauli00@stud.fit.vutbr.cz
## Vytvoreno dne:  9. 12. 2011 
## --------------------------------------------------------
## Pouziti:        [make]
##                  - preklad vsech soucasti projektu
##                 [make run]
##                  - spusteni aplikace
##                 [make pdf]
##                  - pouze vygenerovani dokumentace
##                 [make clean]
##                  - smazani generovanych souboru 
##                 [make pack[-zip|-tar|-targz]]
##                  - zabaleni do archivu zip, tar nebo tar.gz
##                 [make help]
##                  - vypsani pouziti Makefile
##

# ---------------------------------------------------------
# INIT
# ---------------------------------------------------------

# jmeno projektu
NAME=xpauli00

# parametry prekladu
CC=g++
LDFLAGS=-lglut -lGLU -lSDL -lpthread -lz
CFLAGS=-Wall -pedantic -W -g  -Wno-unused
#CFLAGS=-Wall -pedantic -W -g -DNDEBUG
SRC=*.cpp *.h Makefile

GL2PS=gl2ps-1.3.6
LIBGL2PS=$(GL2PS)/gl2ps.o

# parametry spusteni

# parametry pro latex
DOCNAME=$(NAME)
DVIPSFLAGS=-t a4
BIBTEX=disabled#[enabled|disabled]
VLNA=disabled#[enabled|disabled]

# parametry mazani
LATEXFILES=.ps .dvi .log .aux .bbl .toc .blg .pdf
CLEANBIN=enabled#[enabled|disabled]
CLEANBACKUP=enabled#[enabled|disabled]
CLEANLATEX=enabled#[enabled|disabled]

# parametry pro archivovani
DEFAULTPACK=zip
PACKNAME=xpauli00
ALLFILES=$(NAME).tex $(NAME).bib Makefile czechiso.bst

# ---------------------------------------------------------
# NEMENIT !!!
# ---------------------------------------------------------

CLEAN=

ifeq ($(CLEANBIN), enabled)
CLEAN+=itu_demo joytest m2m_device DemoDrawing *.o DemoCursor DemoHunt DemoTTT $(GL2PS)/gl2ps.o
endif
ifeq ($(CLEANBACKUP), enabled)
CLEAN+=*~
endif
ifeq ($(CLEANLATEX), enabled)
CLEAN+=$(foreach var,$(LATEXFILES),$(DOCNAME)$(var))
endif

# ---------------------------------------------------------
# MAKE
# ---------------------------------------------------------

$(NAME): m2m_device joytest itu_demo DemoDrawing DemoHunt DemoTTT DemoCursor

# GLUT version
itu_demo: main.cpp mice2mouse.o hunt_log.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

joytest: joytest.c
	$(CC) $(CFLGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

$(GL2PS)/gl2ps.o:	$(GL2PS)/gl2ps.c $(GL2PS)/gl2ps.h
	gcc -c -DHAVE_ZLIB $(GL2PS)/gl2ps.c -o $@

# SDL version
BaseApp.o: BaseApp.cpp BaseApp.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` BaseApp.cpp -o $@

BaseApp: CEvent.o BaseApp.cpp BaseApp.h $(LIBGL2PS)
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

DemoDrawing: CEvent.o BaseApp.o DemoDrawing.cpp DemoDrawing.h $(LIBGL2PS)
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

DemoCursor: BaseApp.o CEvent.o DemoCursor.cpp $(LIBGL2PS)
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

DemoTTT: BaseApp.o CEvent.o DemoTTT.cpp DemoTTT.h $(LIBGL2PS)
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

DemoHunt: BaseApp.o CEvent.o hunt_log.o DemoHunt.cpp DemoHunt.h $(LIBGL2PS)
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --cflags` $^ -o $@

CEvent.o:	CEvent.cpp CEvent.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` $< -o $@

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

# ---------------------------------------------------------
# PROCESSING OF LOGS
# ---------------------------------------------------------

log:
	./delta_correct.py

stats: log
	gnuplot aiming_time_progress.plt
	

# ---------------------------------------------------------
# RUN
# ---------------------------------------------------------

run: $(NAME)
	sudo ./DemoCursor
	sudo ./DemoDrawing
	sudo ./DemoTTT
	sudo ./DemoHunt
	echo "Dikec!"

# ---------------------------------------------------------
# CLEAN
# ---------------------------------------------------------

clean-latex:
	# rm -f $(foreach var,$(LATEXFILES),$(DOCNAME)$(var))
	make -C doc clean

clean:
	rm -f $(foreach var,$(CLEAN),$(var))
	make -C doc clean

# ---------------------------------------------------------
# LATEX
# ---------------------------------------------------------

vlna:
ifeq ($(VLNA), enabled)
	vlna $(DOCNAME).tex
endif

ifeq ($(BIBTEX), enabled)
bibtex:
	latex $(DOCNAME)
	bibtex $(DOCNAME)
else
bibtex:
endif

latex-double:
	latex $(DOCNAME)
	latex $(DOCNAME)

divps:
	dvips $(DVIPSFLAGS) $(DOCNAME).dvi
ps2pdf:
	ps2pdf $(DOCNAME).ps

doc:
	make -C doc;

pdf: doc

# ---------------------------------------------------------
# PACK
# ---------------------------------------------------------

ifeq ($(DEFAULTPACK), zip)
pack: pack-zip
else ifeq ($(DEFAULTPACK), tar)
pack: pack-tar
else ifeq ($(DEFAULTPACK), tar.gz)
pack: pack-targz
endif

pack-zip:
	rm -f $(PACKNAME).zip
	zip $(PACKNAME).zip $(ALLFILES)
pack-tar:
	rm -f $(PACKNAME).tar
	tar cvf $(PACKNAME).tar $(ALLFILES)
pack-targz:
	rm -f $(PACKNAME).tar.gz
	tar cvzf $(PACKNAME).tar.gz $(ALLFILES)

# ---------------------------------------------------------
# HELP
# ---------------------------------------------------------

help:
	head -n `grep -c "^##" Makefile` Makefile

.PHONY: doc clean
