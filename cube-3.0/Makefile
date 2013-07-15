#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = .
DOC_OUTPUT = $(TOPDIR)/doc

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean distclean doc


# Submodules
MODULES = \
	utils/stats \
	src/cube \
	src/cubew \
        src/tools \
	$(CUBE_TARGETS)


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:
	@echo
	@echo "************************************************************"
	@echo "*                      Building CUBE3                      *"
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
	@echo
	@echo "************************************************************"
	@echo "*                     Installing CUBE3                     *"
	@echo "************************************************************"
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ; \
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) install ) || exit ; \
		fi \
	done
	@if test -f $(PREFIX)/bin/cube3-qt; then \
		ln -f -s $(PREFIX)/bin/cube3-qt $(PREFIX)/bin/cube3 ; \
	elif test -f $(PREFIX)/bin/cube3-wx; then \
		ln -f -s $(PREFIX)/bin/cube3-wx $(PREFIX)/bin/cube3 ; \
	fi
	$(MKDIR) -m 755 $(DOCDIR)/manuals
	$(INSTALL) -c -m 644 doc/CubeInstall.pdf $(DOCDIR)/manuals
	if test -f doc/example.cube.gz; then \
		$(INSTALL) -c -m 644 doc/example.cube.gz $(DOCDIR) ; \
	fi

clean:
	@echo
	@echo "************************************************************"
	@echo "*                      Cleaning CUBE3                      *"
	@echo "************************************************************"
	@for dir in $(MODULES)""; do \
		if test -d "$$dir"; then \
			echo "" ;\
			echo "---------- $$dir ----------" ; \
			echo "" ; \
			( cd $$dir && $(MAKE) clean ) || exit ; \
		fi \
	done
	@if test -f doc/Makefile; then ( cd doc; $(MAKE) clean; ) fi

distclean: clean
	rm -f configure.log Makefile.defs qt.pro.defs

doc:
	@echo
	@echo "************************************************************"
	@echo "*              Generating CUBE documentation               *"
	@echo "************************************************************"
	@echo
	-mkdir -p $(DOC_OUTPUT)/tags
	-mkdir -p $(DOC_OUTPUT)/cubew/html
	-cp doc/logo_small.png $(DOC_OUTPUT)/cubew/html/
	OUTPUT="$(DOC_OUTPUT)/cubew" CUBEVER="$(CUBEVER)" doxygen doc/doxygen-cubew.conf
	-mkdir -p $(DOC_OUTPUT)/cube/html
	-cp doc/logo_small.png $(DOC_OUTPUT)/cube/html/
	OUTPUT="$(DOC_OUTPUT)/cube" CUBEVER="$(CUBEVER)" doxygen doc/doxygen-cube.conf
	-mkdir -p $(DOC_OUTPUT)/cube-guiqt/html
	-cp doc/logo_small.png $(DOC_OUTPUT)/cube-guiqt/html/
	OUTPUT="$(DOC_OUTPUT)/cube-guiqt" CUBEVER="$(CUBEVER)" doxygen doc/doxygen-guiqt.conf

#	OUTPUT="$(DOC_OUTPUT)" doxygen tools.doxygen
#	OUTPUT="$(DOC_OUTPUT)" doxygen GUI-wx.doxygen
