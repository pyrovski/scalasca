#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: QtMakefile
	$(MAKE) -f QtMakefile

install: install$(QT_PLAT) install-doc

clean:
	@if test -f QtMakefile; then ( $(MAKE) -f QtMakefile distclean ); fi
	-rm -f QtMakefile cube3-qt.pro


QtMakefile: cube3-qt.pro $(TOPDIR)/Makefile.defs $(TOPDIR)/qt.pro.defs
	$(QT_QMAKE) $(QT_PLAT) -spec $(QT_MKSPEC) -o QtMakefile cube3-qt.pro

cube3-qt.pro: cube3-qt.pro.in $(TOPDIR)/Makefile.defs
	cat cube3-qt.pro.in > cube3-qt.pro
ifeq ($(ESH_AR),)
	echo "LIBS += -lcube3" >> cube3-qt.pro
else
	echo "LIBS += $(ESH_RPATH) -lcube3so" >> cube3-qt.pro
endif
	echo "LIBS += $(CCXXLIBS) $(SZLIB_LIBPATH) $(SZLIB_LIB)" >> cube3-qt.pro
	echo "QMAKE_LFLAGS_SHAPP += $(LDFLAGS)" >> cube3-qt.pro
	echo "QMAKE_CXXFLAGS += $(SZLIB_CFLAGS)" >> cube3-qt.pro
	sed -e 's,@TOPDIR@,$(TOPDIR),' cube3-qt.pro >sed.out \
	    && mv sed.out cube3-qt.pro || exit 1


install-unix: all
	$(MKDIR) -m 755 $(BINDIR)
	$(INSTALL) -c -m 755 cube3-qt $(BINDIR)

install-macx: all
	$(MKDIR) -m 755 $(BINDIR)/cube3-qt.app/Contents/MacOS
	$(INSTALL) -c -m 755 cube3-qt.app/Contents/MacOS/cube3-qt \
		$(BINDIR)/cube3-qt.app/Contents/MacOS
	$(INSTALL) -c -m 644 cube3-qt.app/Contents/Info.plist \
		cube3-qt.app/Contents/PkgInfo \
		$(BINDIR)/cube3-qt.app/Contents/MacOS
	ln -f -s $(BINDIR)/cube3-qt.app/Contents/MacOS/cube3-qt $(BINDIR)/cube3-qt

install-doc:
	$(MKDIR) -m 755 $(DOCDIR)/manuals
	$(INSTALL) -c -m 644 ../../doc/CubeGuide.pdf $(DOCDIR)/manuals
