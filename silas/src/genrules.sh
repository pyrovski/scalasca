#!/bin/sh

mpiCC  -DPEARL_USE_LIST -MM \
       -I../../epik/utils -I../../pearl/base -I../../pearl/MPI \
       -I../../pearl/replay -I../../cube-3.0/src/cubew *.cpp \
| sed -e 's,../../epik/utils,$(EPKDIR),g' \
      -e 's,../../pearl,$(PEARLDIR),g' \
      -e 's,../../cube-3.0/src/cubew,$(CUBEDIR),g' \
      -e 's,LIST/,$(PEARL_DS)/,g'
