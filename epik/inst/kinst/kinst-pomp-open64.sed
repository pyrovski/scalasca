s/^OPARI2DECL=.*$/OPARI2DECL="--nodecl"/
s/-openmp/-fopenmp|-openmp|-mp/
/-@/i\
  -col*)\
         LENGTH=`echo $arg | sed -e 's/-col//'`\
         PDTF95OPTS="${PDTF95OPTS} -ffixed-line-length-${LENGTH}"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -freeform)\
         PDTF95OPTS="${PDTF95OPTS} -R free"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -fixedform)\
         PDTF95OPTS="${PDTF95OPTS} -R fixed"\
         ARGS="${ARGS} ${arg}"\
         ;;
s/^FSUFFIX=.*$/FSUFFIX="__"/
