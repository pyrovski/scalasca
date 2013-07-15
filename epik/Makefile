#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR     = ..
DOC_OUTPUT = $(TOPDIR)/doc/epik

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean doc


# Submodules
MODULES = config inst/kinst doc utils \
	  adapter/POMP2 adapter/MPI adapter/compiler \
	  episode epilog/base epilog/rts epilog/tools \
	  epi-otf/rts epi-otf/tools 



#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo ""
	@echo "************************************************************"
	@echo "*                      Building EPIK                       *"
	@echo "************************************************************"
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ; \
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) all ) || exit ; \
		fi \
	done

install:
	@echo ""
	@echo "************************************************************"
	@echo "*                     Installing EPIK                      *"
	@echo "************************************************************"
	$(MKDIR) -m 755 $(BINDIR) $(LIBDIR) $(INCDIR) $(DOCDIR)
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ; \
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) install ) || exit ; \
		fi \
	done

clean:
	@echo ""
	@echo "************************************************************"
	@echo "*                      Cleaning EPIK                       *"
	@echo "************************************************************"
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ; \
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) clean ) || exit ; \
		fi \
	done

doc:
	@echo ""
	@echo "************************************************************"
	@echo "*              Generating EPIK documentation               *"
	@echo "************************************************************"
	@echo ""
	-mkdir -p $(DOC_OUTPUT)/pdf
	-mkdir -p $(DOC_OUTPUT)/html
	-cp doc/logo.pdf $(DOC_OUTPUT)/pdf/
	-cp doc/logo_small.png $(DOC_OUTPUT)/html/
	OUTPUT="$(DOC_OUTPUT)" doxygen doc/doxygen.conf
