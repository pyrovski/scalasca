#---------------------------------------------------------------------------
# EARL Tools - Makefile
#---------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs


.SUFFIXES: .cpp .o

EPKDIR  = $(TOPDIR)/epik/utils
EPKINC  = -I$(EPKDIR)
EPKLIB  = -L$(EPKDIR) -lepk.util
ESDDIR  = $(TOPDIR)/epik/episode
ESDINC  = -I$(ESDDIR) -I$(ESDDIR)/platform
#ESDLIB  = -L$(ESDDIR) -lesd
ELGDIR  = $(TOPDIR)/epik/epilog
ELGBASEINC = -I$(ELGDIR)/base
ELGBASELIB = -L$(ELGDIR)/base -lelg.base
ELGRTSINC  = -I$(ELGDIR)/rts
ELGRTSLIB  = -L$(ELGDIR)/rts -lelg

EARLDIR = ..
EARLINC = -I$(EARLDIR)
EARLLIB = -L$(EARLDIR) -learl

UTILLIB += $(SZLIB_LIBPATH) $(SZLIB_LIB)

MYCXXFLAGS = $(EPKINC) $(ESDINC) $(ELGBASEINC) $(EARLINC) 
MYLDFLAGS  = $(EARLLIB) $(ELGBASELIB) $(EPKLIB) $(ECXXLIBS)

#- files
OBJS =  ApproxTimesMgr.o \
	Compensator.o \
	Overheads.o \
	elg_pcomp.o

all: elg_pcomp elg_ovrhds

elg_pcomp: $(OBJS)
	$(ECXX) -o elg_pcomp $(OBJS) $(MYLDFLAGS) $(SFLAG) $(UTILLIB)

elg_ovrhds: elg_ovrhds.cpp
	$(CXX) $(CXXFLAGS) $(EPKINC) $(ESDINC) $(ELGRTSINC) elg_ovrhds.cpp -o elg_ovrhds \
	$(LDFLAGS) $(EPKLIB) $(ELGRTSLIB) $(METRLIB) $(SFLAG) $(UTILLIB)

.cpp.o: 
	$(ECXX) $(MYCXXFLAGS) -c $<

install:
	$(INSTALL) -c -m 755 elg_pcomp $(BINDIR)
	$(INSTALL) -c -m 755 elg_ovrhds $(BINDIR)

clean:
	rm -f *~ *.o  '#'* core 
	rm -f elg_pcomp
	rm -f elg_ovrhds



