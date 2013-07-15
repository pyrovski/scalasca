#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend


# Directories & libraries
CFG_DIR = $(TOPDIR)/epik/config
CFG_INC = -I$(CFG_DIR)

EPK_DIR = $(TOPDIR)/epik/utils
EPK_INC = -I$(EPK_DIR)
EPK_LIB = -L$(EPK_DIR) -lepk.util
EPK_DEP = $(EPK_DIR)/libepk.util.a

ESD_DIR = $(TOPDIR)/epik/episode
ESD_INC = -I$(ESD_DIR)

ELG_DIR = $(TOPDIR)/epik/epilog/base
ELG_INC = -I$(ELG_DIR)
ELG_LIB = -L$(ELG_DIR) -lelg.base
ELG_DEP = $(ELG_DIR)/libelg.base.a

PB_DIR  = $(TOPDIR)/pearl/base
PB_INC  = -I$(PB_DIR)
PB_LIB  = -L$(PB_DIR) -lpearl.base
PB_DEP  = $(PB_DIR)/libpearl.base.a

PO_DIR  = $(TOPDIR)/pearl/OMP
PO_LIB  = -L$(PO_DIR) -lpearl.omp
PO_DEP  = $(PO_DIR)/libpearl.omp.a

PM_DIR  = $(TOPDIR)/pearl/MPI
PM_INC  = -I$(PM_DIR)
PM_LIB  = -L$(PM_DIR) -lpearl.mpi
PM_DEP  = $(PM_DIR)/libpearl.mpi.a

PH_DIR  = $(TOPDIR)/pearl/hybrid
PH_LIB  = -L$(PH_DIR) -lpearl.hybrid
PH_DEP  = $(PH_DIR)/libpearl.hybrid.a

PR_DIR  = $(TOPDIR)/pearl/replay
PR_INC  = -I$(PR_DIR)
PR_LIB  = -L$(PR_DIR) -lpearl.replay
PR_DEP  = $(PR_DIR)/libpearl.replay.a

PW_DIR  = $(TOPDIR)/pearl/writer
PW_INC  = -I$(PW_DIR)
PW_LIB  = -L$(PW_DIR) -lpearl.writer
PW_DEP  = $(PW_DIR)/libpearl.writer

COMMONDEP = $(EPK_DEP) $(ELG_DEP) $(PB_DEP) $(PM_DEP) $(PR_DEP)


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
	     $(MPIEXTRA) \
             -I. $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) \
             $(PB_INC) $(PM_INC) $(PR_INC) $(PW_INC)


# Object files
COMMONOBJS = \
	AmortData.o \
	Clock.o \
	Controller.o \
	Latency.o \
	Network.o \
	Operator.o \
	Predicates.o \
	StatData.o
OBJS = \
	IntervalStat.o \
	RuntimeStat.o \
	Synchronizer.o \
	TimeStat.o \
	Timer.o
MPIOBJS = $(OBJS:.o=.mpi.o)
HYBOBJS = $(OBJS:.o=.hyb.o) OmpData.hyb.o


# Installed header files
USER_HDRS = \
	Synchronizer.h


# Programs & libraries
ifneq ($(MPICXX),)
  PROGRAMS  += clc_synchronize.mpi
#PROGRAMS    += clc_analyze.mpi
  LIBRARIES += libclc.mpi.a
endif
ifneq ($(HYBCXX),)
  PROGRAMS  += clc_synchronize.hyb
#PROGRAMS    += clc_analyze.hyb
  LIBRARIES += libclc.hyb.a
endif


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(LIBRARIES) $(PROGRAMS)

install: all
ifneq ($(LIBRARIES),)
	$(MKDIR) -m 755 $(LIBDIR)
	@if [ -f libclc.mpi.a ]; then \
		echo "$(INSTALL) -c -m 644 libclc.mpi.a $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 libclc.mpi.a $(LIBDIR)" ; \
	 fi
	@if [ -f libclc.hyb.a ]; then \
		echo "$(INSTALL) -c -m 644 libclc.hyb.a $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 libclc.hyb.a $(LIBDIR)" ; \
	 fi
	$(MKDIR) -m 755 $(INCDIR)/clc
	@list='$(USER_HDRS)' ; \
	 for hdr in $$list; do \
		echo "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/clc" ; \
		eval "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/clc" ; \
	 done
endif
ifneq ($(PROGRAMS),)
	$(MKDIR) -m 755 $(BINDIR)
	@if [ -f clc_synchronize.mpi ]; then \
		echo "$(INSTALL) -c -m 755 clc_synchronize.mpi $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 clc_synchronize.mpi $(BINDIR)" ; \
	 fi
	@if [ -f clc_synchronize.hyb ]; then \
		echo "$(INSTALL) -c -m 755 clc_synchronize.hyb $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 clc_synchronize.hyb $(BINDIR)" ; \
	 fi
endif

clean:
	rm -f *.o $(LIBRARIES) $(PROGRAMS)

depend:
ifneq ($(CFGKEY),linux-gomp-mpich2)
	@echo "The 'make depend' rule only works in the 'linux-gomp-mpich2'"
	@echo "configuration with the environment variable MPICH2_ROOT set"
	@echo "to the MPICH2 installation directory."
else
	@echo "Generating/updating dependencies"
	@echo "#------------------------------------------------------------------------------" >Makefile.dep
	@echo "# Dependencies" >>Makefile.dep
	@echo "# (automatically generated using 'make depend')" >>Makefile.dep
	@echo "#------------------------------------------------------------------------------" >>Makefile.dep
	@echo >>Makefile.dep
	@$(MPICXX) -DPEARL_USE_LIST -MM \
	       -isystem $$MPICH2_ROOT/include \
               $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) \
	       $(PB_INC) $(PM_INC) $(PR_INC) $(PW_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,\([a-zA-Z_]*\)\.o:\(.*\),\1.o \1.mpi.o \1.hyb.o:\2,' \
	       -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	       -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	       -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	       -e 's,$(ELG_DIR),$$(ELG_DIR),g' \
	       -e 's,$(PB_DIR),$$(PB_DIR),g' \
	       -e 's,$(PM_DIR),$$(PM_DIR),g' \
	       -e 's,$(PR_DIR),$$(PR_DIR),g' \
	       -e 's,$(PW_DIR),$$(PW_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

%.mpi.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

%.hyb.o: %.cpp
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) -c $< -o $@

libclc.mpi.a: $(COMMONOBJS) $(MPIOBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(COMMONOBJS) $(MPIOBJS)
	@echo "*** $@ built"

libclc.hyb.a: $(COMMONOBJS) $(HYBOBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(COMMONOBJS) $(HYBOBJS)
	@echo "*** $@ built"

clc_synchronize.mpi: clc_synchronize.mpi.o libclc.mpi.a $(COMMONDEP) \
		$(PW_DEP).mpi.a
	$(MPICXX) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		-L. -lclc.mpi \
		$(PW_LIB).mpi $(PR_LIB) $(PM_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
		$(MPILIB) $(UTILLIB)

clc_synchronize.hyb: clc_synchronize.hyb.o libclc.hyb.a $(COMMONDEP) \
		$(PO_DEP) $(PH_DEP) $(PW_DEP).hyb.a
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		-L. -lclc.hyb \
		$(PW_LIB).hyb $(PR_LIB) $(PH_LIB) $(PM_LIB) $(PO_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
		$(MPILIB) $(UTILLIB)

clc_analyze.mpi: clc_analyze.mpi.o libclc.mpi.a $(COMMONDEP)
	$(MPICXX) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		-L. -lclc.mpi \
		$(PR_LIB) $(PM_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
		$(MPILIB) $(UTILLIB)

clc_analyze.hyb: clc_analyze.hyb.o libclc.hyb.a $(COMMONDEP) \
		$(PO_DEP) $(PH_DEP)
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		-L. -lclc.hyb \
		$(PR_LIB) $(PH_LIB) $(PM_LIB) $(PO_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
		$(MPILIB) $(UTILLIB)


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
