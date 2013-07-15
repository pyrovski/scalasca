#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean


# Submodules
MODULES = $(SZLIB) scalasca stats


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo ""
	@echo "************************************************************"
	@echo "*                      Building UTILS                      *"
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
	@echo "*                     Installing UTILS                     *"
	@echo "************************************************************"
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
	@echo "*                      Cleaning UTILS                      *"
	@echo "************************************************************"
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ; \
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) clean ) || exit ; \
		fi \
	done

