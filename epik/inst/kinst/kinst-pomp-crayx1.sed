/^           COMP=${arg}/a\
           case ${COMP} in\
           *f*) OBIN="${OBIN} --nosrc"\
                USESOMP=yes\
                FPP=""\
                FTR="cat"\
                ;;\
           esac

/-lmpi\*) #USESMPI#/i\
  -O*task0|task0) USESOMP=no\
         ARGS="${ARGS} ${arg}"\
         ;;

s/-homp) #USESOMP#/-h*omp|omp) #USESOMP#/
