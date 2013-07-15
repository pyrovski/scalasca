#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES: .c .cpp

.cpp.o:
	$(ECXX) $(CXXFLAGS) -c $< 	

OBJS = P2Statistic.o

#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libstats.a

install: all
	$(MKDIR) -m 755 $(LIBDIR) $(INCDIR)/stats
	$(INSTALL) -c -m 644 P2Statistic.h $(INCDIR)/stats
	$(INSTALL) -c -m 644 libstats.a $(LIBDIR)

libstats.a: $(OBJS)
	$(ECXX_AR) $(AFLAG) $(ECXX_ARFLAGS) $@ $(OBJS)

clean:
	rm -f *~ *.o core libstats.a
	rm -rf SunWS_cache cxx_repository
	rm -rf Templates.DB tempinc ti_files ii_files rii_files *.ti *.ir

P2Statistic.o: P2Statistic.cpp P2Statistic.h


