#---------------------------------------------------------------------------
# EARL Test - Makefile
#---------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES: .cpp .o

ELGDIR      = ../../../epilog/src
ELGINC      = -I$(ELGDIR)
ELGLIB      = -L$(ELGDIR) -lelg.base

EARLDIR     = ..
EARLINC     = -I$(EARLDIR)
EARLLIB     = -L$(EARLDIR) -learl

MYCXXFLAGS  = 

all: earl_test earl_cart epoch_test

earl_test: $(EARLDIR)/libearl.a $(ELGDIR)/libelg.a earl_test.o 
	$(ECXX) -o $@ earl_test.o $(EARLLIB) $(ELGLIB) $(METRLIB) $(MYCXXFLAGS)

earl_cart: $(EARLDIR)/libearl.a $(ELGDIR)/libelg.a earl_cart.o 
	$(ECXX) -o $@ earl_cart.o $(EARLLIB) $(ELGLIB) $(METRLIB) $(MYCXXFLAGS)

epoch_test: $(EARLDIR)/libearl.a $(ELGDIR)/libelg.a epoch_test.o 
	$(ECXX) -o $@ epoch_test.o $(EARLLIB) $(ELGLIB) $(METRLIB) $(MYCXXFLAGS)

earl_test.o: earl_test.cpp
	$(ECXX) $(ELGINC) $(EARLINC) $(MYCXXFLAGS) -c earl_test.cpp

earl_cart.o: earl_cart.cpp
	$(ECXX) $(ELGINC) $(EARLINC) $(MYCXXFLAGS) -c earl_cart.cpp

epoch_test.o: epoch_test.cpp
	$(ECXX) $(ELGINC) $(EARLINC) $(MYCXXFLAGS) -I. -I.. -c epoch_test.cpp


clean:
	rm -f *~ *.o  '#'* core 
	rm -f earl_test earl_cart epoch_test 
# DO NOT DELETE
