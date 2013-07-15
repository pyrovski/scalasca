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
	CallbackData.o \
	CallbackManager.o \
	CompoundTask.o \
	ReplayTask.o \
	Task.o \
	TaskWrapper.o \
	pearl_replay.o


# Installed header files
USER_HDRS = \
	Callback.h \
	CallbackData.h \
	CallbackManager.h \
	pearl_replay.h


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libpearl.replay.a

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 libpearl.replay.a $(LIBDIR)
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
	@$(CXX) -DPEARL_USE_LIST -MM \
	       $(PB_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(PB_DIR),$$(PB_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

libpearl.replay.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)
	@echo "*** $@ built"


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
