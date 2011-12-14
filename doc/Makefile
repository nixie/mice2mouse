DOCFILE=documentation
INCLUDES=navrh.tex implementace.tex experimenty.tex

all: $(DOCFILE).pdf

$(DOCFILE).pdf: $(DOCFILE).tex $(INCLUDES) imgs
	pdflatex $(DOCFILE).tex
	#bibtex   $(DOCFILE).aux
	#pdflatex $(DOCFILE).tex
	pdflatex $(DOCFILE).tex

imgs: img/basic_idea.eps

img/%.eps:	img/%.dia
	dia --nonew --nosplash -t eps $< -e $@

clean:
	rm -f *.aux *.out *.log *.pdf *.toc *.bbl *.blg
	rm -f img/*-eps-converted-to.pdf
	rm -f img/basic_idea.eps
