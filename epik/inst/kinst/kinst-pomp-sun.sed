s/-\*32)  PREC=32/-m32|-xarch=*|-xtarget=*) PREC=32/
s/-\*64)  PREC=64/-m64|-xarch=v9*|-xarch=*64|-xtarget=*64) PREC=64/
s/-.*) #USESOMP#/-*openmp|-*openmp=parallel|-*openmp=noopt) #USESOMP#/
