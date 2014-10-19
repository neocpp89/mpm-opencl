TEXSRC = \
    structured-grid-numbering.tex
PDF = $(TEXSRC:.tex=.pdf)

.PHONY: clean
.PHONY: all

all: $(PDF)

%.pdf:%.tex
	latexmk -pdf $^; latexmk -c $^;

clean:
	latexmk -C $(TEXSRC);
