#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean


# Submodules
MODULES = src    


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo
	@echo "************************************************************"
	@echo "*                       Building CLC                       *"
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
	@echo "*                      Installing CLC                      *"
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
	@echo "*                       Cleaning CLC                       *"
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
