.PHONY: all clean

all: docs/meeting0.pdf docs/meeting1.pdf

docs/meeting0.pdf: docs/meeting0.tex
	cd docs && pdflatex meeting0.tex && rm meeting0.aux meeting0.log meeting0.toc

docs/meeting1.pdf: docs/meeting1.tex
	cd docs && pdflatex meeting1.tex && rm meeting1.aux meeting1.log meeting1.toc

clean:
	rm -f docs/*.pdf docs/*.aux docs/*.log docs/*.toc
