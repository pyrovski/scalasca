TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES: .cc .o

OBJS = opari2.o process_c.o process_f.o process_omp.o \
	ompragma.o ompragma_c.o ompragma_f.o ompregion.o handler.o

all : opari2 opari2-config

opari2: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o opari2 $(OBJS)

opari2-config: opari2_config.h opari2_config.cc opari2_config_tool_frontend.h
	$(CXX) $(CXXFLAGS) -I. $(LDFLAGS) -o opari2-config opari2_config.cc

opari2_config_tool_frontend.h: opari2_config_tool_frontend.h.tmpl
	sed -e "s!@BINDIR@!$(BINDIR)!" \
	    -e "s!@INCDIR@!$(INCDIR)!" \
	opari2_config_tool_frontend.h.tmpl > opari2_config_tool_frontend.h

.cc.o:
	$(CXX) $(CXXFLAGS) -I. -c $*.cc

install: all
	@if test ! -e /usr/bin/gawk; then \
	  sed -e 's@^#!.*$$@#!'`which awk`' -f@' pomp2-parse-init-regions.awk > /tmp/sed.out && mv /tmp/sed.out pomp2-parse-init-regions.awk; \
	fi
	$(INSTALL) -c -m 755 opari2 $(BINDIR)
	$(INSTALL) -c -m 755 opari2-config $(BINDIR)
	$(INSTALL) -c -m 755 pomp2-parse-init-regions.awk $(BINDIR)
	$(MKDIR) -m 755 $(INCDIR)/opari2
	$(INSTALL) -c -m 644 ../../include/opari2/pomp2_lib.h $(INCDIR)/opari2

clean:
	rm -rf core a.out *.o opari2 opari2-config out
	rm -rf Templates.DB tempinc ti_files ii_files rii_files *.ti *.ir
	rm -rf SunWS_cache cxx_repository
	rm -rf opari2_config_tool_frontend.h
