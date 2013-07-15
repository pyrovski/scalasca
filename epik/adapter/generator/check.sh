#!/bin/sh

for f in *.h.gen
do
  echo "=== $f ==="
  b=`basename $f .gen`
  awk 'BEGIN { P=0; }\
       /-- do not change --/ { P=1; }\
       /\^\^\^\^\^\^\^\^\^/  { P=0; }\
       { if ( P == 1 ) print; }' ../$b > $b.GEN
  diff $b.GEN $f
done

rm -f *.GEN

for f in *.c.gen
do
  echo "=== $f ==="
  diff $f ../$f
done
