#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR     = ..
DOC_OUTPUT = $(TOPDIR)/doc/pearl

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean doc


# Submodules
# Assumption: If HYBCXX is set, OMPCXX and MPICXX are set as well
MODULES = base
ifneq ($(OMPCXX),)
  MODULES += OMP
endif
ifneq ($(MPICXX),)
  MODULES += MPI
endif
ifneq ($(HYBCXX),)
  MODULES += hybrid
endif
ifneq ($(MPICXX)$(HYBCXX),)
  MODULES += writer
endif
MODULES += replay
MODULES += example


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo
	@echo "************************************************************"
	@echo "*                      Building PEARL                      *"
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
	@echo "*                     Installing PEARL                     *"
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
	@echo "*                      Cleaning PEARL                      *"
	@echo "************************************************************"
	@echo
	@list='$(MODULES)' ; \
	 for dir in $$list; do \
		if test -d "$$dir"; then \
			echo "---------- $$dir ----------" ; \
			echo ; \
			( cd $$dir && $(MAKE) clean ) || exit ; \
			echo ;\
		fi \
	 done

doc:
	@echo
	@echo "************************************************************"
	@echo "*              Generating PEARL documentation              *"
	@echo "************************************************************"
	@echo
	-mkdir -p $(DOC_OUTPUT)/../tags
	-mkdir -p $(DOC_OUTPUT)/pdf
	-mkdir -p $(DOC_OUTPUT)/html
	-cp doc/logo.pdf $(DOC_OUTPUT)/pdf/
	-cp doc/logo_small.png $(DOC_OUTPUT)/html/
	OUTPUT="$(DOC_OUTPUT)" doxygen doc/doxygen.conf
