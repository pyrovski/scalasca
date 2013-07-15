#!/bin/sh

COMPVER="?.?"

if [ -z "${COMP}" ]; then
  COMPNAME=$1
else
  COMPNAME=${COMP}
fi

case ${COMPNAME} in
  pgi)  COMPVER=`pgcc -V 2>&1 | grep "pgcc" | cut -d\  -f 2`
       ;;
  intel) COMPVER=`icc -V 2>&1 | grep ^Intel | sed -e 's/^.*Version //' \
         | cut -d\  -f 1`
       ;;
  path) COMPVER=`pathcc -v 2>&1 | grep ^PathScale | sed -e 's/^.*Version //'`
       ;;
  ibm) HASXLCOMP="yes"
       CVER=`xlc -qversion 2>&1 | grep "IBM" | sed -e 's/^.*V//' \
         | cut -d\  -f 1`
       FVER=`xlf -qversion 2>&1 | grep "IBM" | sed -e 's/^.*V//' \
         | cut -d\  -f 1`
       COMPVER="${CVER}/${FVER}"
       ;;
  cce) COMPVER=`craycc -V 2>&1 | grep ^Cray | cut -d\  -f 5`
       ;;
  sun) CVER=`suncc  -V 2>&1 | grep 'Sun' | sed -e 's/^.*Sun C //' \
         | cut -d\  -f 1`
       FVER=`sunf90 -V 2>&1 | grep 'Sun' | sed -e 's/^.*Sun Fortran 95 //' \
         | cut -d\  -f 1`
       COMPVER="${CVER}/${FVER}"
       ;;
  gnu) COMPVER=`gcc -dumpversion`
       #COMPVER=`gcc -v 2>&1 | grep -i 'Version [0-9]' \
       #  | sed -e 's/^.*ersion \(.\..\..\).*$/\1/'`
       ;;
  clang) COMPVER=`clang --version 2>&1 | grep 'clang' | sed -e 's/^.*version //' \
         | cut -d\  -f 1`
       ;;
  open64) COMPVER=`opencc --version 2>&1 | head -n 1 | sed -e 's/^.*Version //'`
       ;;
  *)   ErrorAndExit "Unknown compiler '${COMPNAME}'"
       ;;
esac
