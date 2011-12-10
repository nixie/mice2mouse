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
LDFLAGS=-lglut -lGLU
CFLAGS=-Wall -pedantic -W -g
#CFLAGS=-Wall -pedantic -W -g -DNDEBUG
SRC=*.cpp *.h Makefile

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
CLEAN+=itu_demo joytest m2m_device DrawingDemo *.o
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

$(NAME): itu_demo DrawingDemo m2m_device joytest

# GLUT version
itu_demo: main.cpp mice2mouse.o hunt_log.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

joytest: joytest.c
	$(CC) $(CFLGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@

# SDL version
DrawingDemo: CEvent.o DrawingDemo.cpp DrawingDemo.h
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@

BaseApp.o: BaseApp.cpp BaseApp.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` BaseApp.cpp -o $@

BaseApp_main: BaseApp.o CEvent.o BaseApp_main.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@

DA: DA.cpp DA.h BaseApp.o CEvent.o 
	$(CC) $(CFLAGS) $(LDFLAGS) `sdl-config --libs --cflags` $^ -o $@

CEvent.o:	CEvent.cpp CEvent.h
	$(CC) -c $(CFLAGS) `sdl-config --cflags` $< -o $@

%.o: %.cpp %.h
	$(CC) -c $(CFLAGS) $< -o $@

# ---------------------------------------------------------
# RUN
# ---------------------------------------------------------

run:

# ---------------------------------------------------------
# CLEAN
# ---------------------------------------------------------

clean-latex:
	rm -f $(foreach var,$(LATEXFILES),$(DOCNAME)$(var))

clean:
	rm -f $(foreach var,$(CLEAN),$(var))

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

pdf: clean-latex vlna bibtex latex-double divps ps2pdf

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
