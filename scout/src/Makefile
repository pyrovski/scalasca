#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend


# Directories & libraries
CFG_DIR  = $(TOPDIR)/epik/config
CFG_INC  = -I$(CFG_DIR)

EPK_DIR  = $(TOPDIR)/epik/utils
EPK_INC  = -I$(EPK_DIR)
EPK_LIB  = -L$(EPK_DIR) -lepk.util
EPK_DEP  = $(EPK_DIR)/libepk.util.a

ELG_DIR  = $(TOPDIR)/epik/epilog/base
ELG_LIB  = -L$(ELG_DIR) -lelg.base
ELG_DEP  = $(ELG_DIR)/libelg.base.a

PB_DIR   = $(TOPDIR)/pearl/base
PB_INC   = -I$(PB_DIR)
PB_LIB   = -L$(PB_DIR) -lpearl.base
PB_DEP   = $(PB_DIR)/libpearl.base.a

PO_DIR   = $(TOPDIR)/pearl/OMP
PO_LIB   = -L$(PO_DIR) -lpearl.omp
PO_DEP   = $(PO_DIR)/libpearl.omp.a

PM_DIR   = $(TOPDIR)/pearl/MPI
PM_INC   = -I$(PM_DIR)
PM_LIB   = -L$(PM_DIR) -lpearl.mpi
PM_DEP   = $(PM_DIR)/libpearl.mpi.a

PH_DIR   = $(TOPDIR)/pearl/hybrid
PH_LIB   = -L$(PH_DIR) -lpearl.hybrid
PH_DEP   = $(PH_DIR)/libpearl.hybrid.a

PR_DIR   = $(TOPDIR)/pearl/replay
PR_INC   = -I$(PR_DIR)
PR_LIB   = -L$(PR_DIR) -lpearl.replay
PR_DEP   = $(PR_DIR)/libpearl.replay.a

CLC_DIR  = $(TOPDIR)/clc/src
CLC_INC  = -I$(CLC_DIR)
CLC_LIB  = -L$(CLC_DIR) -lclc
CLC_DEP  = $(CLC_DIR)/libclc

CUBE_DIR = $(TOPDIR)/cube-3.0/src/cubew
CUBE_INC = -I$(CUBE_DIR)
CUBE_LIB = -L$(CUBE_DIR) -lcubew3
CUBE_DEP = $(CUBE_DIR)/libcubew3.a


#--- Common settings -------------------------------------------------------

COM_FLAGS = \
	$(CXXFLAGS) -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
	$(CFG_INC) $(EPK_INC) $(PB_INC) $(PR_INC) $(CUBE_INC)
COM_DEPS = \
	$(PB_DEP) $(PR_DEP) $(ELG_DEP) $(EPK_DEP) $(CUBE_DEP)


#--- Serial specific -------------------------------------------------------

SER_FLAGS = \
	$(COM_FLAGS)
SER_OBJS = \
	AnalyzeTask.ser.o \
	CbData.ser.o \
	CheckedTask.ser.o \
	Logging.ser.o \
	Pattern.ser.o \
	Patterns_gen.ser.o \
	PatternDetectionTask.ser.o \
	PreprocessTask.ser.o \
	ReadDefinitionsTask.ser.o \
	ReadTraceTask.ser.o \
	ReportWriterTask.ser.o \
	StatisticsTask.ser.o \
        TaskData.ser.o \
	TimedPhase.ser.o \
	Timer.ser.o \
	scout.ser.o
SER_LIBS = \
	$(PR_LIB) $(PB_LIB) \
	$(ELG_LIB) $(EPK_LIB) $(CUBE_LIB) \
	$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB)
SER_DEPS = \
	$(COM_DEPS)

PROGRAMS += scout.ser


#--- OpenMP specific -------------------------------------------------------

OMP_FLAGS = \
	$(OMPFLAG) $(COM_FLAGS) $(SIONLIB_CFLAGS_OMP)
OMP_OBJS = \
	AnalyzeTask.ser.o \
	CbData.ser.o \
	CheckedTask.omp.o \
	Logging.omp.o \
	OmpPattern.ser.o \
	Pattern.omp.o \
	Patterns_gen.omp.o \
	PatternDetectionTask.ser.o \
	PreprocessTask.omp.o \
	Quantile.ser.o \
	ReadDefinitionsTask.ser.o \
	ReadTraceTask.omp.o \
	ReportWriterTask.omp.o \
	StatisticsTask.omp.o \
        TaskData.ser.o \
	TimedPhase.ser.o \
	Timer.omp.o \
	scout.omp.o
OMP_LIBS = \
	$(PR_LIB) $(PO_LIB) $(PB_LIB) \
	$(ELG_LIB) $(EPK_LIB) $(CUBE_LIB) \
	$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB)
OMP_DEPS = \
	$(COM_DEPS) $(PO_DEP)

ifneq ($(OMPCXX),)
  PROGRAMS += scout.omp
endif


#--- MPI specific -----------------------------------------------------------

MPI_FLAGS = \
	-D_MPI $(MPIEXTRA) $(COM_FLAGS) $(CLC_INC) $(PM_INC) $(SIONLIB_CFLAGS_MPI)
MPI_OBJS = \
	AnalyzeTask.ser.o \
	CbData.mpi.o \
	CheckedTask.mpi.o \
	Logging.ser.o \
	MpiDatatypes.mpi.o \
	MpiOperators.mpi.o \
	MpiPattern.mpi.o \
	Pattern.mpi.o \
	Patterns_gen.mpi.o \
	PatternDetectionTask.ser.o \
	PreprocessTask.mpi.o \
	Quantile.ser.o \
	ReadDefinitionsTask.mpi.o \
	ReadTraceTask.mpi.o \
	ReportWriterTask.mpi.o \
	StatisticsTask.mpi.o \
	SynchronizeTask.mpi.o \
        TaskData.ser.o \
	TimedPhase.ser.o \
	Timer.mpi.o \
	scout.mpi.o
MPI_LIBS = \
	$(CLC_LIB).mpi \
	$(PR_LIB) $(PM_LIB) $(PB_LIB) \
	$(ELG_LIB) $(EPK_LIB) $(CUBE_LIB) \
	$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
	$(MPILIB) $(UTILLIB)
MPI_DEPS = \
	$(COM_DEPS) $(PM_DEP) $(CLC_DEP).mpi.a

ifneq ($(MPICXX),)
  PROGRAMS += scout.mpi
endif


#--- Hybrid MPI/OpenMP specific ---------------------------------------------

HYB_FLAGS = \
	$(HYBFLAG) -D_MPI $(MPIEXTRA) $(COM_FLAGS) $(CLC_INC) $(PM_INC) \
	$(SIONLIB_CFLAGS_OMPI)
HYB_OBJS = \
	AnalyzeTask.ser.o \
	CbData.mpi.o \
	CheckedTask.hyb.o \
	Logging.omp.o \
	MpiDatatypes.mpi.o \
	MpiOperators.mpi.o \
	MpiPattern.hyb.o \
	OmpPattern.ser.o \
	Pattern.hyb.o \
	Patterns_gen.hyb.o \
	PatternDetectionTask.ser.o \
	PreprocessTask.hyb.o \
	Quantile.ser.o \
	ReadDefinitionsTask.mpi.o \
	ReadTraceTask.hyb.o \
	ReportWriterTask.hyb.o \
	StatisticsTask.hyb.o \
	SynchronizeTask.mpi.o \
        TaskData.ser.o \
	TimedPhase.ser.o \
	Timer.omp.o \
	scout.hyb.o
HYB_LIBS = \
	$(CLC_LIB).hyb \
	$(PR_LIB) $(PH_LIB) $(PM_LIB) $(PO_LIB) $(PB_LIB) \
	$(ELG_LIB) $(EPK_LIB) $(CUBE_LIB) \
	$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
	$(MPILIB) $(UTILLIB)
HYB_DEPS = \
	$(COM_DEPS) $(PO_DEP) $(PM_DEP) $(PH_DEP) $(CLC_DEP).hyb.a

ifneq ($(HYBCXX),)
  PROGRAMS += scout.hyb
endif


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(PROGRAMS)

install: all
	$(MKDIR) -m 755 $(BINDIR)
	$(INSTALL) -c -m 755 scout.ser $(BINDIR)
	@if [ -f scout.omp ]; then \
		echo "$(INSTALL) -c -m 755 scout.omp $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 scout.omp $(BINDIR)" ; \
	 fi
	@if [ -f scout.mpi ]; then \
		echo "$(INSTALL) -c -m 755 scout.mpi $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 scout.mpi $(BINDIR)" ; \
	 fi
	@if [ -f scout.hyb ]; then \
		echo "$(INSTALL) -c -m 755 scout.hyb $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 scout.hyb $(BINDIR)" ; \
	 fi

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
	@$(CXX) $(SER_FLAGS) -MM \
		$(patsubst %.ser.o, %.cpp, $(sort $(filter %.ser.o, $(SER_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(HYB_OBJS)))) \
	 | sed -e 's|.o:|.ser.o:|' > depend.tmp
	@$(OMPCXX) $(OMP_FLAGS) -MM \
		$(patsubst %.omp.o, %.cpp, $(sort $(filter %.omp.o, $(SER_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(HYB_OBJS)))) \
	 | sed -e 's|.o:|.omp.o:|' >> depend.tmp
	@$(MPICXX) $(MPI_FLAGS) -MM -isystem $$MPICH2_ROOT/include \
		$(patsubst %.mpi.o, %.cpp, $(sort $(filter %.mpi.o, $(SER_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(HYB_OBJS)))) \
	 | sed -e 's|.o:|.mpi.o:|' >> depend.tmp
	@$(HYBCXX) $(HYB_FLAGS) -MM -isystem $$MPICH2_ROOT/include \
		$(patsubst %.hyb.o, %.cpp, $(sort $(filter %.hyb.o, $(SER_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(HYB_OBJS)))) \
	 | sed -e 's|.o:|.hyb.o:|' >> depend.tmp
	@cat depend.tmp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	      -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	      -e 's,$(PB_DIR),$$(PB_DIR),g' \
	      -e 's,$(PM_DIR),$$(PM_DIR),g' \
	      -e 's,$(PR_DIR),$$(PR_DIR),g' \
	      -e 's,$(CLC_DIR),$$(CLC_DIR),g' \
	      -e 's,$(CUBE_DIR),$$(CUBE_DIR),g' \
	      -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
	@rm -f depend.tmp
endif


%.ser.o: %.cpp
	$(CXX) $(SER_FLAGS) -c $< -o $@

%.omp.o: %.cpp
	$(OMPCXX) $(OMP_FLAGS) -c $< -o $@

%.mpi.o: %.cpp
	$(MPICXX) $(MPI_FLAGS) -c $< -o $@

%.hyb.o: %.cpp
	$(HYBCXX) $(HYB_FLAGS) -c $< -o $@

scout.ser: $(SER_OBJS) $(SER_DEPS)
	$(CXX) $(SER_FLAGS) $(LDFLAGS) $(SFLAG) -o $@ $(SER_OBJS) $(SER_LIBS)

scout.omp: $(OMP_OBJS) $(OMP_DEPS)
	$(OMPCXX) $(OMP_FLAGS) $(LDFLAGS) $(SFLAG) -o $@ $(OMP_OBJS) $(OMP_LIBS)

scout.mpi: $(MPI_OBJS) $(MPI_DEPS)
	$(MPICXX) $(MPI_FLAGS) $(LDFLAGS) $(SFLAG) -o $@ $(MPI_OBJS) $(MPI_LIBS)

scout.hyb: $(HYB_OBJS) $(HYB_DEPS)
	$(HYBCXX) $(HYB_FLAGS) $(LDFLAGS) $(SFLAG) -o $@ $(HYB_OBJS) $(HYB_LIBS)


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
