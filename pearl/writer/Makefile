#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend


# Directories
CFG_DIR = $(TOPDIR)/epik/config
CFG_INC = -I$(CFG_DIR)

EPK_DIR = $(TOPDIR)/epik/utils
EPK_INC = -I$(EPK_DIR)

ESD_DIR = $(TOPDIR)/epik/episode
ESD_INC = -I$(ESD_DIR)

ELG_DIR = $(TOPDIR)/epik/epilog/base
ELG_INC = -I$(ELG_DIR)

PB_DIR  = $(TOPDIR)/pearl/base
PB_INC  = -I$(PB_DIR)

PM_DIR  = $(TOPDIR)/pearl/MPI
PM_INC  = -I$(PM_DIR)

PR_DIR  = $(TOPDIR)/pearl/replay
PR_INC  = -I$(PR_DIR)


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(MPIEXTRA) \
            $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) \
            $(PB_INC) $(PM_INC) $(PR_INC)


# Object files
OBJS = \
	EpkWriter.o

MPI_OBJS = $(OBJS:.o=.mpi.o)
HYB_OBJS = $(OBJS:.o=.hyb.o)


# Installed header files
USER_HDRS = \
	EpkWriter.h


# Libraries
ifneq ($(MPICXX),)
  LIBRARIES += libpearl.writer.mpi.a
endif
ifneq ($(HYBCXX),)
  LIBRARIES += libpearl.writer.hyb.a
endif


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: $(LIBRARIES)

install: all
ifneq ($(LIBRARIES),)
	$(MKDIR) -m 755 $(LIBDIR)
	@if [ -f libpearl.writer.mpi.a ]; then \
		echo "$(INSTALL) -c -m 644 libpearl.writer.mpi.a $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 libpearl.writer.mpi.a $(LIBDIR)" ; \
	 fi
	@if [ -f libpearl.writer.hyb.a ]; then \
		echo "$(INSTALL) -c -m 644 libpearl.writer.hyb.a $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 libpearl.writer.hyb.a $(LIBDIR)" ; \
	 fi
	$(MKDIR) -m 755 $(INCDIR)/pearl
	@list='$(USER_HDRS)' ; \
	 for hdr in $$list; do \
		echo "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
		eval "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
	 done
endif

clean:
	rm -f *.o lib*.a

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
	       $(PB_INC) $(PM_INC) $(PR_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,\([a-zA-Z_]*\)\.o:\(.*\),\1.mpi.o \1.hyb.o:\2,' \
	       -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	       -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	       -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	       -e 's,$(ELG_DIR),$$(ELG_DIR),g' \
	       -e 's,$(PB_DIR),$$(PB_DIR),g' \
	       -e 's,$(PM_DIR),$$(PM_DIR),g' \
	       -e 's,$(PR_DIR),$$(PR_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.mpi.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

%.hyb.o: %.cpp
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) -c $< -o $@

libpearl.writer.mpi.a: $(MPI_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(MPI_OBJS)
	@echo "*** $@ built"

libpearl.writer.hyb.a: $(HYB_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(HYB_OBJS)
	@echo "*** $@ built"


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
