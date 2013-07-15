/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _ELG_DEFS_H
#define _ELG_DEFS_H

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 * 
 *  - Definition of data types and symbolic constants
 *
 *-----------------------------------------------------------------------------
 */

#include <limits.h>

/*
 *-----------------------------------------------------------------------------
 * Data types and byte order (machine dependent)
 *-----------------------------------------------------------------------------
 */

#include "elg_types.h"

/*
 *-----------------------------------------------------------------------------
 * Byte ordering schemes
 *
 * - Currently, the EPILOG format supports only little and big endian, but
 *   alternative byte order schemes may be added later
 * 
 *-----------------------------------------------------------------------------
 */

#define ELG_LITTLE_ENDIAN          1
#define ELG_BIG_ENDIAN             2
 
/*
 *-----------------------------------------------------------------------------
 * Absent information
 *-----------------------------------------------------------------------------
 */

#define ELG_NO_ID                  0xFFFFFFFF
#define ELG_NO_LNO                 0xFFFFFFFF
#define ELG_NO_NUM                 0xFFFFFFFF

/*
 *-----------------------------------------------------------------------------
 * Logical values
 *-----------------------------------------------------------------------------
 */

#define ELG_TRUE                   1
#define ELG_FALSE                  0

/*
 *-----------------------------------------------------------------------------
 * Record types
 *-----------------------------------------------------------------------------
 */

/* -- Definition record types < 100 -- */

#define ELG_STRING                 1
#define ELG_STRING_CNT             2
#define ELG_MACHINE                3
#define ELG_NODE                   4
#define ELG_PROCESS                5
#define ELG_THREAD                 6
#define ELG_LOCATION               7
#define ELG_FILE                   8
#define ELG_REGION                 9
#define ELG_METRIC                10
#define ELG_MPI_COMM              11
#define ELG_OFFSET                12
#define ELG_LAST_DEF              13
#define ELG_NUM_EVENTS            14
#define ELG_CALL_SITE             15
#define ELG_CART_TOPOLOGY         16
#define ELG_CART_COORDS           17
#define ELG_MPI_WIN               18
#define ELG_MPI_COMM_CNT          19
#define ELG_CALL_PATH             20
#define ELG_EVENT_TYPES           21
#define ELG_EVENT_COUNTS          22
#define ELG_MAP_SECTION           23
#define ELG_MAP_OFFSET            24
#define ELG_IDMAP                 25
#define ELG_IDMAP_CNT             26
#define ELG_MPI_GROUP             27
#define ELG_MPI_GROUP_CNT         28
#define ELG_MPI_COMM_REF          29
#define ELG_MPI_COMM_DIST         30

/* -- Attributes -- */

#define ELG_ATTR_UI1              60
#define ELG_ATTR_UI4              61

#define ELG_FIRST_ATTR            ELG_ATTR_UI1
#define ELG_LAST_ATTR             ELG_ATTR_UI4

/* -- Event record types -- */

#define ELG_ENTER                101
#define ELG_EXIT                 102
#define ELG_MPI_SEND             103
#define ELG_MPI_RECV             104
#define ELG_MPI_COLLEXIT         105
#define ELG_OMP_FORK             106
#define ELG_OMP_JOIN             107
#define ELG_OMP_ALOCK            108
#define ELG_OMP_RLOCK            109
#define ELG_OMP_COLLEXIT         110
#define ELG_ENTER_CS             111

/* -- One-Sided event types -- */

#define ELG_MPI_PUT_1TS          112 /* transfer start of MPI put */
#define ELG_MPI_PUT_1TE          113 /* transfer end of MPI put */
#define ELG_MPI_GET_1TS          114 /* transfer start of MPI get */
#define ELG_MPI_GET_1TE          115 /* transfer end of MPI get */
#define ELG_MPI_GET_1TO          116 /* call origin of MPI get at origin */

#define ELG_MPI_WINEXIT          117
#define ELG_MPI_WINCOLLEXIT      118
#define ELG_MPI_WIN_LOCK         119
#define ELG_MPI_WIN_UNLOCK       120

#define ELG_MPI_PUT_1TE_REMOTE   121 /* transfer end of put at remote*/
#define ELG_MPI_GET_1TS_REMOTE   122 /* transfer start of get at remote */

#define ELG_PUT_1TS              123 /* transfer start of put */
#define ELG_PUT_1TE              124 /* transfer end of put */
#define ELG_GET_1TS              125 /* transfer start of get */
#define ELG_GET_1TE              126 /* transfer end of get */
#define ELG_PUT_1TE_REMOTE       127 /* transfer end of put at remote*/
#define ELG_GET_1TS_REMOTE       128 /* transfer start of get at remote */
#define ELG_COLLEXIT             129
#define ELG_ALOCK                130
#define ELG_RLOCK                131

/* -- Enhanced nonblocking event types -- */

#define ELG_MPI_SEND_COMPLETE    132
#define ELG_MPI_RECV_REQUEST     133
#define ELG_MPI_REQUEST_TESTED   134
#define ELG_MPI_CANCELLED        135

#define ELG_LAST_EXTERNAL_EVENT  ELG_MPI_CANCELLED

/* -- Other event types -- */

#define ELG_LOG_OFF              201
#define ELG_LOG_ON               202
#define ELG_ENTER_TRACING        203
#define ELG_EXIT_TRACING         204

#define ELG_ALL_EVENTS           255
#define ELG_MAX_EVENT            255

/*
 *-----------------------------------------------------------------------------
 * Region type
 *-----------------------------------------------------------------------------
 */

#define ELG_UNKNOWN                0

#define ELG_FUNCTION               1
#define ELG_LOOP                   2
#define ELG_USER_REGION            3

#define ELG_FUNCTION_COLL_BARRIER  4
#define ELG_FUNCTION_COLL_ONE2ALL  5
#define ELG_FUNCTION_COLL_ALL2ONE  6
#define ELG_FUNCTION_COLL_ALL2ALL  7
#define ELG_FUNCTION_COLL_OTHER    8

#define ELG_OMP_PARALLEL          11
#define ELG_OMP_LOOP              12
#define ELG_OMP_SECTIONS          13
#define ELG_OMP_SECTION           14
#define ELG_OMP_WORKSHARE         15
#define ELG_OMP_SINGLE            16
#define ELG_OMP_MASTER            17
#define ELG_OMP_CRITICAL          18
#define ELG_OMP_ATOMIC            19
#define ELG_OMP_BARRIER           20
#define ELG_OMP_IBARRIER          21
#define ELG_OMP_FLUSH             22
#define ELG_OMP_CRITICAL_SBLOCK   23
#define ELG_OMP_SINGLE_SBLOCK     24
#define ELG_OMP_ORDERED           25
#define ELG_OMP_ORDERED_SBLOCK    26
/*
 *-----------------------------------------------------------------------------
 * MPI2 Window lock types
 *-----------------------------------------------------------------------------
 */

#define ELG_MPI2_LOCK_SHARED       0
#define ELG_MPI2_LOCK_EXCL         1

/*
 *-----------------------------------------------------------------------------
 * Performance metrics
 *-----------------------------------------------------------------------------
 */

#define ELG_INTEGER                0
#define ELG_FLOAT                  1

#define ELG_COUNTER                0
#define ELG_RATE                   1
#define ELG_SAMPLE                 2

#define ELG_START                  0
#define ELG_LAST                   1
#define ELG_NEXT                   2

/*
 *-----------------------------------------------------------------------------
 * Identifier mapping modes
 *-----------------------------------------------------------------------------
 */

#define ELG_MAP_DENSE              0
#define ELG_MAP_SPARSE             1

/*
 *-----------------------------------------------------------------------------
 * Attributes
 *-----------------------------------------------------------------------------
 */

#define ELG_ATTR_ANYNESS           1
#define ELG_ATTR_REQUEST           2
#define ELG_ATTR_BYTES             3
#define ELG_ATTR_REGION            4

/*
 *-----------------------------------------------------------------------------
 * Communicator and group encoding
 *-----------------------------------------------------------------------------
 */

#define ELG_GROUP_FLAG             1  /* indicating this is a group */
#define ELG_GROUP_WORLD            1  /* masking bit to check for world group */
#define ELG_GROUP_SELF             2  /* masking bit to check for identity group */

#define ELG_ENCODING_MASK         15  /* 4 significant bits to specify encoding */
#define ELG_ENCODING_OFFSET        2  /* number of lower bits not used for encoding */
#define ELG_GET_ENCODING(mask)     (((mask) >> ELG_ENCODING_OFFSET) & ELG_ENCODING_MASK)
#define ELG_MASK_ENCODING(enc)     ((enc) << ELG_ENCODING_OFFSET)

/**
 * Enumeration type defining type of group vector representation in
 * memory
 */
typedef enum {
  ELG_GROUP_ENCODING_BITVECTOR = 0,    /**< Legacy bitvector representation */
  ELG_GROUP_ENCODING_SELF,             /**< SELF representation */
  ELG_GROUP_ENCODING_RLE,              /**< Byte-level run length encoded representation */
  ELG_GROUP_ENCODING_SPARSE,           /**< Sparse representation */
  ELG_GROUP_ENCODING_INV_SPARSE,       /**< Inverse sparse repesentation */
  ELG_GROUP_ENCODING_STRIDE            /**< Strided representation using (start,end,stride) */
} elg_group_encoding_t;

#endif
