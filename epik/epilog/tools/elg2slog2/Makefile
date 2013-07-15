#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs


#------------------------------------------------------------------------------
# Local Settings
#------------------------------------------------------------------------------

SLOG2TRCDIR = ${SLOG2DIR}/src/logformat/trace
PICFLAG_xlc =
PICFLAG_gcc = -fPIC
PICFLAG_icc = -fPIC
PICFLAG     = ${PICFLAG_${CC}}
SHRDFLAG_xlc =
SHRDFLAG_gcc = -shared
SHRDFLAG_icc = -shared
SHRDFLAG     = ${SHRDFLAG_${CC}}

EPIKDIR = ../../..
ELGDIR  = ${EPIKDIR}/epilog
EINCS   = -I${EPIKDIR}/config -I${EPIKDIR}/episode -I${EPIKDIR}/utils \
          -I${ELGDIR}/base -I${ELGDIR}/tools  

MYCFLAGS = ${CFLAGS} ${PICFLAG} ${EINCS} -I.

#------------------------------------------------------------------------------
# Rules
#------------------------------------------------------------------------------

OBJS = so_elg_conv.o so_elg_conv_msgqueue.o so_elg_error.o so_epk_conf.o \
	so_elg_readcb.o so_elg_rw.o so_epk_archive.o \
	trace_impl.o logformat_trace_InputLog.o

all: libTraceInput.so

libTraceInput.so: ${OBJS}
	${CC} ${SHRDFLAG} ${OBJS} -o $@ ${SCALIB}

# -- slog2 files

SLOGHDRS = trace_impl.h logformat_trace_InputLog.h trace_API.h

trace_impl.h: gen_slogconf.c
	${CC} gen_slogconf.c -o gen_slogconf
	./gen_slogconf > trace_impl.h
	rm gen_slogconf

trace_API.h: ${SLOG2TRCDIR}/trace_API.h
	cp ${SLOG2TRCDIR}/trace_API.h .

logformat_trace_InputLog.h: ${SLOG2TRCDIR}/logformat_trace_InputLog.h
	cp ${SLOG2TRCDIR}/logformat_trace_InputLog.h .

logformat_trace_InputLog.o: ${SLOG2TRCDIR}/logformat_trace_InputLog.c ${SLOGHDRS}
	${CC} ${MYCFLAGS} $< -c -o $@

# -- EPIK files

trace_impl.o: trace_impl.c ${SLOGHDRS}
	${CC} ${MYCFLAGS} $< -c -o $@

so_elg_conv.o: ${ELGDIR}/tools/elg_conv.c ${ELGDIR}/tools/elg_conv.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_elg_conv_msgqueue.o: ${ELGDIR}/tools/elg_conv_msgqueue.c \
	${ELGDIR}/tools/elg_conv_msgqueue.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_elg_readcb.o: ${ELGDIR}/base/elg_readcb.c ${ELGDIR}/base/elg_readcb.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_elg_rw.o: ${ELGDIR}/base/elg_rw.c ${ELGDIR}/base/elg_rw.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_elg_error.o: ${EPIKDIR}/utils/elg_error.c ${EPIKDIR}/utils/elg_error.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_epk_conf.o: ${EPIKDIR}/utils/epk_conf.c ${EPIKDIR}/utils/epk_conf.h
	${CC} ${MYCFLAGS} $< -c -o $@

so_epk_archive.o: ${EPIKDIR}/utils/epk_archive.c ${EPIKDIR}/utils/epk_archive.h
	${CC} ${MYCFLAGS} $< -c -o $@

install:
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 755 libTraceInput.so $(LIBDIR)
	
clean:
	rm -rf core ${OBJS} libTraceInput.so

realclean: clean
	rm -rf ${SLOGHDRS}
