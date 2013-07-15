#!/bin/sh

# -- Uncomment for stand-alone use
#CFGecho () {
#  echo "$1"
#}

#ErrorAndExit () {
#  echo "ERROR: $1"
#  exit 1
#}

qtComments () {
  if [ -z "${QT}" ]; then
    CFGecho ""
    if [ "${QTVER}" = "tooOld" ]; then
      CFGecho "NOTE: No suitable version of Qt package found: need version >= 4.3"
    else
      CFGecho "NOTE: No Qt package found (no Qt qmake in \$PATH)"
    fi
    CFGecho "      - skipping building Qt-based CUBE GUI"
  else
    if [ "${ECXX}" != "${QTCXX}" ];  then
      if [ "${SYSTEM}" = "Linux" -a "${ECXX}" = "icpc" ];  then
        CFGecho ""
        CFGecho "NOTE: QT_MKSPEC set to \"linux-icc\""
        if [ -z "${MDF}" ]; then
          sed -e 's,^QT_MKSPEC.*$,QT_MKSPEC           = linux-icc,' Makefile.defs > sed.out \
              && mv sed.out Makefile.defs || exit 1
        else
          sed -e 's,^QT_MKSPEC.*$,QT_MKSPEC           = linux-icc,' Makefile.defs.fe > sed.out \
              && mv sed.out Makefile.defs.fe || exit 1
        fi
      elif [ "${SYSTEM}" = "Darwin" ]; then
        case "${ECXX}" in
          g++)  QTSPEC="macx-g++" ;;
          icpc) QTSPEC="macx-icc" ;;
          xlc)  QTSPEC="macx-xlc" ;;
          *)    ErrorAndExit "Compiler '${COMPCXX}' not supported on Mac OS X"
                ;;
        esac
        CFGecho ""
        CFGecho "NOTE: QT_MKSPEC set to \"${QTSPEC}\""
        sed -e "s,QT_MKSPEC \(.*\)=.*$,QT_MKSPEC \1= ${QTSPEC}," \
            Makefile.defs > sed.out && mv sed.out Makefile.defs || exit 1
        sed -e "s,QT_PLAT \(.*\)=.*$,QT_PLAT \1= -macx," \
            Makefile.defs > sed.out && mv sed.out Makefile.defs || exit 1
      else
        CFGecho ""
        CFGecho "NOTE: Potential C++ compiler mismatch between ECXX(${ECXX}) and Qt(${QTCXX})"
        CFGecho "      - adapt ECXX, QT_MKSPEC, or use different Qt package if necessary"
      fi
    fi
    if [ "${PREC}" != "${QTPREC}" -a "${MF}" != "Makefile.32-64" ]; then
      CFGecho ""
      CFGecho "NOTE: Potential precision mismatch between compiler(${PREC}bit) and Qt(${QTPREC}bit)"
      CFGecho "      - use different Qt package if necessary"
    fi
  fi
}


QT=""
QTCXX="unknown"
QTVER=""

# check for Qt and get version + C++ compiler + precision
QMAKELIST="${QMAKE:-qmake-qt4 qmake}"
for QMAKE in ${QMAKELIST}; do
  QTM=`which ${QMAKE} 2>/dev/null`
  QTB=`basename ${QMAKE}`
  case "${QTM}" in
  */${QTB})
    QTOUT=`${QMAKE} -v 2>&1 | grep Qt`
    if [ -n "$QTOUT" ]; then
      QT=`echo $QTOUT | sed -e 's:^Using Qt[^0-9]*\([.0-9]*\).*:\1:g'`
      case ${QT} in
        [1-3].*|4.[012]*) QTVER=tooOld; QT="";;
        4.[3-9]*)         QTVER=OK; QMAKE="${QTM}"; break;;
        *)                QTVER=unknown; QT="";;
      esac
    fi
  esac
done

if [ -n "${QT}" ]; then
  # get C++ compiler
  QTDIR=`dirname ${QTM} | sed -e 's:\(.*\)/bin:\1:g'`
  QTPATHS="`strings ${QTM} | grep "qt_dat" | sed -e 's:.*=\(.*\):\1:g'` $QTDIR"
  for QTPATH in $QTPATHS; do
    if [ -d "$QTPATH/mkspecs" ]; then
      QTSPEC=`ls -ld $QTPATH/mkspecs/default | sed -e 's/.*-> *//'`
      break
    fi
  done
  if [ -n "${QTSPEC}" ]; then
    cfgfiles="qmake.conf \
              `grep include\( $QTPATH/mkspecs/$QTSPEC/qmake.conf \
                  | sed -e 's,include(\(.*\)),\1,'`"
    for cfgfile in $cfgfiles; do
      QTCXX=`grep "QMAKE_CXX[^A-Z_]" $QTPATH/mkspecs/$QTSPEC/$cfgfile \
               | awk '{ print $3; }'`
      if [ -n "${QTCXX}" ]; then
        break
      fi
    done
    if [ -z "${QTCXX}" ]; then
      QTCXX="unknown"
    fi
  fi

  # Get precision (TODO: need to resolve symlinks here)
  if file ${QTM} | grep 64 > /dev/null 2>&1; then
    QTPREC=64
  else
    QTPREC=32
  fi
fi
