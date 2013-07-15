/*
 * Fortan subroutine external name setup
 */

#define XSUFFIX(name)  name##_
#define XSUFFIX2(name) name##__
#define XPREFIX(name)  _##name
#define XPREFIX2(name) __##name

#define SUFFIX(name)  XSUFFIX(name)
#define SUFFIX2(name) XSUFFIX2(name)
#define PREFIX(name)  XPREFIX(name)
#define PREFIX2(name) XPREFIX2(name)

#define UPCASE(name)  name##_U
#define LOWCASE(name) name##_L

#define FSUB1(name) SUFFIX(LOWCASE(name))
#define FSUB2(name) UPCASE(name)
#define FSUB3(name) LOWCASE(name)
#define FSUB4(name) SUFFIX2(LOWCASE(name))
