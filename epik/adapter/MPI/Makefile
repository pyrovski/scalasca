#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# optional REMEXEC script if required to run executables on target platform
# optional FMPIOBJ if separate Fortran MPI wrappers required

# Include directories to other parts of the EPIK
EPKDIR          =../../utils
CFGDIR          =../../config
ESDDIR          =../../episode
              
CFLAGS         += -I$(CFGDIR) -I$(EPKDIR) -I$(ESDDIR)

# MPI-specific adapter modules, built with MPICC for MPI & OMPI
MPI_OBJS        = epk_mpicom.o \
                  epk_mpireg.o \
                  epk_mpireq.o 

MPI_C_WRAP_SRC  = $(wildcard epk_mpiwrap*.c)
MPI_F_WRAP_SRC  = $(wildcard epk_fmpiwrap_*.c)

MPI_C_WRAP_OBJS = $(patsubst %.c, %.o, $(MPI_C_WRAP_SRC))
MPI_F_WRAP_OBJS = $(patsubst %.c, %.1.o, $(MPI_F_WRAP_SRC)) \
                  $(patsubst %.c, %.2.o, $(MPI_F_WRAP_SRC)) \
                  $(patsubst %.c, %.3.o, $(MPI_F_WRAP_SRC)) \
                  $(patsubst %.c, %.4.o, $(MPI_F_WRAP_SRC)) \
                  epk_fmpiwrap_init.o epk_fmpiwrap.o

MAKEDEP         = Makefile.dep

LIB             = epk.ad.mpi
A_LIB           = lib$(LIB).a

ifneq ("$(MPIF90)","")
FLIB            = epk.ad.fmpi
F_LIB           = lib$(FLIB).a
else
# No suitable compiler available to compile Fortran adapter
FMPILIB   	=
FMPIOBJ   	=
endif

ELGMPILIBS      = $(MPIELGLIB) $(HYBELGLIB) $(OTFMPIELGLIB) $(OTFHYBELGLIB)

.SUFFIXES:  .cpp .c .f

.PHONY: all depend

all: $(ELGMPILIBS)

depend:
	mpicc -MM $(MPI_C_WRAP_SRC) $(MPI_F_WRAP_SRC) \
	  -I $(CFGDIR) \
		-I $(EPKDIR) \
		-I $(ESDDIR)  \
		-isystem `which mpicc | sed -e "s/\(.*\)\/bin\/mpicc$$/\1\/include/"` \
		| sed -e 's,$(CFGDIR),$$(CFGDIR),g' \
		      -e 's,$(EPKDIR),$$(EPKDIR),g'  \
					-e 's,$(ESDDIR),$$(ESDDIR),g' > $(MAKEDEP)
	sed -e "s/\(epk_fmpiwrap_[^\.]*\).o/\1.1.o \1.2.o \1.3.o \1.4.o/" $(MAKEDEP) > $(MAKEDEP).tmp
	mv $(MAKEDEP).tmp $(MAKEDEP)


$(ELGMPILIBS) : $(A_LIB) $(F_LIB)

# C Wrapper Library

$(A_LIB): $(MPI_OBJS) $(FMPIOBJ) $(MPI_C_WRAP_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(MPI_OBJS) $(MPI_C_WRAP_OBJS)
	@echo "*** $@ built"
	@$(C_NM) -g -P $(AFLAG) $(A_LIB) | \
	    egrep "^MPI_*" | \
	    cut -d " " -f 1 | \
	    tr "[a-z]" "[A-Z]" | \
	    sort -u \
	    > mpi-wrappers.txt
	@diff mpi-wrappers.txt $(TOPDIR)/mpi-support.defs | \
	    egrep "^>" | cut -d " " -f 2 \
	    > $(TOPDIR)/mpi-non-wrapped.log
	@echo "*** list of non-wrapped function calls generated"

epk_mpi%.o: epk_mpi%.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -c $<

epk_mpiwrap_%.o: epk_mpiwrap_%.c 
	$(MPICC) $(CFLAGS) $(COMPCFLAG) $(MPIEXTRA) -c $<

# Fortran Wrapper Library

$(F_LIB): epk_fmpiwrap.h $(CFGDIR)/epk_fwrapper_def.h \
  $(CFGDIR)/elg_fmpi_defs.h $(EPKDIR)/elg_error.h \
	$(MPI_F_WRAP_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(MPI_F_WRAP_OBJS)
	@echo "*** $@ built"
	@$(C_NM) -g -P $(AFLAG) $(F_LIB) | \
	    egrep "^MPI_*" | \
	    cut -d " " -f 1 | \
	    tr "[a-z]" "[A-Z]" | \
	    sort -u \
	    > mpi-wrappers.txt
	@diff mpi-wrappers.txt $(TOPDIR)/mpi-support.defs | \
	    egrep "^>" | cut -d " " -f 2 \
	    > $(TOPDIR)/mpi-fortran-non-wrapped.log
	@echo "*** list of non-wrapped fortran function calls generated"

epk_fmpiwrap_init.h: epk_fmpiwrap_init.o
	echo "#ifndef _EPK_FMPIWRAP_INIT_H" >  $@
	echo "#define _EPK_FMPIWRAP_INIT_H" >> $@
	INIT_CB=`$(C_NM) -g -P $(AFLAG) $< \
		| grep -i epk_mpif_init_cb \
		| head -n 1 \
		| cut -d ' ' -f 1 \
		| sed -e 's/^\.\{0,1\}//'`; \
	echo "void $$INIT_CB();" >> $@; \
	echo "#define EPK_MPIF_INIT_CALLBACK $$INIT_CB" >> $@
	echo "#endif" >> $@

epk_fmpiwrap_init.F: epk_fmpiwrap_init.F.in
	cp $< $<.tmp
	for const in MPI_BOTTOM MPI_IN_PLACE MPI_STATUS_IGNORE MPI_STATUSES_IGNORE MPI_UNWEIGHTED; \
      do \
        if [ -n "`grep $$const $(CFGDIR)/epk_defs_mpi.h`" ]; then \
          sed -e "s/^!!\(.*$$const.*\)$$/\1/" $<.tmp > $<.tmp2; \
          cp $<.tmp2 $<.tmp; \
        fi \
      done;
	rm -f $<.tmp2
	mv $<.tmp $@

epk_fmpiwrap.o: epk_fmpiwrap.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -c $< -o $@

epk_fmpiwrap_init.o: epk_fmpiwrap_init.F
	$(MPIF90) $(FFLAGS) -I$(CFGDIR) -c $< -o $@

epk_fmpiwrap_%.1.o: epk_fmpiwrap_%.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -DFSUB=FSUB1 -c $< -o epk_fmpiwrap_$*.1.o

epk_fmpiwrap_%.2.o: epk_fmpiwrap_%.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -DFSUB=FSUB2 -c $< -o epk_fmpiwrap_$*.2.o

epk_fmpiwrap_%.3.o: epk_fmpiwrap_%.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -DFSUB=FSUB3 -c $< -o epk_fmpiwrap_$*.3.o

epk_fmpiwrap_%.4.o: epk_fmpiwrap_%.c
	$(MPICC) $(CFLAGS) $(MPIEXTRA) -DFSUB=FSUB4 -c $< -o epk_fmpiwrap_$*.4.o

$(F_LIB)-t3e:
	rm -f $(F_LIB)
	$(C_AR) x /opt/ctl/mpt/mpt/lib/libmpi.a mpif.o
	$(C_AR) r $(F_LIB) mpif.o
	rm mpif.o

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	@if test -f "$(A_LIB)" ; then \
		echo "$(INSTALL) -c -m 644 $(A_LIB) $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $(A_LIB) $(LIBDIR)" ; \
	fi
	@if test -n "$(F_LIB)" -a -f "$(F_LIB)" ; then \
		echo "$(INSTALL) -c -m 644 $(F_LIB) $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $(F_LIB) $(LIBDIR)" ; \
	fi

clean:
	rm -f *.o $(A_LIB) $(F_LIB) epk_fmpiwrap_init.F epk_fmpiwrap_init.h

-include Makefile.dep
