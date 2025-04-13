.PHONY: all clean

all: docs/meeting0.pdf

docs/meeting0.pdf: docs/meeting0.tex
	cd docs && pdflatex meeting0.tex && rm meeting0.aux meeting0.log meeting0.toc

clean:
	rm -f docs/*.pdf
