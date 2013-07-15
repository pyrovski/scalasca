#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

TOOLS = elg_merge \
        elg_msgord \
        elg_print \
        elg_stat \
        elg_filter \
	elg_timecorrect \
        elg_paraver \
        esd2cube \
        $(ELG_VTF3) \
        $(ELG_OTF)

ifneq ($(MPICC),)
TOOLS += elg_unify.mpi
endif

OTFTOOLS = elg2otf vtotf2elg

CONV_OBJS = elg_conv.o elg_conv_msgqueue.o

CUBEW_DIR    = ../../../cube-3.0/src/cubew
CUBEW_LIB    = cubew3
CUBEW_ARC    = $(CUBEW_DIR)/lib$(CUBEW_LIB).a
CUBEW_LIBS  = -L$(CUBEW_DIR) -l$(CUBEW_LIB)

EPK_CFG_DIR = ../../config

EPK_UTIL_DIR = ../../utils
EPK_UTIL_LIB = epk.util
EPK_UTIL_ARC = $(EPK_UTIL_DIR)/lib$(EPK_UTIL_LIB).a

ESD_DIR      = ../../episode
ESD_LIB      = esd.ser
ESD_ARC      = $(ESD_DIR)/lib$(ESD_LIB).a

ELG_BASE_DIR = ../base
ELG_BASE_LIB = elg.base
ELG_BASE_ARC = $(ELG_BASE_DIR)/lib$(ELG_BASE_LIB).a

EPK_AD_MPI_DIR = ../../adapter/MPI

BASE_LIBS = -L$(ELG_BASE_DIR) -l$(ELG_BASE_LIB) \
            -L$(EPK_UTIL_DIR) -l$(EPK_UTIL_LIB) \
            $(UTILLIB) $(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIBSER)
ESD_LIBS  = -L$(ESD_DIR) -l$(ESD_LIB)
CFLAGS   += -I$(EPK_CFG_DIR) -I$(EPK_UTIL_DIR) -I$(ESD_DIR) -I$(ELG_BASE_DIR)
CXXFLAGS += -I$(EPK_CFG_DIR) -I$(EPK_UTIL_DIR) -I$(ESD_DIR) -I$(ELG_BASE_DIR) -I$(EPK_AD_MPI_DIR)

all: $(CONV_OBJS) $(TOOLS)
	@if test -n "$(SLOG2DIR)"; then \
		( cd elg2slog2; $(MAKE) all ); \
	fi

elg_conv.o : elg_conv.c elg_conv.h
	$(CC) $(CFLAGS) -c elg_conv.c
elg_conv_msgqueue.o : elg_conv_msgqueue.c elg_conv_msgqueue.h elg_conv.h
	$(CC) $(CFLAGS) -c elg_conv_msgqueue.c

elg_merge : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) elg_merge.cpp
	$(CXX) $(CXXFLAGS) -c elg_merge.cpp
	$(CXX) $(LDFLAGS) $(SFLAG) -o $@ elg_merge.o $(BASE_LIBS)
elg_msgord : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) elg_msgord.cpp
	$(CXX) $(CXXFLAGS) -c elg_msgord.cpp
	$(CXX) $(LDFLAGS) $(SFLAG) -o $@ elg_msgord.o $(BASE_LIBS)
elg_print : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) elg_print.c elg_print.c.gen
	$(CC) $(CFLAGS) -c elg_print.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg_print.o $(BASE_LIBS)
elg_stat : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(CONV_OBJS) elg_stat.c
	$(CC) $(CFLAGS) -c elg_stat.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg_stat.o $(CONV_OBJS) $(BASE_LIBS)
elg_vtf3 : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(CONV_OBJS) elg_vtf3.c
	$(CC) $(CFLAGS) -I$(VTF3DIR)/include -c elg_vtf3.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg_vtf3.o $(CONV_OBJS) $(VTF3LIB) $(BASE_LIBS)
elg2otf : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(CONV_OBJS) elg2otf.c vtotf2elg
	$(CC) $(CFLAGS) -I$(OTFDIR)/include -c elg2otf.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg2otf.o $(CONV_OBJS) $(OTFLIB) $(BASE_LIBS)
vtotf_handler.o: vtotf_handler.c vtotf_handler.h elg_dvector.h
	$(CC) $(CFLAGS) -I$(OTFDIR)/include -c vtotf_handler.c
vtotf2elg: $(EPK_UTIL_ARC) $(ELG_BASE_ARC) vtotf2elg.c vtotf_handler.c \
	vtotf_handler.h elg_dvector.h
	$(CC) $(CFLAGS) -I$(OTFDIR)/include -DWRITE_TO_FILE \
	-c vtotf_handler.c -o vtotf_handler_rw.o
	$(CC) $(CFLAGS) -I$(OTFDIR)/include -c vtotf2elg.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ vtotf2elg.o vtotf_handler_rw.o \
	$(OTFLIB) $(BASE_LIBS)
elg_paraver : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(CONV_OBJS) elg_paraver.c paraver_mpitab.h
	$(CC) $(CFLAGS) -c elg_paraver.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg_paraver.o $(CONV_OBJS) $(BASE_LIBS)
elg_filter : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) elg_filter.cpp
	$(CXX) $(CXXFLAGS) -c elg_filter.cpp
	$(CXX) $(LDFLAGS) $(SFLAG) -o $@ elg_filter.o $(BASE_LIBS)
elg_timecorrect : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) elg_timecorrect.c
	$(CC) $(CFLAGS) -c elg_timecorrect.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ elg_timecorrect.o $(BASE_LIBS) -lm
elg_unify.mpi : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(ESD_ARC) elg_unify_mpi.c
	$(MPICC) $(CFLAGS) -DELG_MPI -c elg_unify_mpi.c
	$(MPICC) $(LDFLAGS) $(SFLAG) -o $@ elg_unify_mpi.o \
		$(ESD_LIBS) $(BASE_LIBS) $(MPILIB) $(PMPILIB) $(UTILLIB)

esd2cube : $(EPK_UTIL_ARC) $(ELG_BASE_ARC) $(ESD_ARC) $(CUBEW_ARC) esd2cube.c
	$(CC) $(CFLAGS) -c esd2cube.c
	$(CC) $(LDFLAGS) $(SFLAG) -o $@ esd2cube.o $(ESD_LIBS) $(CUBEW_LIBS) $(BASE_LIBS)

install: $(TOOLS) $(INST_VTF3) $(INST_OTF)
	$(MKDIR) -m 755 $(BINDIR)
	@list='$(TOOLS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 755 $$p $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 $$p $(BINDIR)" ; \
	done
	$(INSTALL) -c -m 755 elg2prv $(BINDIR)
	@if test -n "$(SLOG2DIR)"; then \
		( cd elg2slog2; $(MAKE) install ); \
	fi

install-vtf3:
	$(INSTALL) -c -m 755 elg2vtf $(BINDIR)
	$(INSTALL) -c -m 755 vtf3sort $(BINDIR)/vptmerge

install-otf:
	$(INSTALL) -c -m 755 elg2otf $(BINDIR)
	$(INSTALL) -c -m 755 vtotf2elg $(BINDIR)

clean:
	rm -f *.o $(TOOLS) $(OTFTOOLS)
	@if test -n "$(SLOG2DIR)"; then \
		( cd elg2slog2; $(MAKE) clean ); \
	fi
