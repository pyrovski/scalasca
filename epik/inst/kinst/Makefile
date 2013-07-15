#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# Ugly hack for Open64, which uses 
ifeq ($(KINST_EXTRA),kinst-open64)
COMP = open64
endif

# KINST_EXTRA is an optional extra (compiler) target defined in Makefile.defs

all: kconfig kinst-pomp $(KINST_EXTRA)

install: install-kconfig install-kinst-pomp install-$(KINST_EXTRA)

kinst-pomp: kinst-pomp.in
	sed -e "s!@BINDIR@!$(BINDIR)!" \
		-e "s!@INCDIR@!$(INCDIR)!g" \
		-e "s!@LIBDIR@!$(LIBDIR)!" \
		-e "s!@DOCDIR@!$(DOCDIR)!" \
		-e "s!@OMPCC@!$(OMPCC)!" \
		-e "s!@OPARI2DIR@!$(OPARI2DIR)!" \
		-e "s!@OMPFLAG@!`echo $(OMPFLAG) | cut -d ' ' -f 1`!" \
		-e "s!@PMPILIB@!$(PMPILIB)!" \
		-e "s!@FMPILIB@!$(FMPILIB)!" \
		-e "s!@UTILLIB@!$(UTILLIB) $(SZLIB_LIB)!" \
		-e "s!@OTFLIB@!$(OTFLIB)!" \
		-e "s!@BFDLIB@!$(BFDLIB)!" \
		-e "s!@METRLIB@!$(METRLIB)!" \
		-e "s!@SIONLIB@!$(SIONLIB_LIB)!" \
		-e "s!@PDTBIN@!$(PDTBIN)!" \
		-e "s!@PDTMPIINC@!$(PDTMPIINC)!" \
		-e "s!@FDOPT@!$(FDOPT)!" \
		-e "s!@PREC@!$(PREC)!" \
		-e "s!@FPP@!$(FPP)!" \
		kinst-pomp.in > kinst-pomp
	if test -f kinst-pomp-$(PLAT).sed ; then \
		sed -f kinst-pomp-$(PLAT).sed kinst-pomp > ktmp ; \
		mv ktmp kinst-pomp ; \
	fi
	if test -f kinst-pomp-$(COMP).sed ; then \
		sed -f kinst-pomp-$(COMP).sed kinst-pomp > ktmp ; \
		mv ktmp kinst-pomp ; \
	fi

install-kinst-pomp: kinst-pomp
	$(INSTALL) -c -m 755 kinst-pomp $(BINDIR)
	$(INSTALL) -c -m 644 scalasca.inst $(DOCDIR)

$(KINST_EXTRA): $(KINST_EXTRA).in
	sed -e "s!@BINDIR@!$(BINDIR)!" \
		-e "s!@INCDIR@!$(INCDIR)!g" \
		-e "s!@LIBDIR@!$(LIBDIR)!" \
		-e "s!@DOCDIR@!$(DOCDIR)!" \
		-e "s!@OMPCC@!$(OMPCC)!" \
		-e "s!@OPARI2DIR@!$(OPARI2DIR)!" \
		-e "s!@PMPILIB@!$(PMPILIB)!" \
		-e "s!@FMPILIB@!$(FMPILIB)!" \
		-e "s!@UTILLIB@!$(UTILLIB) $(SZLIB_LIB)!" \
		-e "s!@OTFLIB@!$(OTFLIB)!" \
		-e "s!@BFDLIB@!$(BFDLIB)!" \
		-e "s!@METRLIB@!$(METRLIB)!" \
		-e "s!@SIONLIB@!$(SIONLIB_LIB)!" \
		-e "s!@PDTBIN@!$(PDTBIN)!" \
		-e "s!@PDTMPIINC@!$(PDTMPIINC)!" \
		-e "s!@FDOPT@!$(FDOPT)!" \
		-e "s!@PREC@!$(PREC)!" \
		-e "s!@FPP@!$(FPP)!" \
		-e "s!@CINSTFLAG@!$(CINSTFLAG)!" \
		$(KINST_EXTRA).in > $(KINST_EXTRA)
	if test -f kinst-pomp-$(PLAT).sed ; then \
		sed -f kinst-pomp-$(PLAT).sed $(KINST_EXTRA) > ktmp ; \
		mv ktmp $(KINST_EXTRA) ; \
	fi
	if test -f kinst-pomp-$(COMP).sed ; then \
		sed -f kinst-pomp-$(COMP).sed $(KINST_EXTRA) > ktmp ; \
		mv ktmp $(KINST_EXTRA) ; \
	fi

install-: # dummy target

ifneq ($(KINST_EXTRA),)
install-$(KINST_EXTRA): $(KINST_EXTRA)
	$(INSTALL) -c -m 755 $(KINST_EXTRA) $(BINDIR)/kinst
	@if [ $(KINST_EXTRA) = "kinst-intel" ]; then \
		$(INSTALL) -c -m 644 tcollect10.ini $(BINDIR) ; \
	fi
endif

kconfig: kconfig.in
	sed -e "s!@BINDIR@!$(BINDIR)!" \
		-e "s!@INCDIR@!$(INCDIR)!" \
		-e "s!@LIBDIR@!$(LIBDIR)!" \
		-e "s!@PMPILIB@!$(PMPILIB)!" \
		-e "s!@FMPILIB@!$(FMPILIB)!" \
		-e "s!@UTILLIB@!$(UTILLIB) $(SZLIB_LIB)!" \
		-e "s!@OTFLIB@!$(OTFLIB)!" \
		-e "s!@BFDLIB@!$(BFDLIB)!" \
		-e "s!@METRLIB@!$(METRLIB)!" \
		-e "s!@SIONLIB@!$(SIONLIB_LIB)!" \
		-e "s!@FDOPT@!$(FDOPT)!" \
		-e "s!@CC@!$(CC)!" \
		-e "s!@PREFIX@!$(PREFIX)!" \
		-e "s!@OPARI2DIR@!$(OPARI2DIR)!" \
		-e "s!@PREC@!$(PREC)!" \
		kconfig.in > kconfig
install-kconfig: kconfig
	$(INSTALL) -c -m 755 kconfig $(BINDIR)

clean:
	rm -f kconfig kinst-pomp $(KINST_EXTRA)

