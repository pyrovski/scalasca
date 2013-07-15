/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#if USHRT_MAX == 4294967295
  typedef unsigned short elg_ui4;
#elif UINT_MAX == 4294967295
  typedef unsigned int elg_ui4;
#elif ULONG_MAX == 4294967295
  typedef unsigned long elg_ui4;
#else
# error "Cannot find suitable type for elg_ui4"
#endif



void gen_elg_types() {
  FILE *out;
  union {
    elg_ui4 i;
    char    c[4];
  } v;
  int i;

  if ( (out = fopen("elg_types.h", "w")) == NULL ) {
    perror("elg_types.h");
    exit(1);
  }

  fprintf(out, "#ifndef _ELG_TYPES_H\n");
  fprintf(out, "#define _ELG_TYPES_H\n\n");

  if ( sizeof(char) == 1 ) {
    fprintf(out, "typedef unsigned      char elg_ui1;"
            "   /* unsigned integer of size 1 Byte */\n");
  } else {
    fprintf(stderr, "ERROR: cannot find unsigned integer of size 1 Byte\n");
    exit(1);
  }


  if ( sizeof(short) == 4 ) {
    fprintf(out, "typedef unsigned     short elg_ui4;"
            "   /* unsigned integer of size 4 Byte */\n");
  } else if ( sizeof(int) == 4 ) {
    fprintf(out, "typedef unsigned       int elg_ui4;"
            "   /* unsigned integer of size 4 Byte */\n");
  } else if ( sizeof(long) == 4 ) {
    fprintf(out, "typedef unsigned      long elg_ui4;"
            "   /* unsigned integer of size 4 Byte */\n");
  } else {
    fprintf(stderr, "ERROR: cannot find unsigned integer of size 4 Byte\n");
    exit(1);
  }

  if ( sizeof(long) == 8 ) {
    fprintf(out, "typedef unsigned      long elg_ui8;"
            "   /* unsigned integer of size 8 Byte */\n");
  } else if ( sizeof(long long) == 8 ) {
    fprintf(out, "typedef unsigned long long elg_ui8;"
            "   /* unsigned integer of size 8 Byte */\n");
  } else {
    fprintf(stderr, "ERROR: cannot find unsigned integer of size 8 Byte\n");
    exit(1);
  }

  if ( sizeof(float) == 8 ) {
    fprintf(out, "typedef              float elg_d8; "
            "   /* real of size 8 Byte (ieee 754)  */\n");
  } else if ( sizeof(double) == 8 ) {
    fprintf(out, "typedef             double elg_d8; "
            "   /* real of size 8 Byte (ieee 754)  */\n");
  } else {
    fprintf(stderr, "ERROR: cannot find real of size 8 Byte (ieee 754)\n");
    exit(1);
  }

  v.i = 0x00010203;
  for (i=0; i<4; i++) if ( v.c[i] != i ) break;
  if ( i == 4 ) {
    fprintf(out, "\n#define ELG_BYTE_ORDER ELG_BIG_ENDIAN\n");
  } else {
    for (i=0; i<4; i++) if ( v.c[i] != 3-i ) break;
    if ( i == 4 ) {
      fprintf(out, "\n#define ELG_BYTE_ORDER ELG_LITTLE_ENDIAN\n");
    } else {
      fprintf(stderr, "ERROR: unusual byte order\n");
      exit(1);
    }
  }

  fprintf(out, "\n#endif\n");

  fclose(out);
}

#ifdef NEED_MAIN
int main() {  gen_elg_types(); }
#else
void   gen_elg_types_()  { gen_elg_types(); }
void   gen_elg_types__() { gen_elg_types(); }
void  _gen_elg_types()   { gen_elg_types(); }
void __gen_elg_types()   { gen_elg_types(); }
void  _gen_elg_types_()  { gen_elg_types(); }
void   GEN_ELF_TYPES()   { gen_elg_types(); }
void   GEN_ELF_TYPES_()  { gen_elg_types(); }
void   GEN_ELF_TYPES__() { gen_elg_types(); }
void  _GEN_ELF_TYPES()   { gen_elg_types(); }
void __GEN_ELF_TYPES()   { gen_elg_types(); }
void  _GEN_ELF_TYPES_()  { gen_elg_types(); }
#endif
