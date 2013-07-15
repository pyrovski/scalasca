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

static char *header =
"/*\n"
" * Fortan subroutine external name setup\n"
" */\n"
"\n"
"#define XSUFFIX(name)  name##_\n"
"#define XSUFFIX2(name) name##__\n"
"#define XPREFIX(name)  _##name\n"
"#define XPREFIX2(name) __##name\n"
"\n"
"#define SUFFIX(name)  XSUFFIX(name)\n"
"#define SUFFIX2(name) XSUFFIX2(name)\n"
"#define PREFIX(name)  XPREFIX(name)\n"
"#define PREFIX2(name) XPREFIX2(name)\n"
"\n"
"#define UPCASE(name)  name##_U\n"
"#define LOWCASE(name) name##_L\n"
"\n";

static void genStd4(char mode) {
  int next = 2;
  FILE* f = fopen("epk_fwrapper_def.h", "w");
  if (f == NULL) { perror("epk_fwrapper_def.h"); exit(1); }
 
  next = 2;
  fputs(header, f);
  switch (mode) {
    case 'U': fputs("#define FSUB1(name) UPCASE(name)\n", f);
	      break;
    case 'l': fputs("#define FSUB1(name) LOWCASE(name)\n", f);
	      break;
    case '_': fputs("#define FSUB1(name) SUFFIX(LOWCASE(name))\n", f);
	      break;
    case '2': fputs("#define FSUB1(name) SUFFIX2(LOWCASE(name))\n", f);
	      break;
  }
  if (mode != 'U') fprintf(f, "#define FSUB%d(name) UPCASE(name)\n", next++);
  if (mode != 'l') fprintf(f, "#define FSUB%d(name) LOWCASE(name)\n", next++);
  if (mode != '_') fprintf(f, "#define FSUB%d(name) SUFFIX(LOWCASE(name))\n", next++);
  if (mode != '2') fprintf(f, "#define FSUB%d(name) SUFFIX2(LOWCASE(name))\n", next++);
  fclose(f);
}

static void genExtra3(char *str) {
  FILE* f = fopen("epk_fwrapper_def.h", "w");
  if (f == NULL) { perror("epk_fwrapper_def.h"); exit(1); }
  fputs(header, f);
  fputs(str, f);
  fputs("\n", f);
  fputs("#define FSUB2(name) LOWCASE(name)\n", f);
  fputs("#define FSUB3(name) SUFFIX(LOWCASE(name))\n", f);
  fputs("#define FSUB4(name) SUFFIX2(LOWCASE(name))\n", f);
  fclose(f);
}

void   foo_foo()   { genStd4('l'); }
void   foo_foo_()  { genStd4('_'); }
void   foo_foo__() { genStd4('2'); }
void  _foo_foo()   { genExtra3("#define FSUB1(name) PREFIX(LOWCASE(name))"); }
void __foo_foo()   { genExtra3("#define FSUB1(name) PREFIX2(LOWCASE(name))"); }
void  _foo_foo_()  { genExtra3("#define FSUB1(name) PREFIX(SUFFIX(LOWCASE(name)))"); }
void   FOO_FOO()   { genStd4('U'); }
void   FOO_FOO_()  { genExtra3("#define FSUB1(name) SUFFIX(UPCASE(name))"); }
void   FOO_FOO__() { genExtra3("#define FSUB1(name) SUFFIX2(UPCASE(name))"); }
void  _FOO_FOO()   { genExtra3("#define FSUB1(name) PREFIX(UPCASE(name))"); }
void __FOO_FOO()   { genExtra3("#define FSUB1(name) PREFIX2(UPCASE(name))"); }
void  _FOO_FOO_()  { genExtra3("#define FSUB1(name) PREFIX(SUFFIX(UPCASE(name)))"); }
