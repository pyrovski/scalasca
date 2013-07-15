#!/bin/sh
# Determine which compiler ECXX is referring to

# -- Uncomment for stand-alone use
#CFGecho () {
#  echo "$1"
#}
#
#ErrorAndExit () {
#  echo "ERROR: $1"
#  exit 1
#}


# Determine compiler and version
if [ "`$ECXX -qversion 2>&1 | grep -c '^IBM'`" -gt 0 ]; then
  COMPCXX=ibm
  COMPVER=`$ECXX -qversion 2>&1 | grep '^IBM' | sed -e 's/^.*V//' | cut -d\  -f 1`
  CFGecho "- Using IBM C++ compiler v${COMPVER}"
elif [ `$ECXX -V 2>&1 | grep -c '^Cray'` -gt 0 ]; then
  COMPCXX=cray
  COMPVER=`$ECXX -V 2>&1 | grep '^Cray' | cut -d\  -f 5`
  CFGecho "- Using Cray C++ compiler v${COMPVER}"
elif [ `$ECXX -V 2>&1 | grep -c 'Sun'` -gt 0 ]; then
  COMPCXX=sun
  COMPVER=`$ECXX -V 2>&1 | grep 'Sun' | sed -e 's/^.*Sun C++ //' | cut -d\  -f 1`
  CFGecho "- Using Sun C++ compiler v${COMPVER}"
elif [ `$ECXX -V 2>&1 | grep -c '^Intel'` -gt 0 ]; then
  COMPCXX=intel
  COMPVER=`$ECXX -V 2>&1 | grep '^Intel' | sed -e 's/^.*Version //' | cut -d\  -f 1`
  CFGecho "- Intel C++ compiler v${COMPVER}"
elif [ `$ECXX -V 2>&1 | grep -c '^pg'` -gt 0 ]; then
  COMPCXX=pgi
  COMPVER=`$ECXX -V 2>&1 | grep '^pg' | cut -d\  -f 2`
  CFGecho "- Using PGI C++ compiler v${COMPVER}"
elif [ `$ECXX -v 2>&1 | grep -c '^PathScale'` -gt 0 ]; then
  COMPCXX=pathscale
  COMPVER=`$ECXX -v 2>&1 | grep '^PathScale' | sed -e 's/^.*Version //'`
  CFGecho "- Using PathScale C++ compiler v$COMPVER"
# NB: since PathScale includes GCC, test for GCC last
elif [ `$ECXX -v 2>&1 | grep -c '^gcc'` -gt 0 ]; then
  COMPCXX=gnu
  COMPVER=`$ECXX -dumpversion`
  CFGecho "- Using GNU C++ compiler v$COMPVER"
else
  ErrorAndExit "Unable to identify C++ compiler '$ECXX'!"
fi
