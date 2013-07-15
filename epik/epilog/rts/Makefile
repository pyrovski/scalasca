#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# optional SZLIB_CFLAGS to enable compression of tracefiles

#dependencies
# elg_thrd.h elg_pform.h
# elg_mpireg.h elg_omplock.h epk_conf.h elg_metric.h

.SUFFIXES:  .cpp .c .f

EPK_UTIL_DIR = ../../utils
EPK_CFG_DIR  = ../../config
EPK_AD_MPI_DIR = ../../adapter/MPI
EPK_AD_OMP_DIR = ../../adapter/POMP2
EPK_AD_USR_DIR = ../../adapter/POMP2
EPK_AD_COMP_DIR = ../../adapter/compiler
ESD_DIR = ../../episode
PLATDIR = $(ESD_DIR)/platform
ELG_BASE_DIR = ../base

CFLAGS += -I. -I$(EPK_CFG_DIR) -I$(EPK_UTIL_DIR) -I$(ESD_DIR) -I$(PLATDIR) \
        -I$(EPK_AD_MPI_DIR) -I$(EPK_AD_OMP_DIR) -I$(EPK_AD_COMP_DIR)

ELG_RTS_LIBS = libelg.ser.a $(MPIELGLIB) $(OMPELGLIB) $(HYBELGLIB)

SER_RTS_LIBS =  $(EPK_UTIL_DIR)/libepk.util.a \
                $(EPK_AD_COMP_DIR)/libepk.ad.comp.ser.a \
                $(EPK_AD_USR_DIR)/libepk.ad.usr.a \
                $(ELG_BASE_DIR)/libelg.base.a \
                $(ESD_DIR)/libesd.ser.a
SER_RTS_OBJS =  elg_gen.o elg_mrgthrd.o elg_trc.ser.o
MPI_RTS_LIBS =  $(EPK_UTIL_DIR)/libepk.util.a \
                $(EPK_AD_COMP_DIR)/libepk.ad.comp.mpi.a \
                $(EPK_AD_USR_DIR)/libepk.ad.usr.a \
                $(EPK_AD_MPI_DIR)/libepk.ad.mpi.a \
                $(ELG_BASE_DIR)/libelg.base.a \
                $(ESD_DIR)/libesd.mpi.a
MPI_RTS_OBJS =  elg_gen.o elg_mrgthrd.o elg_trc.mpi.o
OMP_RTS_LIBS =  $(EPK_UTIL_DIR)/libepk.util.a \
                $(EPK_AD_COMP_DIR)/libepk.ad.comp.omp.a \
                $(EPK_AD_OMP_DIR)/libepk.ad.omp.a \
                $(ELG_BASE_DIR)/libelg.base.a \
                $(ESD_DIR)/libesd.omp.a
OMP_RTS_OBJS =  elg_gen.o elg_mrgthrd.o elg_trc.omp.o
HYB_RTS_LIBS =  $(EPK_UTIL_DIR)/libepk.util.a \
                $(EPK_AD_COMP_DIR)/libepk.ad.comp.ompi.a \
                $(EPK_AD_OMP_DIR)/libepk.ad.omp.a \
                $(EPK_AD_MPI_DIR)/libepk.ad.mpi.a \
                $(ELG_BASE_DIR)/libelg.base.a \
                $(ESD_DIR)/libesd.ompi.a
HYB_RTS_OBJS =  elg_gen.o elg_mrgthrd.o elg_trc.ompi.o

USER_HDRS = elg_gen.h elg_trc.h

all: $(ELG_RTS_LIBS)

libelg.ser.a: $(SER_RTS_LIBS) $(SER_RTS_OBJS)
	@for arc in $(SER_RTS_LIBS); do \
                dir=`dirname $$arc` ; \
		for obj in `$(C_AR) $(AFLAG) -t $$arc`; do \
                        $(C_AR) $(AFLAG) -rcs $@ $$dir/$$obj ; \
                done; \
        done
	$(C_AR) $(AFLAG) -rcs $@ $(SER_RTS_OBJS)
	@echo "*** $@ built"

libelg.mpi.a: $(MPI_RTS_LIBS) $(MPI_RTS_OBJS)
	@for arc in $(MPI_RTS_LIBS); do \
                dir=`dirname $$arc` ; \
		for obj in `$(C_AR) $(AFLAG) -t $$arc`; do \
                        $(C_AR) $(AFLAG) -rcs $@ $$dir/$$obj ; \
                done; \
        done
	$(C_AR) $(AFLAG) -rcs $@ $(MPI_RTS_OBJS)
	@echo "*** $@ built"

libelg.omp.a: $(OMP_RTS_LIBS) $(OMP_RTS_OBJS)
	@for arc in $(OMP_RTS_LIBS); do \
                dir=`dirname $$arc` ; \
		for obj in `$(C_AR) $(AFLAG) -t $$arc`; do \
                        $(C_AR) $(AFLAG) -rcs $@ $$dir/$$obj ; \
                done; \
        done
	$(C_AR) $(AFLAG) -rcs $@ $(OMP_RTS_OBJS)
	@echo "*** $@ built"

libelg.ompi.a: $(HYB_RTS_LIBS) $(HYB_RTS_OBJS)
	@for arc in $(HYB_RTS_LIBS); do \
                dir=`dirname $$arc` ; \
		for obj in `$(C_AR) $(AFLAG) -t $$arc`; do \
                        $(C_AR) $(AFLAG) -rcs $@ $$dir/$$obj ; \
                done; \
        done
	$(C_AR) $(AFLAG) -rcs $@ $(HYB_RTS_OBJS)
	@echo "*** $@ built"

elg_gen.o : elg_gen.c elg_gen.h elg_gen.c.gen
	$(CC) $(CFLAGS) $(SZLIB_CFLAGS) $(SIONLIB_CFLAGS_SER) -c elg_gen.c
elg_mrgthrd.o : elg_mrgthrd.c elg_mrgthrd.h
	$(CC) $(CFLAGS) -I$(ELG_BASE_DIR) $(SZLIB_CFLAGS) $(SIONLIB_CFLAGS_SER) -c elg_mrgthrd.c

elg_trc.ser.o : elg_gen.h elg_trc.c.gen $(ESD_DIR)/esd_def.c $(ESD_DIR)/esd_run.c
	$(CC) $(CFLAGS) $(METRCFLAGS) $(COMPCFLAG) \
	-DBINDIR=\"$(BINDIR)\" -c elg_trc.c -o $@
elg_trc.mpi.o : elg_gen.h elg_trc.c.gen $(ESD_DIR)/esd_def.c $(ESD_DIR)/esd_run.c
	$(MPICC) $(CFLAGS) $(METRCFLAGS) $(COMPCFLAG) $(SIONLIB_CFLAGS_MPI) -DELG_MPI \
	-DBINDIR=\"$(BINDIR)\" -c elg_trc.c -o $@
elg_trc.omp.o : elg_gen.h elg_trc.c.gen $(ESD_DIR)/esd_def.c $(ESD_DIR)/esd_run.c
	$(OMPCC) $(CFLAGS) $(METRCFLAGS) $(COMPCFLAG) $(OMPFLAG) $(SIONLIB_CFLAGS_OMP) -DELG_OMP \
	-DBINDIR=\"$(BINDIR)\" -c elg_trc.c -o $@
elg_trc.ompi.o : elg_gen.h elg_trc.c.gen $(ESD_DIR)/esd_def.c $(ESD_DIR)/esd_run.c
	$(HYBCC) $(CFLAGS) $(METRCFLAGS) $(COMPCFLAG) $(OMPFLAG) $(SIONLIB_CFLAGS_OMPI) -DELG_OMPI \
	-DBINDIR=\"$(BINDIR)\" -c elg_trc.c -o $@

elg_trc_c : $(ESD_DIR)/esd_def.c elg_trc.c.gen $(ESD_DIR)/esd_run.c
elg_trc.h : $(ESD_DIR)/esd_def.h elg_trc.h.gen $(ESD_DIR)/esd_run.h
	@echo "/* -- automatically generated - do not change -- */"  > elg_trc.h
	@echo "/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */" >> elg_trc.h
	@echo ""                                                    >> elg_trc.h
	@echo "#ifndef _ELG_TRC_H"                                  >> elg_trc.h
	@echo "#define _ELG_TRC_H"                                  >> elg_trc.h
	@echo ""                                                    >> elg_trc.h
	@echo "#define _OLD_ELG_TRC_INTERFACE_FOR_TAU_"             >> elg_trc.h
	@echo ""                                                    >> elg_trc.h
	cat $(ESD_DIR)/esd_def.h elg_trc.h.gen $(ESD_DIR)/esd_run.h >> elg_trc.h
	@echo ""                                                    >> elg_trc.h
	@echo "#endif"                                              >> elg_trc.h

install: all $(USER_HDRS)
	$(MKDIR) -m 755 $(LIBDIR) $(INCDIR)
	@for p in $(ELG_RTS_LIBS); do \
		echo "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
	done
	@list='$(USER_HDRS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done

clean:
	rm -f *.o elg_trc.h $(ELG_RTS_LIBS)
