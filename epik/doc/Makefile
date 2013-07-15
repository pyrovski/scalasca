#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

all:

clean:

install:
	$(MKDIR) -m 755 $(DOCDIR)/manuals
	$(INSTALL) -c -m 644 $(TOPDIR)/example/METRICS.SPEC $(DOCDIR)
	-test -f ../epilog/doc/epilog.pdf && $(INSTALL) -c -m 644 ../epilog/doc/epilog.pdf $(DOCDIR)/manuals
