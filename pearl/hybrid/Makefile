#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend


# Directories
PB_DIR = $(TOPDIR)/pearl/base
PB_INC = -I$(PB_DIR)

PO_DIR = $(TOPDIR)/pearl/OMP
PO_INC = -I$(PO_DIR)

PM_DIR = $(TOPDIR)/pearl/MPI
PM_INC = -I$(PM_DIR)


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(MPIEXTRA) \
            $(PB_INC) $(PO_INC) $(PM_INC)


# Object files
OBJS = \
	HybridEventFactory.o \
	pearl_hybrid.o


# Installed header files
USER_HDRS =


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libpearl.hybrid.a

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 libpearl.hybrid.a $(LIBDIR)
	$(MKDIR) -m 755 $(INCDIR)/pearl
	@list='$(USER_HDRS)' ; \
	 for hdr in $$list; do \
		echo "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
		eval "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
	done

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
	@$(HYBCXX) $(HYBFLAG) -DPEARL_USE_LIST -MM \
	       -isystem $$MPICH2_ROOT/include \
	       $(PB_INC) $(PO_INC) $(PM_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(PB_DIR),$$(PB_DIR),g' \
	       -e 's,$(PO_DIR),$$(PO_DIR),g' \
	       -e 's,$(PM_DIR),$$(PM_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(HYBCXX) $(HYBFLAG) $(CXXFLAGS) -c $<

libpearl.hybrid.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)
	@echo "*** $@ built"


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
