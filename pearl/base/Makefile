#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs
include $(TOPDIR)/pearl/Makefile.inc

.PHONY: all install clean backend depend


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
            -I. $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) 


# Object files
OBJS = \
	BlockAllocator.o \
	Buffer.o \
	CNode.o \
	Callsite.o \
	Calltree.o \
	Cartesian.o \
	Comm.o \
	DefsFactory.o \
	EnterCS_rep.o \
	Enter_rep.o \
	Error.o \
	EventFactory.o \
	Event_rep.o \
	Exit_rep.o \
	Flow_rep.o \
	GlobalDefs.o \
	Group.o \
	IdObject.o \
	Location.o \
	Machine.o \
	MappingTable.o \
	MemoryChunk.o \
	Metric.o \
	NamedObject.o \
	Node.o \
	Process.o \
	Region.o \
	RemoteEvent.o \
	RmaGetEnd_rep.o \
	RmaGetStart_rep.o \
	RmaPutEnd_rep.o \
	RmaPutStart_rep.o \
	RmaWindow.o \
	Rma_rep.o \
	SmallObjAllocator.o \
	SmallObject.o \
	Thread.o \
	Topology.o \
	pearl_base.o \
	readcb.o


# Installed header files
USER_HDRS = \
	Buffer.h \
	CNode.h \
	Callsite.h \
	Cartesian.h \
	Comm.h \
	CountedPtr.h \
	Error.h \
	Event_rep.h \
	GlobalDefs.h \
	Group.h \
	IdObject.h \
	Location.h \
	Machine.h \
	Metric.h \
	NamedObject.h \
	Node.h \
	Process.h \
	Region.h \
	RemoteEvent.h \
	RmaWindow.h \
	SmallObject.h \
	Thread.h \
	Topology.h \
	pearl.h \
	pearl_padding.h \
	pearl_types.h


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: backend libpearl.base.a pearl-config

install: all
	$(MKDIR) -m 755 $(BINDIR)
	$(INSTALL) -c -m 755 pearl-config $(BINDIR)
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 libpearl.base.a $(LIBDIR)
	$(MKDIR) -m 755 $(INCDIR)/pearl
	@list='$(USER_HDRS)' ; \
	 for hdr in $$list; do \
		echo "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
		eval "$(INSTALL) -c -m 644 $$hdr $(INCDIR)/pearl" ; \
	 done
	cd $(PEARL_DS); $(MAKE) install

clean:
	rm -f *.o lib*.a pearl-config
	cd $(PEARL_DS); $(MAKE) clean

backend:
	cd $(PEARL_DS); $(MAKE) all

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
	@$(CXX) -DPEARL_USE_LIST -MM -I. \
	       $(CFG_INC) $(EPK_INC) $(ESD_INC) $(ELG_INC) *.cpp \
	 | $(TOPDIR)/utils/build/canonicalize_depends.pl \
	 | sed -e 's,$(CFG_DIR),$$(CFG_DIR),g' \
	       -e 's,$(EPK_DIR),$$(EPK_DIR),g' \
	       -e 's,$(ESD_DIR),$$(ESD_DIR),g' \
	       -e 's,$(ELG_DIR),$$(ELG_DIR),g' \
               -e 's,LIST/,$$(PEARL_DS)/,g' >>Makefile.dep
endif


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

libpearl.base.a: $(OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OBJS)
	@echo "*** $@ built"

pearl-config: pearl-config.in
	sed -e "s|@INCDIR@|$(INCDIR)|" \
            -e "s|@LIBDIR@|$(LIBDIR)|" \
	    -e "s|@PO_LIB@|$(if $(OMPCXX),-lpearl.omp)|" \
	    -e "s|@PM_LIB@|$(if $(MPICXX),-lpearl.mpi)|" \
	    -e "s|@PH_LIB@|$(if $(HYBCXX),-lpearl.hyb)|" \
	    -e "s|@PW_LIB@|$(if $(MPICXX)$(HYBCXX),-lpearl.writer)|" \
	    -e "s|@SZ_LIB@|$(if $(SZLIB),-lsc.z)|" \
	    pearl-config.in > pearl-config


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

-include Makefile.dep
