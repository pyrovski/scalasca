#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean


# Figures uses in documentation
FIGURES = \
	figures/cluster.pdf \
	figures/event_types.pdf


#-----------------------------------------------------------------------------
# Rules
#-----------------------------------------------------------------------------

all: doc

install:
	$(MKDIR) -m 755 $(DOCDIR)/manuals
	$(INSTALL) -c -m 644 earl.pdf $(DOCDIR)/manuals

clean:
	-rm -f earl.aux earl.bbl earl.blg
	-rm -f earl.log earl.out earl.toc


doc: earl.pdf

earl.pdf: earl.tex $(FIGURES)
	pdflatex  earl.tex
	bibtex earl
	pdflatex  earl.tex
	pdflatex  earl.tex
