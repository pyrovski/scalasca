#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES: .c

SZLIB_OBJS = adler32.o compress.o crc32.o gzio.o uncompr.o deflate.o trees.o \
             zutil.o inflate.o infback.o inftrees.o inffast.o


#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

all: libsc.z.a

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(MKDIR) -m 755 $(INCDIR)
	$(INSTALL) -c -m 644 libsc.z.a $(LIBDIR)

clean:
	rm -f $(SZLIB_OBJS) libsc.z.a


libsc.z.a: $(SZLIB_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(SZLIB_OBJS)

.c.o:
	$(CC) $(CFLAGS) $(SZLIB_OPTFLAGS) -c $<


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

adler32.o: zlib.h zconf.h
compress.o: zlib.h zconf.h
crc32.o: crc32.h zlib.h zconf.h
deflate.o: deflate.h zutil.h zlib.h zconf.h
gzio.o: zutil.h zlib.h zconf.h
inffast.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inflate.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
infback.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inftrees.o: zutil.h zlib.h zconf.h inftrees.h
trees.o: deflate.h zutil.h zlib.h zconf.h trees.h
uncompr.o: zlib.h zconf.h
zutil.o: zutil.h zlib.h zconf.h
