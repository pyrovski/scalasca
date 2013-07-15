#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

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


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            -I. -I.. $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC)


# Object files
OBJS = \
	Event.o \
	LocalTrace.o


# Installed header files
USER_HDRS = \
	Event.h \
	LocalTrace.h


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: ../libpearl.base.a

install: all
	@list='$(USER_HDRS)' ; \
	 for hdr in $$list; do \
		echo "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
		eval "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
	 done

clean:
	rm -f *.o

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
	@$(CXX) -DPEARL_USE_LIST -MM -I. -I.. \
	       $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	       -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	       -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	       -e 's,$(ELG_DIR),$$(ELG_DIR),g' >>Makefile.dep
endif


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

../libpearl.base.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
