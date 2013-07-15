#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

OPARI2INC = -I${OPARI2DIR}/include \
            -I${TOPDIR}/opari2/include
OPARI2DEVINC = -I${OPARI2DIR}/share/opari2/devel \
               -I${TOPDIR}/opari2/src/pomp-lib-dummy/

FWRAPPER_H = ../../config/epk_fwrapper_def.h

CFLAGS += -I. -I../../config -I../../utils -I../../episode ${OPARI2INC} ${OPARI2DEVINC}

.SUFFIXES:  .cpp .c .f

# Fortran base modules
FOBASE      = epk_fwrapper_base1.o \
	      epk_fwrapper_base2.o \
	      epk_fwrapper_base3.o \
	      epk_fwrapper_base4.o

# Non-OMP-specific modules
USR_OBJS    = epk_pomp2_base.o epk_region_info.o $(FOBASE)

# OMP-specific modules, built with OMPCC for OMP & OMPI
OMP_OBJS    = epk_omplock.o \
	      epk_ompreg.o \
	      epk_pomp2.o \
	      epk_fpomp1.o \
	      epk_fpomp2.o \
	      epk_fpomp3.o \
	      epk_fpomp4.o \
	      epk_pomp2_obase.o \
	      epk_fwrapper1.o \
	      epk_fwrapper2.o \
	      epk_fwrapper3.o \
	      epk_fwrapper4.o \
	      epk_region_info.o $(FOBASE)

USR_LIB = libepk.ad.usr.a
OMP_LIB = libepk.ad.omp.a

ELGOMPLIBS = $(OMPELGLIB) $(HYBELGLIB) $(OTFOMPELGLIB) $(OTFHYBELGLIB)

all: $(FWRAPPER_H) $(USR_LIB) $(ELGOMPLIBS)

$(ELGOMPLIBS) : $(OMP_LIB)

$(OMP_LIB) : $(OMP_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(OMP_OBJS)
	@echo "*** $(OMP_LIB) built"

$(USR_LIB) : $(USR_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(USR_OBJS)
	@echo "*** $(USR_LIB) built"

epk_omplock.o : epk_omplock.c epk_omplock.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -c epk_omplock.c
epk_ompreg.o : epk_ompreg.c epk_ompreg.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -c epk_ompreg.c

epk_pomp2_base.o: epk_pomp2_base.c epk_omplock.h epk_ompreg.h epk_pomp2.h
	$(CC) $(CFLAGS) -c epk_pomp2_base.c
epk_region_info.o: epk_region_info.c
	$(CC) $(CFLAGS) -c epk_region_info.c
epk_pomp2_obase.o: epk_pomp2_base.c epk_omplock.h epk_ompreg.h epk_pomp2.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -DELG_OMP -c epk_pomp2_base.c -o $@

epk_pomp2.o: epk_pomp2.c epk_omplock.h epk_ompreg.h epk_pomp2.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -I../../epilog/rts -c epk_pomp2.c

epk_fpomp1.o: epk_fpomp.c $(FWRAPPER_H) \
	epk_fpomp.h epk_pomp2.h epk_omplock.h epk_ompreg.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -I../../epilog/rts -DFSUB=FSUB1 -c epk_fpomp.c -o epk_fpomp1.o

epk_fpomp2.o: epk_fpomp.c $(FWRAPPER_H) \
	epk_fpomp.h epk_pomp2.h epk_omplock.h epk_ompreg.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -I../../epilog/rts -DFSUB=FSUB2 -c epk_fpomp.c -o epk_fpomp2.o

epk_fpomp3.o: epk_fpomp.c $(FWRAPPER_H) \
	epk_fpomp.h epk_pomp2.h epk_omplock.h epk_ompreg.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -I../../epilog/rts -DFSUB=FSUB3 -c epk_fpomp.c -o epk_fpomp3.o

epk_fpomp4.o: epk_fpomp.c $(FWRAPPER_H) \
	epk_fpomp.h epk_pomp2.h epk_omplock.h epk_ompreg.h
	$(OMPCC) $(CFLAGS) $(OMPFLAG)  -I../../epilog/rts -DFSUB=FSUB4 -c epk_fpomp.c -o epk_fpomp4.o

epk_fwrapper_base1.o: epk_fwrapper_base.c $(FWRAPPER_H) 
	$(CC) $(CFLAGS) -DFSUB=FSUB1 -c epk_fwrapper_base.c -o epk_fwrapper_base1.o

epk_fwrapper_base2.o: epk_fwrapper_base.c $(FWRAPPER_H) 
	$(CC) $(CFLAGS) -DFSUB=FSUB2 -c epk_fwrapper_base.c -o epk_fwrapper_base2.o

epk_fwrapper_base3.o: epk_fwrapper_base.c $(FWRAPPER_H) 
	$(CC) $(CFLAGS) -DFSUB=FSUB3 -c epk_fwrapper_base.c -o epk_fwrapper_base3.o

epk_fwrapper_base4.o: epk_fwrapper_base.c $(FWRAPPER_H) 
	$(CC) $(CFLAGS) -DFSUB=FSUB4 -c epk_fwrapper_base.c -o epk_fwrapper_base4.o

epk_fwrapper1.o: epk_fwrapper.c $(FWRAPPER_H) 
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -DFSUB=FSUB1 -c epk_fwrapper.c -o epk_fwrapper1.o

epk_fwrapper2.o: epk_fwrapper.c $(FWRAPPER_H) 
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -DFSUB=FSUB2 -c epk_fwrapper.c -o epk_fwrapper2.o

epk_fwrapper3.o: epk_fwrapper.c $(FWRAPPER_H) 
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -DFSUB=FSUB3 -c epk_fwrapper.c -o epk_fwrapper3.o

epk_fwrapper4.o: epk_fwrapper.c $(FWRAPPER_H) 
	$(OMPCC) $(CFLAGS) $(OMPFLAG) -DFSUB=FSUB4 -c epk_fwrapper.c -o epk_fwrapper4.o

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	@if test -f $(USR_LIB) ; then \
		echo "$(INSTALL) -c -m 644 $(USR_LIB) $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $(USR_LIB) $(LIBDIR)" ; \
	fi
	@if test -f $(OMP_LIB) ; then \
		echo "$(INSTALL) -c -m 644 $(OMP_LIB) $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $(OMP_LIB) $(LIBDIR)" ; \
	fi

clean:
	rm -f *.o getfname $(USR_LIB) $(OMP_LIB)
