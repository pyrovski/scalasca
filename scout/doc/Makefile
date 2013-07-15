#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.PHONY: all install clean


# Analyzer version
VERSION = 1.4


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all:

clean:

install:
	$(MKDIR) -m 755 $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/logo_small.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/regions.html $(DOCDIR)/html/scalasca_regions-$(VERSION).html
	(cd $(DOCDIR)/html; ln -s -f scalasca_regions-$(VERSION).html scalasca_regions.html)
	$(INSTALL) -c -m 644 patterns/score_tree.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/patterns.html $(DOCDIR)/html/scalasca_patterns-$(VERSION).html
	(cd $(DOCDIR)/html; ln -s -f scalasca_patterns-$(VERSION).html scalasca_patterns.html)
	$(INSTALL) -c -m 644 patterns/Imbalance.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/Imbal_Overload.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/Imbal_Single.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/Imbal_Underload.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/Imbal_Bypass.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/Imbal_Singularity.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/BarrierCompletion.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/EarlyReduce.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/EarlyScan.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/LateBroadcast.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/LateReceiver.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/LateSender.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/LSWO_DifferentSource.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/LSWO_SameSource.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/NxNCompletion.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/WaitAtBarrier.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/WaitAtNxN.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/OMPIdle.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/OMPLimitedParallelism.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/OMPThreadManagement.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/OMPThreadFork.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaEarlyFence.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaEarlyTransfer.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaEarlyWait.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaLateComplete.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaLatePost.png $(DOCDIR)/html
	$(INSTALL) -c -m 644 patterns/RmaWaitAtFence.png $(DOCDIR)/html
