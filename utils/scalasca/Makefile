#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES: .c .cpp

CXXFLAGS += $(SZLIB_CFLAGS)

EPK_UTIL_DIR = $(TOPDIR)/epik/utils
SCAN_INCS = -I$(EPK_UTIL_DIR)
SCAN_LIBS = -L$(EPK_UTIL_DIR) -lepk.util $(LIBERTY) $(UTILLIB)


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: skin scan square scalasca

install: all
	$(MKDIR) -m 755 $(BINDIR)
	$(INSTALL) -c -m 755 skin $(BINDIR)
	$(INSTALL) -c -m 755 scan $(BINDIR)
	$(INSTALL) -c -m 755 square $(BINDIR)
	$(INSTALL) -c -m 755 scarlet $(BINDIR)
	$(INSTALL) -c -m 755 scalasca $(BINDIR)

clean:
	rm -f skin scan square scalasca

skin: skin.in
	sed -e "s!@PDTARCH@!$(PDTARCH)!" skin.in > skin

scan: scan.cpp
	$(ECXX) $(CXXFLAGS) -DBINDIR=\"$(BINDIR)\" $(LDFLAGS) \
		$(SCAN_INCS) scan.cpp -o $@ $(SCAN_LIBS)

square: square.in
	sed -e "s!@DOCDIR@!$(DOCDIR)/html!" square.in > square

scalasca: scalasca.in
	sed -e "s!@CONFIG@!$(CFGKEY): $(CONFIG)!" \
	    -e "s!@DOCDIR@!$(DOCDIR)!" scalasca.in > scalasca


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------
