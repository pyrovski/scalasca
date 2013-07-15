include Makefile.defs


# -- Default instrumentation of routines by compiler
PREP      = scalasca -instrument
# build without any instrumentation
#PREP	  = scalasca -instrument -mode=none

# -- Manual EPIK instrumentation (only)
PREP_EPIK = $(PREP) -user -comp=none

# -- Manual POMP instrumentation (only)
PREP_POMP = $(PREP) -pomp -comp=none

# -- Automatic PDT instrumentation (needs PDToolkit)
PREP_PDT  = $(PREP) -pdt -comp=none

# -- Support for explicit instrumentation
ECONFFLAG  = -DEPIK
VCONFFLAG  = -DVT_INST
CONFLIB   = `kconfig --mpi --$(PREC) --libs`
FECONFFLAG = $(FDOPT)EPIK
FVCONFFLAG = $(FDOPT)VT_INST
FCONFLIB  = `kconfig --for --mpi --$(PREC) --libs`


ifdef MPICC
    MCC = $(MPICC) $(PFLAG)
else
    MCC = -mode=none echo NO_MPICC
endif
ifdef MPIF90
    MF90 = $(MPIF90) $(PFLAG) $(FPP)
else
    MF90 = -mode=none echo NO_MPIF90
endif
ifdef HYBCC
    HCC = $(HYBCC) $(PFLAG) $(OMPFLAG) -DOPENMP
else
    HCC = -mode=none echo NO_HYBCC
endif
ifdef HYBF90
    HF90 = $(HYBF90) $(PFLAG) $(OMPFLAG) $(FPP) $(FDOPT)OPENMP
else
    HF90 = -mode=none echo NO_HYBF90
endif

# -- MPI test cases
MCPROGS   = ctest-mpi ftest-mpi
MPPROGS   = ctest-pomp-mpi ftest-pomp-mpi
MEPROGS   = ctest-epik-mpi ftest-epik-mpi
MOPROGS   = ctest-elg-mpi ftest-elg-mpi
MTPROGS   = ctest-pdt-mpi ftest-pdt-mpi
MVPROGS   = ctest-vt-mpi ftest-vt-mpi
MXPROGS   = ctest-xt-mpi ftest-xt-mpi

# -- Hybrid test cases (MPI/OpenMP)
HCPROGS   = ctest-hyb ftest-hyb
HPPROGS   = ctest-pomp-hyb ftest-pomp-hyb
#HEPROGS   = ctest-epik-hyb ftest-epik-hyb
#HOPROGS   = ctest-elg-hyb ftest-elg-hyb
HTPROGS   = ctest-pdt-hyb ftest-pdt-hyb
#HVPROGS   = ctest-vt-hyb ftest-vt-hyb
HXPROGS   = ctest-xt-hyb ftest-xt-hyb

# -- Required with and without instrumentation
CONFINC   = `kconfig --$(PREC) --cflags`
FCONFINC  = `kconfig --for --$(PREC) --cflags`


# -- Rules --
all: auto-progs pomp-progs epik-progs elg-progs pdt-progs

clean:
	rm -rf core a.* *.o
	rm -f *.inc *.mod.* opari.* *.tau.* *.pdb
	rm -f pompregions_*
	rm -f $(MCPROGS) $(MPPROGS) $(MEPROGS) $(MOPROGS) $(MVPROGS) \
              $(MXPROGS) $(MTPROGS)
	rm -f $(HCPROGS) $(HPPROGS) $(HEPROGS) $(HOPROGS) $(HVPROGS) \
              $(HXPROGS) $(HTPROGS)

auto-progs:
	$(MAKE) $(MCPROGS)
	@if test "$(OMPFLAG)" = "-OpenMPnotAvailable"; then \
		echo "INFO: Skipping hybrid auto-progs"; \
	else \
		echo $(MAKE) $(HCPROGS); \
		eval $(MAKE) $(HCPROGS); \
	fi
pomp-progs:
	$(MAKE) $(MPPROGS)
	@if test "$(OMPFLAG)" = "-OpenMPnotAvailable"; then \
		echo "INFO: Skipping hybrid pomp-progs"; \
	else \
		echo $(MAKE) $(HPPROGS); \
		eval $(MAKE) $(HPPROGS); \
	fi
epik-progs:
	$(MAKE) $(MEPROGS)
# Skip hybrid, since no EPIK instrumentation in OpenMP part
	@echo "INFO: Skipping hybrid epik-progs"
elg-progs:
	$(MAKE) $(MOPROGS)
# Skip hybrid, since no ELG instrumentation in OpenMP part
	@echo "INFO: Skipping hybrid elg-progs"
vt-progs:
	$(MAKE) $(MVPROGS)
# Skip hybrid, since no VT instrumentation in OpenMP part
	@echo "INFO: Skipping hybrid vt-progs"
xt-progs:
	$(MAKE) $(MXPROGS)
	@if test "$(OMPFLAG)" = "-OpenMPnotAvailable"; then \
		echo "INFO: Skipping hybrid xt-progs"; \
	else \
		echo $(MAKE) $(HXPROGS); \
		eval $(MAKE) $(HXPROGS); \
	fi
pdt-progs:
	@if test `kinst -pdt | grep -c "not supported"` -eq 0; then \
		$(MAKE) real-pdt-progs; \
	else \
		echo "INFO: Skipping pdt-progs"; \
	fi
real-pdt-progs:
	$(MAKE) $(MTPROGS)
	@if test "$(OMPFLAG)" = "-OpenMPnotAvailable"; then \
		echo "INFO: Skipping hybrid pdt-progs"; \
	else \
		echo $(MAKE) $(HTPROGS); \
		eval $(MAKE) $(HTPROGS); \
	fi


# -- Build rules / Automatic compiler instrumentation --
ctest-mpi: ctest.c
	$(PREP) $(MCC) ctest.c -o ctest-mpi $(MPILIB)

ftest-mpi: ftest.F90
	$(PREP) $(MF90) ftest.F90 -o ftest-mpi $(MPILIB)

ctest-hyb: ctest.c
	$(PREP) $(HCC) ctest.c -o ctest-hyb $(MPILIB)

ftest-hyb: ftest.F90
	$(PREP) $(HF90) ftest.F90 -o ftest-hyb $(MPILIB)


# -- Build rules / Automatic compiler instrumentation --
# -- (separate compile and link) --
ctest-xt-mpi: ctest.c
	$(PREP) $(MCC) ctest.c -c
	$(PREP) $(MCC) ctest.o -o ctest-xt-mpi $(MPILIB)

ftest-xt-mpi: ftest.F90
	$(PREP) $(MF90) ftest.F90 -c
	$(PREP) $(MF90) ftest.o -o ftest-xt-mpi $(MPILIB)

ctest-xt-hyb: ctest.c
	$(PREP) $(HCC) ctest.c -c
	$(PREP) $(HCC) ctest.o -o ctest-xt-hyb $(MPILIB)

ftest-xt-hyb: ftest.F90
	$(PREP) $(HF90) ftest.F90 -c
	$(PREP) $(HF90) ftest.o -o ftest-xt-hyb $(MPILIB)


# -- Build rules / Manual POMP instrumentation --
ctest-pomp-mpi: ctest-pomp.c
	$(PREP_POMP) $(MCC) ctest-pomp.c -o ctest-pomp-mpi $(MPILIB)

ftest-pomp-mpi: ftest-pomp.F90
	$(PREP_POMP) $(MF90) ftest-pomp.F90 -o ftest-pomp-mpi $(MPILIB)

ctest-pomp-hyb: ctest-pomp.c
	$(PREP_POMP) $(HCC) ctest-pomp.c -o ctest-pomp-hyb $(MPILIB)

ftest-pomp-hyb: ftest-pomp.F90
	$(PREP_POMP) $(HF90) ftest-pomp.F90 -o ftest-pomp-hyb $(MPILIB)


# -- Build rules / Manual EPIK instrumentation --
ctest-epik-mpi: ctest-epik.c
	$(MCC) $(ECONFFLAG) $(CONFINC) ctest-epik.c \
		-o ctest-epik-mpi $(CONFLIB) $(MPILIB)

ftest-epik-mpi: ftest-epik.F90
	$(MF90) $(FECONFFLAG) $(FCONFINC) ftest-epik.F90 \
		-o ftest-epik-mpi $(FCONFLIB) $(MPILIB)

ctest-epik-hyb: ctest-epik.c
	$(PREP_EPIK) $(HCC) ctest-epik.c -o ctest-epik-hyb $(MPILIB)

ftest-epik-hyb: ftest-epik.F90
	$(PREP_EPIK) $(HF90) ftest-epik.F90 -o ftest-epik-hyb $(MPILIB)

# -- Build rules / Manual ELG instrumentation --
ctest-elg-mpi: ctest-elg.c
	$(MCC) -DEPILOG $(CONFINC) ctest-elg.c \
		-o ctest-elg-mpi $(CONFLIB) $(MPILIB)

ftest-elg-mpi: ftest-elg.F90
	$(MF90) $(FDOPT)EPILOG $(FCONFINC) ftest-elg.F90 \
		-o ftest-elg-mpi $(FCONFLIB) $(MPILIB)

# -- Build rules / VampirTrace instrumentation --
ctest-vt-mpi: ctest-vt.c
	$(MCC) $(VCONFFLAG) $(CONFINC) ctest-vt.c \
		-o ctest-vt-mpi $(CONFLIB) $(MPILIB)

ftest-vt-mpi: ftest-vt.F90
	$(MF90) $(FVCONFFLAG) $(FCONFINC) ftest-vt.F90 \
		-o ftest-vt-mpi $(FCONFLIB) $(MPILIB)

# -- Build rules / PDT instrumentation --
ctest-pdt-mpi: ctest.c
	$(PREP_PDT) $(MCC) ctest.c -o ctest-pdt-mpi $(MPILIB)

ftest-pdt-mpi: ftest.F90
	$(PREP_PDT) $(MF90) ftest.F90 -o ftest-pdt-mpi $(MPILIB)

ctest-pdt-hyb: ctest.c
	$(PREP_PDT) $(HCC) ctest.c -o ctest-pdt-hyb $(MPILIB)

ftest-pdt-hyb: ftest.F90
	$(PREP_PDT) $(HF90) ftest.F90 -o ftest-pdt-hyb $(MPILIB)


