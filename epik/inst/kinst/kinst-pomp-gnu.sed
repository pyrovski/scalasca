/-@/i\
  -ffixed-line-length-132)\
         PDTF95OPTS="${PDTF95OPTS} -ffixed-line-length-132"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -ffree-form)\
         PDTF95OPTS="${PDTF95OPTS} -R free"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -ffixed-form)\
         PDTF95OPTS="${PDTF95OPTS} -R fixed"\
         ARGS="${ARGS} ${arg}"\
         ;;
