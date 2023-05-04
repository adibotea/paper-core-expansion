master.pdf: *.tex *.bib
	-pdflatex master
	-bibtex master -min-crossrefs=1000
	-pdflatex master
	-pdflatex master
clean:
	rm -f *.aux *.bbl *.blg *.log master.pdf
