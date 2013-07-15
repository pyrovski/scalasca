/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-2005 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#ifndef ZCONF_H
#define ZCONF_H

/*
 * To avoid name clashes with a system wide installed zlib library, we
 * prepend a unique prefix to all types and library functions.
 *
 * Markus Geimer <m.geimer@fz-juelich.de>
 */

#define SZ_PREFIX(identifier)     sz_  ## identifier
#define SGZ_PREFIX(identifier)    sgz_ ## identifier

/* Types */
#define Byte                      SZ_PREFIX(Byte)
#define uInt                      SZ_PREFIX(uInt)
#define uLong                     SZ_PREFIX(uLong)
#define Bytef                     SZ_PREFIX(Bytef)
#define charf                     SZ_PREFIX(charf)
#define intf                      SZ_PREFIX(intf)
#define uIntf                     SZ_PREFIX(uIntf)
#define uLongf                    SZ_PREFIX(uLongf)
#define voidpf                    SZ_PREFIX(voidpf)
#define voidp                     SZ_PREFIX(voidp)

#define alloc_func                SZ_PREFIX(alloc_func)
#define free_func                 SZ_PREFIX(free_func)
#define in_func                   SZ_PREFIX(in_func)
#define out_func                  SZ_PREFIX(out_func)

#define z_stream                  SZ_PREFIX(stream)
#define z_stream_s                SZ_PREFIX(stream_s)
#define z_stream_p                SZ_PREFIX(stream_p)

#define gz_header                 SGZ_PREFIX(header)
#define gz_header_s               SGZ_PREFIX(header_s)
#define gz_header_p               SGZ_PREFIX(header_p)

#define gzFile                    SGZ_PREFIX(File)

/* Variables */
#define z_errmsg                  SZ_PREFIX(errmsg)

#define _dist_code                SZ_PREFIX(dist_code)
#define _length_code              SZ_PREFIX(length_code)

#define inflate_copyright         SZ_PREFIX(inflate_copyright)
#define deflate_copyright         SZ_PREFIX(deflate_copyright)

/* Functions */
#define zlibVersion               SZ_PREFIX(libVersion)
#define zlibCompileFlags          SZ_PREFIX(libCompileFlags)
#define zError                    SZ_PREFIX(Error)

#define deflate                   SZ_PREFIX(deflate)
#define deflateEnd                SZ_PREFIX(deflateEnd)
#define deflateSetDictionary      SZ_PREFIX(deflateSetDictionary)
#define deflateCopy               SZ_PREFIX(deflateCopy)
#define deflateReset              SZ_PREFIX(deflateReset)
#define deflateParams             SZ_PREFIX(deflateParams)
#define deflateTune               SZ_PREFIX(deflateTune)
#define deflateBound              SZ_PREFIX(deflateBound)
#define deflatePrime              SZ_PREFIX(deflatePrime)
#define deflateSetHeader          SZ_PREFIX(deflateSetHeader)
#define deflateInit_              SZ_PREFIX(deflateInit_)
#define deflateInit2_             SZ_PREFIX(deflateInit2_)

#define inflate                   SZ_PREFIX(inflate)
#define inflateEnd                SZ_PREFIX(inflateEnd)
#define inflateSetDictionary      SZ_PREFIX(inflateSetDictionary)
#define inflateSync               SZ_PREFIX(inflateSync)
#define inflateCopy               SZ_PREFIX(inflateCopy)
#define inflateReset              SZ_PREFIX(inflateReset)
#define inflatePrime              SZ_PREFIX(inflatePrime)
#define inflateGetHeader          SZ_PREFIX(inflateGetHeader)
#define inflateBack               SZ_PREFIX(inflateBack)
#define inflateBackEnd            SZ_PREFIX(inflateBackEnd)
#define inflateInit_              SZ_PREFIX(inflateInit_)
#define inflateInit2_             SZ_PREFIX(inflateInit2_)
#define inflateBackInit_          SZ_PREFIX(inflateBackInit_)
#define inflateSyncPoint          SZ_PREFIX(inflateSyncPoint)

#define compress                  SZ_PREFIX(compress)
#define compress2                 SZ_PREFIX(compress2)
#define compressBound             SZ_PREFIX(compressBound)
#define uncompress                SZ_PREFIX(uncompress)

#define gzopen                    SGZ_PREFIX(open)
#define gzdopen                   SGZ_PREFIX(dopen)
#define gzsetparams               SGZ_PREFIX(setparams)
#define gzread                    SGZ_PREFIX(read)
#define gzwrite                   SGZ_PREFIX(write)
#define gzprintf                  SGZ_PREFIX(printf)
#define gzputs                    SGZ_PREFIX(puts)
#define gzgets                    SGZ_PREFIX(gets)
#define gzputc                    SGZ_PREFIX(putc)
#define gzgetc                    SGZ_PREFIX(getc)
#define gzungetc                  SGZ_PREFIX(ungetc)
#define gzflush                   SGZ_PREFIX(flush)
#define gzseek                    SGZ_PREFIX(seek)
#define gzrewind                  SGZ_PREFIX(rewind)
#define gztell                    SGZ_PREFIX(tell)
#define gzeof                     SGZ_PREFIX(eof)
#define gzdirect                  SGZ_PREFIX(direct)
#define gzclose                   SGZ_PREFIX(close)
#define gzerror                   SGZ_PREFIX(error)
#define gzclearerr                SGZ_PREFIX(clearerr)
#define gzgetpos                  SGZ_PREFIX(getpos)
#define gzsetpos                  SGZ_PREFIX(setpos)

#define adler32                   SZ_PREFIX(adler32)
#define adler32_combine           SZ_PREFIX(adler32_combine)
#define crc32                     SZ_PREFIX(crc32)
#define crc32_combine             SZ_PREFIX(crc32_combine)
#define get_crc_table             SZ_PREFIX(get_crc_table)

/* Internal functions */
#define zcalloc                   SZ_PREFIX(calloc)
#define zcfree                    SZ_PREFIX(cfree)

#define _tr_align                 SZ_PREFIX(tr_align)
#define _tr_flush_block           SZ_PREFIX(tr_flush_block)
#define _tr_init                  SZ_PREFIX(tr_init)
#define _tr_stored_block          SZ_PREFIX(tr_stored_block)
#define _tr_tally                 SZ_PREFIX(tr_tally)

#define inflate_fast              SZ_PREFIX(inflate_fast)
#define inflate_table             SZ_PREFIX(inflate_table)

#if defined(__MSDOS__) && !defined(MSDOS)
#  define MSDOS
#endif
#if (defined(OS_2) || defined(__OS2__)) && !defined(OS2)
#  define OS2
#endif
#if defined(_WINDOWS) && !defined(WINDOWS)
#  define WINDOWS
#endif
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__)
#  ifndef WIN32
#    define WIN32
#  endif
#endif
#if (defined(MSDOS) || defined(OS2) || defined(WINDOWS)) && !defined(WIN32)
#  if !defined(__GNUC__) && !defined(__FLAT__) && !defined(__386__)
#    ifndef SYS16BIT
#      define SYS16BIT
#    endif
#  endif
#endif

/*
 * Compile with -DMAXSEG_64K if the alloc function cannot allocate more
 * than 64k bytes at a time (needed on systems with 16-bit int).
 */
#ifdef SYS16BIT
#  define MAXSEG_64K
#endif
#ifdef MSDOS
#  define UNALIGNED_OK
#endif

#ifdef __STDC_VERSION__
#  ifndef STDC
#    define STDC
#  endif
#  if __STDC_VERSION__ >= 199901L
#    ifndef STDC99
#      define STDC99
#    endif
#  endif
#endif
#if !defined(STDC) && (defined(__STDC__) || defined(__cplusplus))
#  define STDC
#endif
#if !defined(STDC) && (defined(__GNUC__) || defined(__BORLANDC__))
#  define STDC
#endif
#if !defined(STDC) && (defined(MSDOS) || defined(WINDOWS) || defined(WIN32))
#  define STDC
#endif
#if !defined(STDC) && (defined(OS2) || defined(__HOS_AIX__))
#  define STDC
#endif

#if defined(__OS400__) && !defined(STDC)    /* iSeries (formerly AS/400). */
#  define STDC
#endif

#ifndef STDC
#  ifndef const /* cannot use !defined(STDC) && !defined(const) on Mac */
#    define const       /* note: need a more gentle solution here */
#  endif
#endif

/* Some Mac compilers merge all .h files incorrectly: */
#if defined(__MWERKS__)||defined(applec)||defined(THINK_C)||defined(__SC__)
#  define NO_DUMMY_DECL
#endif

/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#  ifdef MAXSEG_64K
#    define MAX_MEM_LEVEL 8
#  else
#    define MAX_MEM_LEVEL 9
#  endif
#endif

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#ifndef MAX_WBITS
#  define MAX_WBITS   15 /* 32K LZ77 window */
#endif

/* The memory requirements for deflate are (in bytes):
            (1 << (windowBits+2)) +  (1 << (memLevel+9))
 that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for inflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus a few kilobytes
 for small objects.
*/

                        /* Type declarations */

#ifndef OF /* function prototypes */
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif

/* The following definitions for FAR are needed only for MSDOS mixed
 * model programming (small or medium model with some far allocations).
 * This was tested only with MSC; for other MSDOS compilers you may have
 * to define NO_MEMCPY in zutil.h.  If you don't need the mixed model,
 * just define FAR to be empty.
 */
#ifdef SYS16BIT
#  if defined(M_I86SM) || defined(M_I86MM)
     /* MSC small or medium model */
#    define SMALL_MEDIUM
#    ifdef _MSC_VER
#      define FAR _far
#    else
#      define FAR far
#    endif
#  endif
#  if (defined(__SMALL__) || defined(__MEDIUM__))
     /* Turbo C small or medium model */
#    define SMALL_MEDIUM
#    ifdef __BORLANDC__
#      define FAR _far
#    else
#      define FAR far
#    endif
#  endif
#endif

#if defined(WINDOWS) || defined(WIN32)
   /* If building or using zlib as a DLL, define ZLIB_DLL.
    * This is not mandatory, but it offers a little performance increase.
    */
#  ifdef ZLIB_DLL
#    if defined(WIN32) && (!defined(__BORLANDC__) || (__BORLANDC__ >= 0x500))
#      ifdef ZLIB_INTERNAL
#        define ZEXTERN extern __declspec(dllexport)
#      else
#        define ZEXTERN extern __declspec(dllimport)
#      endif
#    endif
#  endif  /* ZLIB_DLL */
   /* If building or using zlib with the WINAPI/WINAPIV calling convention,
    * define ZLIB_WINAPI.
    * Caution: the standard ZLIB1.DLL is NOT compiled using ZLIB_WINAPI.
    */
#  ifdef ZLIB_WINAPI
#    ifdef FAR
#      undef FAR
#    endif
#    include <windows.h>
     /* No need for _export, use ZLIB.DEF instead. */
     /* For complete Windows compatibility, use WINAPI, not __stdcall. */
#    define ZEXPORT WINAPI
#    ifdef WIN32
#      define ZEXPORTVA WINAPIV
#    else
#      define ZEXPORTVA FAR CDECL
#    endif
#  endif
#endif

#if defined (__BEOS__)
#  ifdef ZLIB_DLL
#    ifdef ZLIB_INTERNAL
#      define ZEXPORT   __declspec(dllexport)
#      define ZEXPORTVA __declspec(dllexport)
#    else
#      define ZEXPORT   __declspec(dllimport)
#      define ZEXPORTVA __declspec(dllimport)
#    endif
#  endif
#endif

#ifndef ZEXTERN
#  define ZEXTERN extern
#endif
#ifndef ZEXPORT
#  define ZEXPORT
#endif
#ifndef ZEXPORTVA
#  define ZEXPORTVA
#endif

#ifndef FAR
#  define FAR
#endif

#if !defined(__MACTYPES__)
typedef unsigned char  Byte;  /* 8 bits */
#endif
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

#ifdef SMALL_MEDIUM
   /* Borland C/C++ and some old MSC versions ignore FAR inside typedef */
#  define Bytef Byte FAR
#else
   typedef Byte  FAR Bytef;
#endif
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;

#ifdef STDC
   typedef void const *voidpc;
   typedef void FAR   *voidpf;
   typedef void       *voidp;
#else
   typedef Byte const *voidpc;
   typedef Byte FAR   *voidpf;
   typedef Byte       *voidp;
#endif

#if 0           /* HAVE_UNISTD_H -- this line is updated by ./configure */
#  include <sys/types.h> /* for off_t */
#  include <unistd.h>    /* for SEEK_* and off_t */
#  ifdef VMS
#    include <unixio.h>   /* for off_t */
#  endif
#  define z_off_t off_t
#endif
#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif
#ifndef z_off_t
#  define z_off_t long
#endif

#if defined(__OS400__)
#  define NO_vsnprintf
#endif

#if defined(__MVS__)
#  define NO_vsnprintf
#  ifdef FAR
#    undef FAR
#  endif
#endif

/* MVS linker does not support external names larger than 8 bytes */
#if defined(__MVS__)
#   pragma map(deflateInit_,"DEIN")
#   pragma map(deflateInit2_,"DEIN2")
#   pragma map(deflateEnd,"DEEND")
#   pragma map(deflateBound,"DEBND")
#   pragma map(inflateInit_,"ININ")
#   pragma map(inflateInit2_,"ININ2")
#   pragma map(inflateEnd,"INEND")
#   pragma map(inflateSync,"INSY")
#   pragma map(inflateSetDictionary,"INSEDI")
#   pragma map(compressBound,"CMBND")
#   pragma map(inflate_table,"INTABL")
#   pragma map(inflate_fast,"INFA")
#   pragma map(inflate_copyright,"INCOPY")
#endif

#endif /* ZCONF_H */
