#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ..
DOC_OUTPUT = $(TOPDIR)/doc/scout

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean doc


# Submodules
MODULES = src doc


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo
	@echo "************************************************************"
	@echo "*                      Building SCOUT                      *"
	@echo "************************************************************"
	@echo
	@list='$(MODULES)' ; \
	 for dir in $$list; do \
		if test -d "$$dir"; then \
			echo "---------- $$dir ----------" ; \
			echo ; \
			( cd $$dir && $(MAKE) all ) || exit ; \
			echo ; \
		fi \
	 done

install:
	@echo
	@echo "************************************************************"
	@echo "*                     Installing SCOUT                     *"
	@echo "************************************************************"
	@echo
	@list='$(MODULES)' ; \
	 for dir in $$list; do \
		if test -d "$$dir"; then \
			echo "---------- $$dir ----------" ; \
			echo ; \
			( cd $$dir && $(MAKE) install ) || exit ; \
			echo ; \
		fi \
	 done

clean:
	@echo
	@echo "************************************************************"
	@echo "*                      Cleaning SCOUT                      *"
	@echo "************************************************************"
	@echo
	@list='$(MODULES)' ; \
	 for dir in $$list; do \
		if test -d "$$dir"; then \
			echo "---------- $$dir ----------" ; \
			echo ; \
			( cd $$dir && $(MAKE) clean ) || exit ; \
			echo ; \
		fi \
	 done

doc:
	@echo
	@echo "************************************************************"
	@echo "*              Generating SCOUT documentation              *"
	@echo "************************************************************"
	@echo
	-mkdir -p $(DOC_OUTPUT)/../tags
	-mkdir -p $(DOC_OUTPUT)/pdf
	-mkdir -p $(DOC_OUTPUT)/html
	-cp doc/scalasca.pdf $(DOC_OUTPUT)/pdf/
	-cp doc/scalasca.png $(DOC_OUTPUT)/html/
	OUTPUT="$(DOC_OUTPUT)" doxygen doc/doxygen.conf
