#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend grammar


# Backend data structure
# PEARL_DS = LIST


# Directories & libraries
CFG_DIR  = $(TOPDIR)/epik/config
CFG_INC  = -I$(CFG_DIR)

EPK_DIR  = $(TOPDIR)/epik/utils
EPK_INC  = -I$(EPK_DIR)
EPK_LIB  = -L$(EPK_DIR) -lepk.util
EPK_DEP  = $(EPK_DIR)/libepk.util.a

ELG_DIR  = $(TOPDIR)/epik/epilog/base
ELG_INC  = -I$(ELG_DIR)
ELG_LIB  = -L$(ELG_DIR) -lelg.base
ELG_DEP  = $(ELG_DIR)/libelg.base.a

ESD_DIR  = $(TOPDIR)/epik/episode
ESD_INC  = -I$(ESD_DIR) -I$(ESD_DIR)/platform
ESD_LIB  = -L$(ESD_DIR) -lesd.ser
ESD_DEP  = $(ESD_DIR)/libesd.ser.a

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

PW_DIR   = $(TOPDIR)/pearl/writer
PW_INC   = -I$(PW_DIR)
PW_LIB  = -L$(PW_DIR) -lpearl.writer
PW_DEP  = $(PW_DIR)/libpearl.writer

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
	-I. $(CFG_INC) $(EPK_INC) $(ELG_INC) $(ESD_INC) \
	$(PB_INC) $(PR_INC) $(PW_INC) $(CUBE_INC)
COM_DEPS = \
	$(PB_DEP) $(PR_DEP) $(ELG_DEP) $(EPK_DEP) $(ESD_DEP) $(CUBE_DEP)

YACC     = bison
YFLAGS   = -d -g -v 
LEX      = flex
LFLAGS   = -Cf

#--- Serial specific -------------------------------------------------------

# INFO: Currently no serial simulation is supported. In case you want to
#       to work on this support, use the Makefile in scout/src as a template

SER_FLAGS = \
	$(COM_FLAGS)

#--- OpenMP specific -------------------------------------------------------

# INFO: Currently no OpenMP simulation is supported. In case you want to
#       work on this support, use the Makefile in scout/src as a template

#--- MPI specific -----------------------------------------------------------

MPI_FLAGS = \
	-D_MPI $(MPIEXTRA) $(COM_FLAGS) $(CLC_INC) $(PM_INC) $(SIONLIB_CFLAGS_MPI)
MPI_OBJS = \
	silas.mpi.o \
	Buffer.mpi.o \
	config_parser.mpi.o \
	config_scanner.mpi.o \
	HypBalance.mpi.o \
	HypCutMessage.mpi.o \
	Hypothesis.mpi.o \
	HypScale.mpi.o \
	ModelFactory.mpi.o \
	Model.ser.o \
	ReenactCallbackData.mpi.o \
	ReenactModel.mpi.o \
	silas_replay.mpi.o \
	SilasConfig.mpi.o \
	Simulator.mpi.o \
	Timer.mpi.o

MPI_LIBS = \
	$(CLC_LIB).mpi \
	$(PW_LIB).mpi $(PR_LIB) $(PM_LIB) $(PB_LIB) \
	$(ELG_LIB) $(EPK_LIB) $(ESD_LIB) $(CUBE_LIB) \
	$(SZLIB_LIBPATH) $(SZLIB_LIB) $(SIONLIB_LIB) \
	$(MPILIB) $(UTILLIB)
MPI_DEPS = \
	$(COM_DEPS) $(ESD_DEP) $(PM_DEP) $(CLC_DEP).mpi.a

ifneq ($(MPICXX),)
  PROGRAMS += silas.mpi
endif

#--- Hybrid MPI/OpenMP specific ---------------------------------------------

# INFO: Currently no hybrid MPI/OpenMP simulation is supported. In case you want
#       to work on this support, use the Makefile in scout/src as a template

#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(PROGRAMS)

install: all
	$(MKDIR) -m 755 $(BINDIR)
	@for program in $(PROGRAMS); do \
	if [ -f $$program ]; then \
		echo "$(INSTALL) -c -m 755 $$program $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 $$program $(BINDIR)" ; \
	fi ; done

clean:
	rm -f *.o $(PROGRAMS)

grammar:
	$(YACC) $(YFLAGS) config.y
	mv config.tab.h config_parser.h
	mv config.tab.c config_parser.cpp
	$(LEX) $(LFLAGS) config.l
	sed -e "s/lex\.yy\.c/config_scanner.cpp/" lex.yy.c > config_scanner.cpp
	rm -f lex.yy.c

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
	@$(MPICXX) $(MPI_FLAGS) -MM -isystem $$MPICH2_ROOT/include \
		$(patsubst %.mpi.o, %.cpp, $(sort $(filter %.mpi.o, $(SER_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(HYB_OBJS)))) \
	 | sed -e 's|.o:|.mpi.o:|' >> depend.tmp
	@cat depend.tmp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	      -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	      -e 's,$(ELG_DIR),$$(ELG_DIR),g' \
	      -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	      -e 's,$(PB_DIR),$$(PB_DIR),g' \
	      -e 's,$(PM_DIR),$$(PM_DIR),g' \
	      -e 's,$(PR_DIR),$$(PR_DIR),g' \
	      -e 's,$(PW_DIR),$$(PW_DIR),g' \
	      -e 's,$(CLC_DIR),$$(CLC_DIR),g' \
	      -e 's,$(CUBE_DIR),$$(CUBE_DIR),g' \
	      -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
	@rm -f depend.tmp
endif

%.ser.o: %.cpp
	$(CXX) $(SER_FLAGS) -c $< -o $@

%.mpi.o: %.cpp
	$(MPICXX) $(MPI_FLAGS) -c $< -o $@

silas.mpi: $(MPI_OBJS) $(MPI_DEPS)
	$(MPICXX) $(MPI_FLAGS) $(LDFLAGS) $(SFLAG) -o $@ $(MPI_OBJS) $(MPI_LIBS)

#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
