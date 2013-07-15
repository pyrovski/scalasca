#!/bin/sh

# -- Uncomment for stand-alone use
#CFGecho () {
#  echo "$1"
#}

#ErrorAndExit () {
#  echo "ERROR: $1"
#  exit 1
#}

CFGecho "Configuring ${CF}"

if [ -n "${PREFIX}" ]
then
  CFGecho "- Setting PREFIX=${PREFIX}"
  sed -e 's,^PREFIX = .*$,PREFIX = '${PREFIX}',' ${CF} > sed.out \
      && mv sed.out ${CF} || exit 1
fi

if [ -n "${SWCLOCK}" ]
then
  CFGecho "- Enabling switch clock"
  sed -e 's,^PLAT_CFLAGS =$,& -DUSE_SWITCH_CLOCK,' \
      -e 's,^UTILLIB  =$,& -lswclock,' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${MMTIMER}" ]
then
  CFGecho "- Enabling multimedia timer"
  sed -e 's,^PLAT_CFLAGS =$,& -DUSE_MMTIMER,' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${ISKCOMPUTER}" ]
then
  CFGecho "- Detected K Computer"
  sed -e 's/Fujitsu/K/' \
      -e 's/FX10/Computer/' \
      -e 's@/opt/FJSVfxlang/GM-1.2.1-03@/opt/FJSVtclang/GM-1.2.0-11@' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${COMP}" ]
then
  CFGecho "- Configuring for ${COMP} v${COMPVER} compilers"

  # Disable building hybrid MPI/OpenMP analysis components on platforms
  # which are known to cause trouble
  if [ "${COMP}" = "gnu" ]
  then
    case "${COMPVER}" in
      4.2.*)
        HYBRID=""
        sed -e 's,^OMPCXX,#OMPCXX,' \
            -e 's,^HYBCXX,#HYBCXX,' \
            ${CF} > sed.out && mv sed.out ${CF} || exit 1
        ;;
    esac
  fi
fi

# Enable new IBM compiler instrumentation interface for XLC/XLF > 11/13
if [ -n "${HASXLCOMP}" ]
then
  XLC_MAJOR=`echo ${CVER} | sed -e 's/\..*$//'`
  XLF_MAJOR=`echo ${FVER} | sed -e 's/\..*$//'`

  if [ ${XLC_MAJOR} -ge 11 ]
  then
    if [ ${XLF_MAJOR} -lt 13 ]
    then
      ErrorAndExit "Unsupported combination of compiler versions!"
    fi
    sed -e 's/^CINSTFLAG.*$/CINSTFLAG = -qfunctrace/' \
        -e 's/^#DEMANGLE/DEMANGLE/' -e 's/^#BFDLIB/BFDLIB/' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi
fi

if [ -n "${GFORTRAN}" ]
then
  CFGecho "- Using gfortran instead of g77"
  sed -e 's,^F77.*$,F77      = gfortran,' \
      -e 's,^F90.*$,F90      = gfortran,' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${MPI}" ]
then
  if [ ! -f "../mf/mpi-${MPI}.def" ]
  then
    ErrorAndExit "Unknown MPI '${MPI}'"
  fi
  CFGecho "- Configuring ${MPI} MPI library"
  sed -e '/^MPI_CONFIG_TARGETS/r '../mf/mpi-${MPI}.def -e '/^MPI/d' \
      -e '/^[FP]MPI/d' -e '/^#MPI/d' -e '/^#[FP]MPI/d' ${CF} > sed.out \
      && mv sed.out ${CF} || exit 1

  # Check whether RMA is supported and enabled
  # TODO: Guard parts in scout with more fine grained defines
  if [ ! -z "`echo ${EXCL_MPI_DEFS} | grep RMA`" -o ! -z "${DISABLE_MPI_RMA_SUPPORT}" ]; then
      CFGecho "- Disabling MPI-2 RMA analysis"
      sed -e 's,-DHAS_MPI2_1SIDED,,' \
          ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi

fi

if [ -n "${OPARI2DIR}" ]
then
  if [ "${OPARI2DIR}" = "${PREFIX}" ]
  then
    CFGecho "- Configuring opari2 (using builtin)"
  else
    CFGecho "- Configuring opari2 (using ${OPARI2DIR})"
  fi
  sed -e 's,^OPARI2DIR = .*$,OPARI2DIR = '${OPARI2DIR}',' ${CF} > sed.out \
      && mv sed.out ${CF} || exit 1
fi

if [ "${CF}" != "Makefile.defs.fe" ]
then
  if [ -n "${BINUTILSDIR}" ]; then
    CFGecho "- Configuring Binutils (using ${BINUTILSDIR})"
    sed -e "s|^BINUTILS.*$|BINUTILS     = ${BINUTILSDIR}|" ${CF} > sed.out \
        && mv sed.out ${CF} || exit 1
  fi
fi

if [ "${CF}" = "Makefile.defs.fe" ]
then
  # Enable metrics
  if [ -n "${PAPIDIR}" ]
  then
    sed -e 's,^#METRCFLAGS,METRCFLAGS,' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi
else
  # PAPI is not needed on frontend
  if [ -n "${PAPIDIR}" ]
  then
    CFGecho "- Configuring PAPI (using ${PAPIDIR})"
    sed -e 's,^#METR,METR,' -e 's,^#PAPI,PAPI,' \
        -e 's,^METRDIR.*$,METRDIR    = '${PAPIDIR}',' \
        -e "s,\$(PAPI_INCLUDE_OPTS),${PAPI_INCLUDE_OPTS}," \
        -e "s,\$(PAPI_POST_LINK_OPTS),${PAPI_POST_LINK_OPTS}," \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
    if [ "${CF}" != "Makefile.defs.be" ]
    then
      if [ -e ${PAPIDIR}/lib64/libpapi.a -o -e ${PAPIDIR}/lib64/libpapi.so ]
      then
        sed -e '/METRLDIR64/s/lib$/lib64/' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
      fi
      if [ -e ${PAPIDIR}/lib/libpapi64.a -o -e ${PAPIDIR}/lib/libpapi64.so ]
      then
        sed -e '/METRLIB64/s/papi$/papi64/' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
      fi
      if [ -e ${PAPIDIR}/lib64/libpapi64.a -o -e ${PAPIDIR}/lib64/libpapi64.so ]
      then
        sed -e '/METRLDIR64/s/lib$/lib64/' \
            -e '/METRLIB64/s/papi$/papi64/' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
      fi
    fi
  fi
fi

if [ "${CF}" != "Makefile.defs.be" ]
then
  # VTF3 is not needed on backend
  if [ -n "${VTF3DIR}" ]
  then
    CFGecho "- Configuring VTF3 (using ${VTF3DIR})"
    sed -e 's,^#VTF3,VTF3,' -e 's,^#\(.*_VTF3 \),\1,' \
        -e 's,^VTF3DIR.*$,VTF3DIR     = '${VTF3DIR}',' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi
fi

if [ -n "${OTFDIR}" ]
then
  CFGecho "- Configuring OTF (using ${OTFDIR})"
  sed -e 's,^#OTF,OTF,' -e 's,^#\(.*_OTF \),\1,' \
      -e 's,^OTFDIR.*$,OTFDIR      = '${OTFDIR}',' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ "${CF}" != "Makefile.defs.be" ]
then
  if [ -n "${PDTMPIINC}" ]
  then
    sed -e 's,^#PDTMPIINC.*$,#PDTMPIINC = '${PDTMPIINC}',' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi
  if [ -n "${PDTARCH}" ]
  then
    CFGecho "- Configuring PDToolkit (using ${PDTDIR}/${PDTARCH})"
    sed -e 's,^#PDT,PDT,' \
        -e 's,^PDTARCH.*$,PDTARCH   = '${PDTARCH}',' \
        -e 's,^PDTDIR.*$,PDTDIR    = '${PDTDIR}',' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
  fi
fi

if [ -n "${SLOG2DIR}" ]
then
  CFGecho "- Configuring SLOG2 (using ${SLOG2DIR})"
  sed -e 's,^#SLOG2,SLOG2,' -e 's,SLOG2DIR.*$,SLOG2DIR = '${SLOG2DIR}',' \
      ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${SZLIB}" ]
then
  CFGecho "- Disabling trace compression"
  sed -e 's,^SZLIB,#SZLIB,' ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
fi

if [ -n "${SIONLIB}" ]
then
  CFGecho "- Enabling SIONlib v${SIONVER} for trace file I/O"
  sed -e 's,^#SIONLIB,SIONLIB,' \
      -e 's,^SIONLIB_CONFIG.*$,SIONLIB_CONFIG      = '${SIONLIBCONFIG}',' \
       ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
fi

if [ -n "${DISABLE}" ]
then
  CFGecho "- Disabling ${DISABLE} measurement support"
  case ${DISABLE} in
    mpi) sed -e 's,^MPI,#MPI,' -e 's,^[PF]MPI,#&,' -e 's,^HYB,#HYB,' \
             -e 's,^OTFMPI,#OTFMPI,' -e 's,^OTFHYB,#OTFHYB,' \
                ${CF} > sed.out && mv sed.out ${CF} || exit 1
        ;;
    omp) sed -e 's,^OMP,#OMP,' -e 's,^HYB,#HYB,' \
             -e 's,^OTFOMP,#OTFOMP,' -e 's,^OTFHYB,#OTFHYB,' \
             -e 's,^#OMPCC .*$,OMPCC      = \$(CC),' \
             -e 's,^#OMPFLAG .*$,OMPFLAG    = -OpenMPnotAvailable,' \
                ${CF} > sed.out && mv sed.out ${CF} || exit 1
        ;;
    ompi) sed -e 's,^MPI,#MPI,' -e 's,^[PF]MPI,#&,' \
                  -e 's,^OMP,#OMP,' -e 's,^HYB,#HYB,' \
              -e 's,^OTFMPI,#OTFMPI,' -e 's,^OTFOMP,#OTFOMP,' \
              -e 's,^OTFHYB,#OTFHYB,' \
              -e 's,^#OMPCC .*$,OMPCC      = \$(CC),' \
              -e 's,^#OMPFLAG .*$,OMPFLAG    = -OpenMPnotAvailable,' \
                 ${CF} > sed.out && mv sed.out ${CF} || exit 1
        ;;
  esac
fi

if [ -n "${NOFOR}" ]
then
  sed -e 's,^F77,#F77,' -e'/^#F77/a\
F77=' -e 's,^F90,#F90,' -e'/^#F90/a\
F90=' -e 's,^MPIF90,#MPIF90,' -e'/^#MPIF90/a\
MPIF90=' -e 's,^OMPF90,#OMPF90,' -e'/^#OMPF90/a\
OMPF90=' -e 's,^HYBF90,#HYBF90,' -e'/^#HYBF90/a\
HYBF90=' ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
    CFGecho "- Disabling support for Fortran"
fi

# Fix CFGKEY and CONFIG variables
if [ "${CF}" != "Makefile.defs.be" ]
then
    sed -e 's,^CFGKEY.*$,CFGKEY = '${CFG}',' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
    sed -e 's,^CONFIG.*$,CONFIG = '"${CFGMSG}"',' \
        ${CF} > sed.out && mv sed.out ${CF} || exit 1
fi

if [ -n "${PREC}" ]
then
  if [ -n "${DEFPREC}" ]
  then
    CFGecho "- Using default ${PREC}-bit compilation mode"
  else
    CFGecho "- Setting ${PREC}-bit compilation mode"
  fi
  sed -e 's,^PREC .*$,PREC   = '${PREC}',' ${CF} \
      > sed.out &&  mv sed.out ${CF} || exit 1
fi
if [ "${CF}" = "Makefile.defs" -a "${MF}" = "Makefile.32-64" ]
then
  sed -e 's,^PREC .*$,DEFAULTPREC   = '${PREC}',' ${CF} \
      > sed.out &&  mv sed.out ${CF} || exit 1
fi

if [ "${CF}" != "Makefile.defs.be" ]
then
  # Qt is not needed on backend
  if [ -n "${QT}" ]
  then
    CFGecho "- Using Qt version ${QT}"
    sed -e 's,^#QT,QT,' \
        -e 's,^QT_QMAKE \(.*\)=.*$,QT_QMAKE \1= '${QMAKE}',' \
        ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
  fi
fi

# For combined 32/64-bit builds, compile Qt-based GUI with precision
# of Qt library
if [ "${CF}" = "Makefile.defs" -a "${MF}" = "Makefile.32-64" -a \
     -n "${QT}" -a "${QTPREC}" = "64" ]
then
  sed -e 's,^\(CUBE_BUILD_TARGETS64.*\)$,\1 \$(QT_BUILD_TARGETS),' \
      -e 's,^\(CUBE_INSTALL_TARGETS64.*\)$,\1 \$(QT_INSTALL_TARGETS),' \
    ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
else
  sed -e 's,^\(CUBE_BUILD_TARGETS .*\)$,\1 \$(QT_BUILD_TARGETS),' \
      -e 's,^\(CUBE_INSTALL_TARGETS .*\)$,\1 \$(QT_INSTALL_TARGETS),' \
    ${CF} > sed.out &&  mv sed.out ${CF} || exit 1
fi
