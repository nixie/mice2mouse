DOCFILE=documentation
INCLUDES=navrh.tex implementace.tex experimenty.tex

all: $(DOCFILE).pdf

$(DOCFILE).pdf: $(DOCFILE).tex $(INCLUDES) img/basic_idea.eps
	pdflatex $(DOCFILE).tex
	#bibtex   $(DOCFILE).aux
	#pdflatex $(DOCFILE).tex
	pdflatex $(DOCFILE).tex


img/%.eps:	img/%.dia
	dia --nosplash -t eps $< -e $@

clean:
	rm -f *.aux *.out *.log *.pdf *.toc *.bbl *.blg
	rm -f img/*-eps-converted-to.pdf
	rm -f img/basic_idea.eps
