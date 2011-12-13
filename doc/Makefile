DOCFILE=documentation
INCLUDES=navrh.tex implementace.tex experimenty.tex

all: $(DOCFILE).pdf

$(DOCFILE).pdf: $(DOCFILE).tex $(INCLUDES)
	pdflatex $(DOCFILE).tex
	#bibtex   $(DOCFILE).aux
	#pdflatex $(DOCFILE).tex
	pdflatex $(DOCFILE).tex

clean:
	rm -f *.aux *.out *.log *.pdf *.toc *.bbl *.blg
