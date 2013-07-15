#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

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

PB_DIR = $(TOPDIR)/pearl/base
PB_INC = -I$(PB_DIR)
PB_LIB = -L$(PB_DIR) -lpearl.base
PB_DEP = $(PB_DIR)/libpearl.base.a

PO_DIR = $(TOPDIR)/pearl/OMP
PO_LIB = -L$(PO_DIR) -lpearl.omp
PO_DEP = $(PO_DIR)/libpearl.omp.a

PM_DIR = $(TOPDIR)/pearl/MPI
PM_LIB = -L$(PM_DIR) -lpearl.mpi
PM_DEP = $(PM_DIR)/libpearl.mpi.a

PH_DIR = $(TOPDIR)/pearl/hybrid
PH_LIB = -L$(PH_DIR) -lpearl.hybrid
PH_DEP = $(PH_DIR)/libpearl.hybrid.a

PR_DIR = $(TOPDIR)/pearl/replay
PR_INC = -I$(PR_DIR)
PR_LIB = -L$(PR_DIR) -lpearl.replay
PR_DEP = $(PR_DIR)/libpearl.replay.a

PW_DIR = $(TOPDIR)/pearl/writer
PW_INC = -I$(PW_DIR)
PW_LIB = -L$(PW_DIR) -lpearl.writer
PW_DEP = $(PW_DIR)/libpearl.writer

COMMONDEP = $(EPK_DEP) $(ELG_DEP) $(PB_DEP) $(PR_DEP)


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(MPI_EXTRA) \
            $(CFG_INC) $(ESD_INC) $(ELG_INC) \
            $(PB_INC) $(PR_INC) $(PW_INC)


# Programs
ifneq ($(MPICXX),)
  PROGS += pearl_write.mpi
endif
ifneq ($(HYBCXX),)
  PROGS += pearl_write.hyb
endif


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(PROGS)

install: all

clean:
	rm -f *.o $(PROGS)

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
               $(CFG_INC) $(ESD_INC) $(ELG_INC) \
	       $(PB_INC) $(PR_INC) $(PW_INC) *.cpp \
	 | sed -e 's,\([a-zA-Z_]*\)\.o:\(.*\),\1.mpi.o \1.hyb.o: \\\n \2,' \
	       -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	       -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	       -e 's,$(ELG_DIR),$$(ELG_DIR),g' \
	       -e 's,$(PB_DIR),$$(PB_DIR),g' \
	       -e 's,$(PR_DIR),$$(PR_DIR),g' \
	       -e 's,$(PW_DIR),$$(PW_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.mpi.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

%.hyb.o: %.cpp
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) -c $< -o $@

pearl_write.mpi: pearl_write.mpi.o $(COMMONDEP) $(PM_DEP) $(PW_DEP).mpi.a
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) $(SFLAG) -o $@ $< \
		$(PW_LIB).mpi $(PR_LIB) $(PM_LIB) $(PB_LIB) \
                $(ELG_LIB) $(EPK_LIB) \
                $(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
                $(MPILIB) $(UTILLIB)

pearl_write.hyb: pearl_write.hyb.o $(COMMONDEP) $(PO_DEP) $(PM_DEP) $(PH_DEP) \
		$(PW_DEP).hyb.a
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) $(LDFLAGS) $(SFLAG) -o $@ $< \
		$(PW_LIB).hyb $(PR_LIB) $(PH_LIB) $(PM_LIB) $(PO_LIB) $(PB_LIB) \
		$(ELG_LIB) $(EPK_LIB) \
		$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
                $(MPILIB) $(UTILLIB)


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
