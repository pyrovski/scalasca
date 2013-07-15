#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# mandatory PLAT specification of target platform

.SUFFIXES:  .cpp .c .f

PFORM = elg_pform_$(PLAT)
PFORM_MOD = elg_pform_${PLAT}
PFORM_TOPOL = epk_pform_topol_${PLAT}


EPK_UTILDIR = ../../utils
EPK_UTILLIB = $(EPK_UTILDIR)/libepk.util.a

CFLAGS += -g -I.. -I../../config -I$(EPK_UTILDIR) -I../../epilog/rts
LIBS = $(EPK_UTILLIB) $(UTILLIB)

all: $(PFORM).o 

test: elg_pform_test
	./elg_pform_test

$(PFORM_MOD).o: $(PFORM_MOD).c elg_pform.h 
	$(PLATCC) $(PLAT_CFLAGS) $(CFLAGS) $(PLAT_LDIR) $(PLAT_NNODES) -c $(PFORM_MOD).c

$(PFORM_TOPOL).o : $(PFORM_MOD).c elg_pform.h $(PFORM_TOPOL).c epk_pform_topol.h
	$(PLATCC) $(PLAT_CFLAGS) $(CFLAGS) $(PLAT_LDIR) $(PLAT_NNODES) -c $(PFORM_TOPOL).c 


#elg_pform_test: $(EPK_UTILLIB) $(PFORM).o elg_pform_test.c 
#	$(CC) $(CFLAGS) -o $@ elg_pform_test.c $(PFORM).o $(PFORM_TOPOL).o $(LIBS) ../esd_gen.c  ../elg_topol.o

#epk_pform_topol_test: $(EPK_UTILLIB) $(PFORM).o epk_pform_topol_test.c
#	$(MPICC) $(CFLAGS) -o $@ epk_pform_topol_test.c $(PFORM).o $(LIBS) ../esd_gen.c ../elg_topol.o $(PFORM_TOPOL).o


elg_pform.h : elg_pform_defs.h

install: all

clean:
	rm -f *.o elg_pform_test epk_pform_topol_test
