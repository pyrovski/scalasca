#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean depend


# Directories
EPK_DIR = $(TOPDIR)/epik/utils
EPK_INC = -I$(EPK_DIR)

PB_DIR = $(TOPDIR)/pearl/base
PB_INC = -I$(PB_DIR)


# Compile flags
CXXFLAGS += -DPEARL_USE_$(PEARL_DS) $(PEARL_DS_INC) \
            $(MPIEXTRA) \
            $(EPK_INC) $(PB_INC)


# Object files
OBJS = \
	EventSet.o \
	LocalData.o \
	MpiCancelled_rep.o \
	MpiCartesian.o \
	MpiCollExit_rep.o \
	MpiComm.o \
	MpiDefsFactory.o \
	MpiEventFactory.o \
	MpiGroup.o \
	MpiMessage.o \
	MpiP2P_rep.o \
	MpiReceive_rep.o \
	MpiReceiveComplete_rep.o \
	MpiReceiveRequest_rep.o \
	MpiRequest_rep.o \
	MpiRequestTested_rep.o \
	MpiRmaCollExit_rep.o \
	MpiRmaExit_rep.o \
	MpiRmaGetEnd_rep.o \
	MpiRmaGetStart_rep.o \
	MpiRmaLock_rep.o \
	MpiRmaPutEnd_rep.o \
	MpiRmaPutStart_rep.o \
	MpiRmaUnlock_rep.o \
	MpiSend_rep.o \
	MpiSendRequest_rep.o \
	MpiSendComplete_rep.o \
	MpiWindow.o \
	RemoteData.o \
	RemoteEventSet.o \
	pearl_mpi.o 


# Installed header files
USER_HDRS = \
	EventSet.h \
	MpiCartesian.h \
	MpiComm.h \
	MpiMessage.h \
	MpiWindow.h \
	RemoteEventSet.h


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libpearl.mpi.a

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 libpearl.mpi.a $(LIBDIR)
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
	@$(MPICXX) -DPEARL_USE_LIST -MM \
	       -isystem $$MPICH2_ROOT/include \
	       $(EPK_INC) $(PB_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	       -e 's,$(PB_DIR),$$(PB_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(MPICXX) $(CXXFLAGS) -c $<

libpearl.mpi.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)
	@echo "*** $@ built"


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
