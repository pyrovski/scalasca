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
PM_LIB  = -L$(PM_DIR) -lpearl.mpi
PM_DEP  = $(PM_DIR)/libpearl.mpi.a

PH_DIR  = $(TOPDIR)/pearl/hybrid
PH_LIB  = -L$(PH_DIR) -lpearl.hybrid
PH_DEP  = $(PH_DIR)/libpearl.hybrid.a

PR_DIR  = $(TOPDIR)/pearl/replay
PR_INC  = -I$(PR_DIR)
PR_LIB  = -L$(PR_DIR) -lpearl.replay
PR_DEP  = $(PR_DIR)/libpearl.replay.a

COMMONDEP = $(EPK_DEP) $(ELG_DEP) $(PB_DEP) $(PR_DEP)


# Compile flags
MPIEXTRA += -D_MPI
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(CFG_INC) $(ESD_INC) $(ELG_INC) \
            $(PB_INC) $(PR_INC)


# Programs
ifneq ($(OMPCXX),)
  PROGRAMS += pearl_print.omp
endif
ifneq ($(MPICXX),)
  PROGRAMS += pearl_print.mpi
endif
ifneq ($(HYBCXX),)
  PROGRAMS += pearl_print.hyb
endif


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(PROGRAMS)

install: all

clean:
	rm -f *.o $(PROGRAMS)

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
	@$(OMPCXX) $(OMPFLAG) -DPEARL_USE_LIST -MM \
	       $(PB_INC) $(PR_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,\([a-zA-Z_]*\)\.o:\(.*\),\1.omp.o \1.mpi.o \1.hyb.o:\2,' \
	       -e 's,$(PB_DIR),$$(PB_DIR),g' \
	       -e 's,$(PR_DIR),$$(PR_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.omp.o: %.cpp
	$(OMPCXX) $(OMPFLAG) $(CXXFLAGS) -c $< -o $@

%.mpi.o: %.cpp
	$(MPICXX) $(CXXFLAGS) $(MPIEXTRA) -c $< -o $@

%.hyb.o: %.cpp
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) $(MPIEXTRA) -c $< -o $@

pearl_print.omp: pearl_print.omp.o $(COMMONDEP) $(PO_DEP)
	$(OMPCXX) $(OMPFLAG) $(CXXFLAGS) $(LDFLAGS) $(SFLAG) -o $@ $< \
		$(PR_LIB) $(PO_LIB) $(PB_LIB) $(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
                $(UTILLIB)

pearl_print.mpi: pearl_print.mpi.o $(COMMONDEP) $(PM_DEP)
	$(MPICXX) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		$(PR_LIB) $(PM_LIB) $(PB_LIB) $(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
                $(MPILIB) $(UTILLIB)

pearl_print.hyb: pearl_print.hyb.o $(COMMONDEP) $(PO_DEP) $(PM_DEP) $(PH_DEP)
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) $(MPIEXTRA) $(LDFLAGS) $(SFLAG) -o $@ $< \
		$(PR_LIB) $(PH_LIB) $(PM_LIB) $(PO_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
                $(MPILIB) $(UTILLIB)


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
