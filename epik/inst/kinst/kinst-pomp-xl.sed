s/-\*32)  PREC=32/-m32|-q32) PREC=32/
s/-\*64)  PREC=64/-m64|-q64) PREC=64/
/^         OINC=""/a\
         if [ -n "$FPP" ]\
         then\
           case ${arg} in\
           *.f|*.F) FPP=" -qsuffix=cpp=F"\
           ;;\
           esac\
         fi
s/@FLIBINDICATOR@/xlf\*/
/-@/i\
  -WF,-D*)\
         PDTPARSEOPTS="${PDTPARSEOPTS} ${PDTOPTS}"\
         unset PDTOPTS\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -qfixed=132)\
         PDTF95OPTS="${PDTF95OPTS} -ffixed-line-length-132"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -qfree*)\
         PDTF95OPTS="${PDTF95OPTS} -R free"\
         ARGS="${ARGS} ${arg}"\
         ;;\
  -qfixed*)\
         PDTF95OPTS="${PDTF95OPTS} -R fixed"\
         ARGS="${ARGS} ${arg}"\
         ;;
s/^FSUFFIX=.*$/FSUFFIX=""/
