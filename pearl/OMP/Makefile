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


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(PB_INC) 


# Object files
OBJS = \
	OmpALock_rep.o \
	OmpCollExit_rep.o \
	OmpEventFactory.o \
	OmpFork_rep.o \
	OmpJoin_rep.o \
	OmpLock_rep.o \
	OmpRLock_rep.o \
	OmpSmallObjAllocator.o \
	pearl_omp.o


# Installed header files
USER_HDRS =


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libpearl.omp.a

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 libpearl.omp.a $(LIBDIR)
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
	@$(OMPCXX) $(OMPFLAG) -DPEARL_USE_LIST -MM \
	       $(PB_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(PB_DIR),$$(PB_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(OMPCXX) $(OMPFLAG) $(CXXFLAGS) -c $<

libpearl.omp.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)
	@echo "*** $@ built"


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
