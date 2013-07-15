#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# mandatory PLAT specification of target platform
# optional METR/METROBJS for additional metrics (e.g., papi)

# epilog/rts/elg_gen.h required by elg_thrd

CUBEDIR = $(TOPDIR)/cube-3.0/src/cubew
CUBEINC = -I$(CUBEDIR)

MPIADIR = ../adapter/MPI 
MPIAINC = -I$(MPIADIR)

CFLAGS += -I. -I../config -I../utils -I../epilog/base -I./platform $(CUBEINC)

.SUFFIXES:  .cpp .c .f

USER_HDRS = elg_defs.h elg_topol.h epik_topol.h elg_impl.h

# EPISODE runtime system modules
PFORM_MOD = elg_pform_${PLAT}
PFORM_TOPOL = epk_pform_topol_${PLAT}

# elg_topol = 3d topology for backwards compatibility
# epik_topol = new version
ESD_BASE_OBJS = $(PFORM_MOD).o \
              esd_gen.o \
              esd_paths.o \
              esd_unify.o \
              esd_report.o \
              elg_topol.o \
              epk_topol.o \
	      $(METROBJS) 

# install Fortran topology headers only if Fortran compiler defined
ifneq ("x$(F77)","x")
USER_HDRS += elg_ftopol.inc epik_ftopol.inc
endif

ESD_SER_OBJS = $(ESD_BASE_OBJS) esd_proc.ser.o elg_thrd.ser.o esd_event.ser.o \
		$(PFORM_TOPOL).ser.o
ESD_MPI_OBJS = $(ESD_BASE_OBJS) esd_proc.mpi.o elg_thrd.mpi.o esd_event.ser.o \
		elg_sync.o $(PFORM_TOPOL).mpi.o
ESD_OMP_OBJS = $(ESD_BASE_OBJS) esd_proc.ser.o elg_thrd.omp.o esd_event.omp.o \
		$(PFORM_TOPOL).omp.o
ESD_HYB_OBJS = $(ESD_BASE_OBJS) esd_proc.mpi.o elg_thrd.ompi.o esd_event.omp.o \
		elg_sync.o $(PFORM_TOPOL).ompi.o

all: libesd.ser.a $(MPIELGLIB) $(OMPELGLIB) $(HYBELGLIB) \
                  $(OTFMPIELGLIB) $(OTFOMPELGLIB) $(OTFHYBELGLIB) 

$(MPIELGLIB) $(OTFMPIELGLIB) : libesd.mpi.a
$(OMPELGLIB) $(OTFOMPELGLIB) : libesd.omp.a
$(HYBELGLIB) $(OTFHYBELGLIB) : libesd.ompi.a

libesd.ser.a : $(ESD_SER_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(ESD_SER_OBJS) 
	@echo "*** $@ built"
libesd.mpi.a : $(ESD_MPI_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(ESD_MPI_OBJS)
	@echo "*** $@ built"
libesd.omp.a : $(ESD_OMP_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(ESD_OMP_OBJS)
	@echo "*** $@ built"
libesd.ompi.a : $(ESD_HYB_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(ESD_HYB_OBJS)
	@echo "*** $@ built"

$(PFORM_MOD).o: platform/$(PFORM_MOD).c platform/elg_pform.h 
	$(PLATCC) $(PLAT_CFLAGS) $(CFLAGS) $(PLAT_LDIR) $(PLAT_NNODES) -c platform/$(PFORM_MOD).c

$(PFORM_TOPOL).ser.o : platform/$(PFORM_MOD).c platform/elg_pform.h \
	platform/$(PFORM_TOPOL).c platform/epk_pform_topol.h
	$(CC) $(PLAT_CFLAGS) $(CFLAGS) -c platform/$(PFORM_TOPOL).c -o $@
$(PFORM_TOPOL).mpi.o : platform/$(PFORM_MOD).c platform/elg_pform.h \
	platform/$(PFORM_TOPOL).c platform/epk_pform_topol.h
	$(MPICC) $(PLAT_CFLAGS) $(CFLAGS) -DELG_MPI -c platform/$(PFORM_TOPOL).c -o $@
$(PFORM_TOPOL).ompi.o : platform/$(PFORM_MOD).c platform/elg_pform.h \
	platform/$(PFORM_TOPOL).c platform/epk_pform_topol.h
	$(HYBCC) $(PLAT_CFLAGS) $(CFLAGS) $(OMPFLAG) -DELG_OMPI \
	-c platform/$(PFORM_TOPOL).c -o $@
$(PFORM_TOPOL).omp.o : platform/$(PFORM_MOD).c platform/elg_pform.h \
	platform/$(PFORM_TOPOL).c platform/epk_pform_topol.h
	$(OMPCC) $(PLAT_CFLAGS) $(CFLAGS) $(OMPFLAG) -DELG_OMP \
	-c platform/$(PFORM_TOPOL).c -o $@

esd_gen.o : esd_gen.c esd_gen.h esd_run.h esd_def.c esd_def.h
	$(CC) $(CFLAGS) -c esd_gen.c
esd_paths.o: esd_paths.c esd_paths.h
	$(CC) $(CFLAGS) -c esd_paths.c
esd_unify.o : esd_unify.c ../utils/elg_error.h ../epilog/base/elg_readcb.h \
	elg_defs.h ../config/elg_types.h elg_impl.h ../utils/epk_hashtab.h \
	../utils/epk_vector.h esd_unify.h ../epilog/base/elg_rw.h \
	../utils/epk_idmap.h ../epilog/base/elg_readcb_dummies.c.gen
	$(CC) $(CFLAGS) -c esd_unify.c
esd_report.o : esd_report.c esd_report.h esd_unify.h esd_paths.h
	$(CC) $(CFLAGS) -c esd_report.c
elg_sync.o : elg_sync.c elg_sync.h
	$(MPICC) $(CFLAGS) -c elg_sync.c

elg_topol.o: elg_topol.c ../config/epk_fwrapper_def.h
	$(CC) $(CFLAGS)  -I../epilog/rts -c elg_topol.c -o $@
	
epk_topol.o: epk_topol.c epk_ftopol.c ../config/epk_fwrapper_def.h
	$(CC) $(CFLAGS)  -I../epilog/rts -c epk_topol.c -o $@

esd_metric_$(METR).o: esd_metric_$(METR).c
	$(CC) $(CFLAGS) $(METRINC) -c esd_metric_$(METR).c 	

esd_proc.ser.o: esd_proc.c esd_proc.h elg_defs.h ../config/elg_types.h \
	esd_gen.h elg_impl.h ../utils/epk_archive.h ../utils/epk_idmap.h \
	esd_unify.h ../epilog/base/elg_rw.h ../utils/epk_vector.h \
	../utils/epk_hashtab.h esd_paths.h esd_report.h platform/elg_pform.h \
	platform/elg_pform_defs.h ../utils/elg_error.h ../utils/epk_conf.h
	$(CC) $(CFLAGS) -c esd_proc.c -o $@
esd_proc.mpi.o: esd_proc.c esd_proc.h elg_defs.h ../config/elg_types.h \
	esd_gen.h elg_impl.h ../utils/epk_archive.h ../utils/epk_idmap.h \
	esd_unify.h ../epilog/base/elg_rw.h ../utils/epk_vector.h \
	../utils/epk_hashtab.h esd_paths.h esd_report.h platform/elg_pform.h \
	platform/elg_pform_defs.h ../utils/elg_error.h ../utils/epk_conf.h \
	../adapter/MPI/epk_mpireg.h
	$(MPICC) $(CFLAGS) -DELG_MPI $(MPIAINC) -c esd_proc.c -o $@

THRD_CFLAGS = $(CFLAGS) -I../epilog/rts $(METRCFLAGS) $(COMPCFLAG)

esd_event.ser.o: esd_event.c esd_event.h
	$(CC) $(THRD_CFLAGS) -c esd_event.c -o $@
esd_event.omp.o: esd_event.c esd_event.h
	$(OMPCC) $(THRD_CFLAGS) $(OMPFLAG) -DELG_OMP -c esd_event.c -o $@

elg_thrd.ser.o: elg_thrd.c elg_thrd.h
	$(CC) $(THRD_CFLAGS) -c elg_thrd.c -o $@
elg_thrd.mpi.o: elg_thrd.c elg_thrd.h
	$(MPICC) $(THRD_CFLAGS) -DELG_MPI -c elg_thrd.c -o $@
elg_thrd.omp.o: elg_thrd.c elg_thrd.h
	$(OMPCC) $(THRD_CFLAGS) $(OMPFLAG) -DELG_OMP -c elg_thrd.c -o $@
elg_thrd.ompi.o: elg_thrd.c elg_thrd.h
	$(HYBCC) $(THRD_CFLAGS) $(OMPFLAG) -DELG_OMPI -c elg_thrd.c -o $@

install: all
	$(MKDIR) -m 755 $(INCDIR)
	@list='$(USER_HDRS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done

clean:
	cd platform; $(MAKE) clean
	rm -f *.o lib*.a

