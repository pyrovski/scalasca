/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_mpireg.h
 *
 * @brief Registration of MPI functions
 */

#ifndef _EPK_MPIREG_H
#define _EPK_MPIREG_H

#ifdef __cplusplus
#   define EXTERN extern "C" /**< External C definition*/
#else
#   define EXTERN extern     /**< External definition */
#endif

#include "elg_defs.h"

/******************************************************************************
**
**  EPIK Library (Event Processing Interface Kit)
**
**  - Registration of MPI functions
**
******************************************************************************/

/** function type is not point-to-point */
#define EPK_MPI_TYPE__NONE                          0
/** function type is receive operation */
#define EPK_MPI_TYPE__RECV                          1
/** function type is send operation */
#define EPK_MPI_TYPE__SEND                          2
/** function type is send and receive operation */
#define EPK_MPI_TYPE__SENDRECV                      3
/** function type is collective */
#define EPK_MPI_TYPE__COLLECTIVE                    4

/** function has is unknown communication pattern */
#define EPK_COLL_TYPE__UNKNOWN                      1
/** function is barrier-like operation */
#define EPK_COLL_TYPE__BARRIER                      2
/** function has 1:n communication pattern */
#define EPK_COLL_TYPE__ONE2ALL                      3
/** function has n:1 communication pattern */
#define EPK_COLL_TYPE__ALL2ONE                      4
/** function has n:n communication pattern */
#define EPK_COLL_TYPE__ALL2ALL                      5
/** function may be partially synchronizing */
#define EPK_COLL_TYPE__PARTIAL                      6
/** function is implicitely synchronizing */
#define EPK_COLL_TYPE__IMPLIED                      7

/**
 * Bitpatterns for runtime wrapper enabling
 */
enum epk_mpi_groups
{
  /* pure groups, which can be specified in conf */
  EPK_MPI_ENABLED_CG        =     1,
  EPK_MPI_ENABLED_COLL      =     2,
  EPK_MPI_ENABLED_ENV       =     4,
  EPK_MPI_ENABLED_ERR       =     8,
  EPK_MPI_ENABLED_EXT       =    16,
  EPK_MPI_ENABLED_IO        =    32,
  EPK_MPI_ENABLED_MISC      =    64,
  EPK_MPI_ENABLED_P2P       =   128,
  EPK_MPI_ENABLED_RMA       =   256,
  EPK_MPI_ENABLED_SPAWN     =   512,
  EPK_MPI_ENABLED_TOPO      =  1024,
  EPK_MPI_ENABLED_TYPE      =  2048,
  EPK_MPI_ENABLED_PERF      =  4096,
  EPK_MPI_ENABLED_XNONBLOCK =  8192,
  EPK_MPI_ENABLED_XREQTEST  = 16384,
  /* derived groups, which are a combination of existing groups */
  EPK_MPI_ENABLED_CG_ERR    =  EPK_MPI_ENABLED_CG    | EPK_MPI_ENABLED_ERR,
  EPK_MPI_ENABLED_CG_EXT    =  EPK_MPI_ENABLED_CG    | EPK_MPI_ENABLED_EXT,
  EPK_MPI_ENABLED_CG_MISC   =  EPK_MPI_ENABLED_CG    |
                              EPK_MPI_ENABLED_MISC,
  EPK_MPI_ENABLED_IO_ERR    =  EPK_MPI_ENABLED_IO    | EPK_MPI_ENABLED_ERR,
  EPK_MPI_ENABLED_IO_MISC   =  EPK_MPI_ENABLED_IO    |
                              EPK_MPI_ENABLED_MISC,
  EPK_MPI_ENABLED_RMA_ERR   =  EPK_MPI_ENABLED_RMA   | EPK_MPI_ENABLED_ERR,
  EPK_MPI_ENABLED_RMA_EXT   =  EPK_MPI_ENABLED_RMA   | EPK_MPI_ENABLED_EXT,
  EPK_MPI_ENABLED_RMA_MISC  =  EPK_MPI_ENABLED_RMA   |
                              EPK_MPI_ENABLED_MISC,
  EPK_MPI_ENABLED_TYPE_EXT  =  EPK_MPI_ENABLED_TYPE  | EPK_MPI_ENABLED_EXT,
  EPK_MPI_ENABLED_TYPE_MISC =  EPK_MPI_ENABLED_TYPE  |
                              EPK_MPI_ENABLED_MISC,
  /* NOTE: ALL should comprise all pure groups */
  EPK_MPI_ENABLED_ALL       =  EPK_MPI_ENABLED_CG    |
                              EPK_MPI_ENABLED_COLL  |
                              EPK_MPI_ENABLED_ENV   |
                              EPK_MPI_ENABLED_ERR   |
                              EPK_MPI_ENABLED_EXT   |
                              EPK_MPI_ENABLED_IO    |
                              EPK_MPI_ENABLED_MISC  |
                              EPK_MPI_ENABLED_P2P   |
                              EPK_MPI_ENABLED_PERF  |
                              EPK_MPI_ENABLED_RMA   |
                              EPK_MPI_ENABLED_SPAWN |
                              EPK_MPI_ENABLED_TOPO  |
                              EPK_MPI_ENABLED_TYPE  |
                              EPK_MPI_ENABLED_XNONBLOCK,
  /* NOTE: DEFAULT should reflect the default set in 'epk_conf.c' */
  EPK_MPI_ENABLED_DEFAULT =  EPK_MPI_ENABLED_CG    |
                            EPK_MPI_ENABLED_COLL  |
                            EPK_MPI_ENABLED_ENV   |
                            EPK_MPI_ENABLED_IO    |
                            EPK_MPI_ENABLED_P2P   |
                            EPK_MPI_ENABLED_RMA   |
                            EPK_MPI_ENABLED_TOPO
};

/** Bit vector for runtime measurement wrapper enabling/disabling */
EXTERN elg_ui4 epk_mpi_enabled;

/** EPIK region ID for MPI_Abort */
#define EPK__MPI_ABORT                              0
/** EPIK region ID for MPI_Accumulate */
#define EPK__MPI_ACCUMULATE                         1
/** EPIK region ID for MPI_Add_error_class */
#define EPK__MPI_ADD_ERROR_CLASS                    2
/** EPIK region ID for MPI_Add_error_code */
#define EPK__MPI_ADD_ERROR_CODE                     3
/** EPIK region ID for MPI_Add_error_string */
#define EPK__MPI_ADD_ERROR_STRING                   4
/** EPIK region ID for MPI_Address */
#define EPK__MPI_ADDRESS                            5
/** EPIK region ID for MPI_Allgather */
#define EPK__MPI_ALLGATHER                          6
/** EPIK region ID for MPI_Allgatherv */
#define EPK__MPI_ALLGATHERV                         7
/** EPIK region ID for MPI_Alloc_mem */
#define EPK__MPI_ALLOC_MEM                          8
/** EPIK region ID for MPI_Allreduce */
#define EPK__MPI_ALLREDUCE                          9
/** EPIK region ID for MPI_Alltoall */
#define EPK__MPI_ALLTOALL                          10
/** EPIK region ID for MPI_Alltoallv */
#define EPK__MPI_ALLTOALLV                         11
/** EPIK region ID for MPI_Alltoallw */
#define EPK__MPI_ALLTOALLW                         12
/** EPIK region ID for MPI_Attr_delete */
#define EPK__MPI_ATTR_DELETE                       13
/** EPIK region ID for MPI_Attr_get */
#define EPK__MPI_ATTR_GET                          14
/** EPIK region ID for MPI_Attr_put */
#define EPK__MPI_ATTR_PUT                          15
/** EPIK region ID for MPI_Barrier */
#define EPK__MPI_BARRIER                           16
/** EPIK region ID for MPI_Bcast */
#define EPK__MPI_BCAST                             17
/** EPIK region ID for MPI_Bsend */
#define EPK__MPI_BSEND                             18
/** EPIK region ID for MPI_Bsend_init */
#define EPK__MPI_BSEND_INIT                        19
/** EPIK region ID for MPI_Buffer_attach */
#define EPK__MPI_BUFFER_ATTACH                     20
/** EPIK region ID for MPI_Buffer_detach */
#define EPK__MPI_BUFFER_DETACH                     21
/** EPIK region ID for MPI_Cancel */
#define EPK__MPI_CANCEL                            22
/** EPIK region ID for MPI_Cart_coords */
#define EPK__MPI_CART_COORDS                       23
/** EPIK region ID for MPI_Cart_create */
#define EPK__MPI_CART_CREATE                       24
/** EPIK region ID for MPI_Cart_get */
#define EPK__MPI_CART_GET                          25
/** EPIK region ID for MPI_Cart_map */
#define EPK__MPI_CART_MAP                          26
/** EPIK region ID for MPI_Cart_rank */
#define EPK__MPI_CART_RANK                         27
/** EPIK region ID for MPI_Cart_shift */
#define EPK__MPI_CART_SHIFT                        28
/** EPIK region ID for MPI_Cart_sub */
#define EPK__MPI_CART_SUB                          29
/** EPIK region ID for MPI_Cartdim_get */
#define EPK__MPI_CARTDIM_GET                       30
/** EPIK region ID for MPI_Close_port */
#define EPK__MPI_CLOSE_PORT                        31
/** EPIK region ID for MPI_Comm_accept */
#define EPK__MPI_COMM_ACCEPT                       32
/** EPIK region ID for MPI_Comm_c2f */
#define EPK__MPI_COMM_C2F                          33
/** EPIK region ID for MPI_Comm_call_errhandler */
#define EPK__MPI_COMM_CALL_ERRHANDLER              34
/** EPIK region ID for MPI_Comm_compare */
#define EPK__MPI_COMM_COMPARE                      35
/** EPIK region ID for MPI_Comm_connect */
#define EPK__MPI_COMM_CONNECT                      36
/** EPIK region ID for MPI_Comm_create */
#define EPK__MPI_COMM_CREATE                       37
/** EPIK region ID for MPI_Comm_create_errhandler */
#define EPK__MPI_COMM_CREATE_ERRHANDLER            38
/** EPIK region ID for MPI_Comm_create_keyval */
#define EPK__MPI_COMM_CREATE_KEYVAL                39
/** EPIK region ID for MPI_Comm_delete_attr */
#define EPK__MPI_COMM_DELETE_ATTR                  40
/** EPIK region ID for MPI_Comm_disconnect */
#define EPK__MPI_COMM_DISCONNECT                   41
/** EPIK region ID for MPI_Comm_dup */
#define EPK__MPI_COMM_DUP                          42
/** EPIK region ID for MPI_Comm_f2c */
#define EPK__MPI_COMM_F2C                          43
/** EPIK region ID for MPI_Comm_free */
#define EPK__MPI_COMM_FREE                         44
/** EPIK region ID for MPI_Comm_free_keyval */
#define EPK__MPI_COMM_FREE_KEYVAL                  45
/** EPIK region ID for MPI_Comm_get_attr */
#define EPK__MPI_COMM_GET_ATTR                     46
/** EPIK region ID for MPI_Comm_get_errhandler */
#define EPK__MPI_COMM_GET_ERRHANDLER               47
/** EPIK region ID for MPI_Comm_get_name */
#define EPK__MPI_COMM_GET_NAME                     48
/** EPIK region ID for MPI_Comm_get_parent */
#define EPK__MPI_COMM_GET_PARENT                   49
/** EPIK region ID for MPI_Comm_group */
#define EPK__MPI_COMM_GROUP                        50
/** EPIK region ID for MPI_Comm_join */
#define EPK__MPI_COMM_JOIN                         51
/** EPIK region ID for MPI_Comm_rank */
#define EPK__MPI_COMM_RANK                         52
/** EPIK region ID for MPI_Comm_remote_group */
#define EPK__MPI_COMM_REMOTE_GROUP                 53
/** EPIK region ID for MPI_Comm_remote_size */
#define EPK__MPI_COMM_REMOTE_SIZE                  54
/** EPIK region ID for MPI_Comm_set_attr */
#define EPK__MPI_COMM_SET_ATTR                     55
/** EPIK region ID for MPI_Comm_set_errhandler */
#define EPK__MPI_COMM_SET_ERRHANDLER               56
/** EPIK region ID for MPI_Comm_set_name */
#define EPK__MPI_COMM_SET_NAME                     57
/** EPIK region ID for MPI_Comm_size */
#define EPK__MPI_COMM_SIZE                         58
/** EPIK region ID for MPI_Comm_spawn */
#define EPK__MPI_COMM_SPAWN                        59
/** EPIK region ID for MPI_Comm_spawn_multiple */
#define EPK__MPI_COMM_SPAWN_MULTIPLE               60
/** EPIK region ID for MPI_Comm_split */
#define EPK__MPI_COMM_SPLIT                        61
/** EPIK region ID for MPI_Comm_test_inter */
#define EPK__MPI_COMM_TEST_INTER                   62
/** EPIK region ID for MPI_Dims_create */
#define EPK__MPI_DIMS_CREATE                       63
/** EPIK region ID for MPI_Dist_graph_create */
#define EPK__MPI_DIST_GRAPH_CREATE                 64
/** EPIK region ID for MPI_Dist_graph_create_adjacent */
#define EPK__MPI_DIST_GRAPH_CREATE_ADJACENT        65
/** EPIK region ID for MPI_Dist_graph_neighbors */
#define EPK__MPI_DIST_GRAPH_NEIGHBORS              66
/** EPIK region ID for MPI_Dist_graph_neighbors_count */
#define EPK__MPI_DIST_GRAPH_NEIGHBORS_COUNT        67
/** EPIK region ID for MPI_Errhandler_create */
#define EPK__MPI_ERRHANDLER_CREATE                 68
/** EPIK region ID for MPI_Errhandler_free */
#define EPK__MPI_ERRHANDLER_FREE                   69
/** EPIK region ID for MPI_Errhandler_get */
#define EPK__MPI_ERRHANDLER_GET                    70
/** EPIK region ID for MPI_Errhandler_set */
#define EPK__MPI_ERRHANDLER_SET                    71
/** EPIK region ID for MPI_Error_class */
#define EPK__MPI_ERROR_CLASS                       72
/** EPIK region ID for MPI_Error_string */
#define EPK__MPI_ERROR_STRING                      73
/** EPIK region ID for MPI_Exscan */
#define EPK__MPI_EXSCAN                            74
/** EPIK region ID for MPI_File_c2f */
#define EPK__MPI_FILE_C2F                          75
/** EPIK region ID for MPI_File_call_errhandler */
#define EPK__MPI_FILE_CALL_ERRHANDLER              76
/** EPIK region ID for MPI_File_close */
#define EPK__MPI_FILE_CLOSE                        77
/** EPIK region ID for MPI_File_create_errhandler */
#define EPK__MPI_FILE_CREATE_ERRHANDLER            78
/** EPIK region ID for MPI_File_delete */
#define EPK__MPI_FILE_DELETE                       79
/** EPIK region ID for MPI_File_f2c */
#define EPK__MPI_FILE_F2C                          80
/** EPIK region ID for MPI_File_get_amode */
#define EPK__MPI_FILE_GET_AMODE                    81
/** EPIK region ID for MPI_File_get_atomicity */
#define EPK__MPI_FILE_GET_ATOMICITY                82
/** EPIK region ID for MPI_File_get_byte_offset */
#define EPK__MPI_FILE_GET_BYTE_OFFSET              83
/** EPIK region ID for MPI_File_get_errhandler */
#define EPK__MPI_FILE_GET_ERRHANDLER               84
/** EPIK region ID for MPI_File_get_group */
#define EPK__MPI_FILE_GET_GROUP                    85
/** EPIK region ID for MPI_File_get_info */
#define EPK__MPI_FILE_GET_INFO                     86
/** EPIK region ID for MPI_File_get_position */
#define EPK__MPI_FILE_GET_POSITION                 87
/** EPIK region ID for MPI_File_get_position_shared */
#define EPK__MPI_FILE_GET_POSITION_SHARED          88
/** EPIK region ID for MPI_File_get_size */
#define EPK__MPI_FILE_GET_SIZE                     89
/** EPIK region ID for MPI_File_get_type_extent */
#define EPK__MPI_FILE_GET_TYPE_EXTENT              90
/** EPIK region ID for MPI_File_get_view */
#define EPK__MPI_FILE_GET_VIEW                     91
/** EPIK region ID for MPI_File_iread */
#define EPK__MPI_FILE_IREAD                        92
/** EPIK region ID for MPI_File_iread_at */
#define EPK__MPI_FILE_IREAD_AT                     93
/** EPIK region ID for MPI_File_iread_shared */
#define EPK__MPI_FILE_IREAD_SHARED                 94
/** EPIK region ID for MPI_File_iwrite */
#define EPK__MPI_FILE_IWRITE                       95
/** EPIK region ID for MPI_File_iwrite_at */
#define EPK__MPI_FILE_IWRITE_AT                    96
/** EPIK region ID for MPI_File_iwrite_shared */
#define EPK__MPI_FILE_IWRITE_SHARED                97
/** EPIK region ID for MPI_File_open */
#define EPK__MPI_FILE_OPEN                         98
/** EPIK region ID for MPI_File_preallocate */
#define EPK__MPI_FILE_PREALLOCATE                  99
/** EPIK region ID for MPI_File_read */
#define EPK__MPI_FILE_READ                        100
/** EPIK region ID for MPI_File_read_all */
#define EPK__MPI_FILE_READ_ALL                    101
/** EPIK region ID for MPI_File_read_all_begin */
#define EPK__MPI_FILE_READ_ALL_BEGIN              102
/** EPIK region ID for MPI_File_read_all_end */
#define EPK__MPI_FILE_READ_ALL_END                103
/** EPIK region ID for MPI_File_read_at */
#define EPK__MPI_FILE_READ_AT                     104
/** EPIK region ID for MPI_File_read_at_all */
#define EPK__MPI_FILE_READ_AT_ALL                 105
/** EPIK region ID for MPI_File_read_at_all_begin */
#define EPK__MPI_FILE_READ_AT_ALL_BEGIN           106
/** EPIK region ID for MPI_File_read_at_all_end */
#define EPK__MPI_FILE_READ_AT_ALL_END             107
/** EPIK region ID for MPI_File_read_ordered */
#define EPK__MPI_FILE_READ_ORDERED                108
/** EPIK region ID for MPI_File_read_ordered_begin */
#define EPK__MPI_FILE_READ_ORDERED_BEGIN          109
/** EPIK region ID for MPI_File_read_ordered_end */
#define EPK__MPI_FILE_READ_ORDERED_END            110
/** EPIK region ID for MPI_File_read_shared */
#define EPK__MPI_FILE_READ_SHARED                 111
/** EPIK region ID for MPI_File_seek */
#define EPK__MPI_FILE_SEEK                        112
/** EPIK region ID for MPI_File_seek_shared */
#define EPK__MPI_FILE_SEEK_SHARED                 113
/** EPIK region ID for MPI_File_set_atomicity */
#define EPK__MPI_FILE_SET_ATOMICITY               114
/** EPIK region ID for MPI_File_set_errhandler */
#define EPK__MPI_FILE_SET_ERRHANDLER              115
/** EPIK region ID for MPI_File_set_info */
#define EPK__MPI_FILE_SET_INFO                    116
/** EPIK region ID for MPI_File_set_size */
#define EPK__MPI_FILE_SET_SIZE                    117
/** EPIK region ID for MPI_File_set_view */
#define EPK__MPI_FILE_SET_VIEW                    118
/** EPIK region ID for MPI_File_sync */
#define EPK__MPI_FILE_SYNC                        119
/** EPIK region ID for MPI_File_write */
#define EPK__MPI_FILE_WRITE                       120
/** EPIK region ID for MPI_File_write_all */
#define EPK__MPI_FILE_WRITE_ALL                   121
/** EPIK region ID for MPI_File_write_all_begin */
#define EPK__MPI_FILE_WRITE_ALL_BEGIN             122
/** EPIK region ID for MPI_File_write_all_end */
#define EPK__MPI_FILE_WRITE_ALL_END               123
/** EPIK region ID for MPI_File_write_at */
#define EPK__MPI_FILE_WRITE_AT                    124
/** EPIK region ID for MPI_File_write_at_all */
#define EPK__MPI_FILE_WRITE_AT_ALL                125
/** EPIK region ID for MPI_File_write_at_all_begin */
#define EPK__MPI_FILE_WRITE_AT_ALL_BEGIN          126
/** EPIK region ID for MPI_File_write_at_all_end */
#define EPK__MPI_FILE_WRITE_AT_ALL_END            127
/** EPIK region ID for MPI_File_write_ordered */
#define EPK__MPI_FILE_WRITE_ORDERED               128
/** EPIK region ID for MPI_File_write_ordered_begin */
#define EPK__MPI_FILE_WRITE_ORDERED_BEGIN         129
/** EPIK region ID for MPI_File_write_ordered_end */
#define EPK__MPI_FILE_WRITE_ORDERED_END           130
/** EPIK region ID for MPI_File_write_shared */
#define EPK__MPI_FILE_WRITE_SHARED                131
/** EPIK region ID for MPI_Finalize */
#define EPK__MPI_FINALIZE                         132
/** EPIK region ID for MPI_Finalized */
#define EPK__MPI_FINALIZED                        133
/** EPIK region ID for MPI_Free_mem */
#define EPK__MPI_FREE_MEM                         134
/** EPIK region ID for MPI_Gather */
#define EPK__MPI_GATHER                           135
/** EPIK region ID for MPI_Gatherv */
#define EPK__MPI_GATHERV                          136
/** EPIK region ID for MPI_Get */
#define EPK__MPI_GET                              137
/** EPIK region ID for MPI_Get_address */
#define EPK__MPI_GET_ADDRESS                      138
/** EPIK region ID for MPI_Get_count */
#define EPK__MPI_GET_COUNT                        139
/** EPIK region ID for MPI_Get_elements */
#define EPK__MPI_GET_ELEMENTS                     140
/** EPIK region ID for MPI_Get_processor_name */
#define EPK__MPI_GET_PROCESSOR_NAME               141
/** EPIK region ID for MPI_Get_version */
#define EPK__MPI_GET_VERSION                      142
/** EPIK region ID for MPI_Graph_create */
#define EPK__MPI_GRAPH_CREATE                     143
/** EPIK region ID for MPI_Graph_get */
#define EPK__MPI_GRAPH_GET                        144
/** EPIK region ID for MPI_Graph_map */
#define EPK__MPI_GRAPH_MAP                        145
/** EPIK region ID for MPI_Graph_neighbors */
#define EPK__MPI_GRAPH_NEIGHBORS                  146
/** EPIK region ID for MPI_Graph_neighbors_count */
#define EPK__MPI_GRAPH_NEIGHBORS_COUNT            147
/** EPIK region ID for MPI_Graphdims_get */
#define EPK__MPI_GRAPHDIMS_GET                    148
/** EPIK region ID for MPI_Grequest_complete */
#define EPK__MPI_GREQUEST_COMPLETE                149
/** EPIK region ID for MPI_Grequest_start */
#define EPK__MPI_GREQUEST_START                   150
/** EPIK region ID for MPI_Group_c2f */
#define EPK__MPI_GROUP_C2F                        151
/** EPIK region ID for MPI_Group_compare */
#define EPK__MPI_GROUP_COMPARE                    152
/** EPIK region ID for MPI_Group_difference */
#define EPK__MPI_GROUP_DIFFERENCE                 153
/** EPIK region ID for MPI_Group_excl */
#define EPK__MPI_GROUP_EXCL                       154
/** EPIK region ID for MPI_Group_f2c */
#define EPK__MPI_GROUP_F2C                        155
/** EPIK region ID for MPI_Group_free */
#define EPK__MPI_GROUP_FREE                       156
/** EPIK region ID for MPI_Group_incl */
#define EPK__MPI_GROUP_INCL                       157
/** EPIK region ID for MPI_Group_intersection */
#define EPK__MPI_GROUP_INTERSECTION               158
/** EPIK region ID for MPI_Group_range_excl */
#define EPK__MPI_GROUP_RANGE_EXCL                 159
/** EPIK region ID for MPI_Group_range_incl */
#define EPK__MPI_GROUP_RANGE_INCL                 160
/** EPIK region ID for MPI_Group_rank */
#define EPK__MPI_GROUP_RANK                       161
/** EPIK region ID for MPI_Group_size */
#define EPK__MPI_GROUP_SIZE                       162
/** EPIK region ID for MPI_Group_translate_ranks */
#define EPK__MPI_GROUP_TRANSLATE_RANKS            163
/** EPIK region ID for MPI_Group_union */
#define EPK__MPI_GROUP_UNION                      164
/** EPIK region ID for MPI_Ibsend */
#define EPK__MPI_IBSEND                           165
/** EPIK region ID for MPI_Info_c2f */
#define EPK__MPI_INFO_C2F                         166
/** EPIK region ID for MPI_Info_create */
#define EPK__MPI_INFO_CREATE                      167
/** EPIK region ID for MPI_Info_delete */
#define EPK__MPI_INFO_DELETE                      168
/** EPIK region ID for MPI_Info_dup */
#define EPK__MPI_INFO_DUP                         169
/** EPIK region ID for MPI_Info_f2c */
#define EPK__MPI_INFO_F2C                         170
/** EPIK region ID for MPI_Info_free */
#define EPK__MPI_INFO_FREE                        171
/** EPIK region ID for MPI_Info_get */
#define EPK__MPI_INFO_GET                         172
/** EPIK region ID for MPI_Info_get_nkeys */
#define EPK__MPI_INFO_GET_NKEYS                   173
/** EPIK region ID for MPI_Info_get_nthkey */
#define EPK__MPI_INFO_GET_NTHKEY                  174
/** EPIK region ID for MPI_Info_get_valuelen */
#define EPK__MPI_INFO_GET_VALUELEN                175
/** EPIK region ID for MPI_Info_set */
#define EPK__MPI_INFO_SET                         176
/** EPIK region ID for MPI_Init */
#define EPK__MPI_INIT                             177
/** EPIK region ID for MPI_Init_thread */
#define EPK__MPI_INIT_THREAD                      178
/** EPIK region ID for MPI_Initialized */
#define EPK__MPI_INITIALIZED                      179
/** EPIK region ID for MPI_Intercomm_create */
#define EPK__MPI_INTERCOMM_CREATE                 180
/** EPIK region ID for MPI_Intercomm_merge */
#define EPK__MPI_INTERCOMM_MERGE                  181
/** EPIK region ID for MPI_Iprobe */
#define EPK__MPI_IPROBE                           182
/** EPIK region ID for MPI_Irecv */
#define EPK__MPI_IRECV                            183
/** EPIK region ID for MPI_Irsend */
#define EPK__MPI_IRSEND                           184
/** EPIK region ID for MPI_Is_thread_main */
#define EPK__MPI_IS_THREAD_MAIN                   185
/** EPIK region ID for MPI_Isend */
#define EPK__MPI_ISEND                            186
/** EPIK region ID for MPI_Issend */
#define EPK__MPI_ISSEND                           187
/** EPIK region ID for MPI_Keyval_create */
#define EPK__MPI_KEYVAL_CREATE                    188
/** EPIK region ID for MPI_Keyval_free */
#define EPK__MPI_KEYVAL_FREE                      189
/** EPIK region ID for MPI_Lookup_name */
#define EPK__MPI_LOOKUP_NAME                      190
/** EPIK region ID for MPI_Op_c2f */
#define EPK__MPI_OP_C2F                           191
/** EPIK region ID for MPI_Op_commutative */
#define EPK__MPI_OP_COMMUTATIVE                   192
/** EPIK region ID for MPI_Op_create */
#define EPK__MPI_OP_CREATE                        193
/** EPIK region ID for MPI_Op_f2c */
#define EPK__MPI_OP_F2C                           194
/** EPIK region ID for MPI_Op_free */
#define EPK__MPI_OP_FREE                          195
/** EPIK region ID for MPI_Open_port */
#define EPK__MPI_OPEN_PORT                        196
/** EPIK region ID for MPI_Pack */
#define EPK__MPI_PACK                             197
/** EPIK region ID for MPI_Pack_external */
#define EPK__MPI_PACK_EXTERNAL                    198
/** EPIK region ID for MPI_Pack_external_size */
#define EPK__MPI_PACK_EXTERNAL_SIZE               199
/** EPIK region ID for MPI_Pack_size */
#define EPK__MPI_PACK_SIZE                        200
/** EPIK region ID for MPI_Pcontrol */
#define EPK__MPI_PCONTROL                         201
/** EPIK region ID for MPI_Probe */
#define EPK__MPI_PROBE                            202
/** EPIK region ID for MPI_Publish_name */
#define EPK__MPI_PUBLISH_NAME                     203
/** EPIK region ID for MPI_Put */
#define EPK__MPI_PUT                              204
/** EPIK region ID for MPI_Query_thread */
#define EPK__MPI_QUERY_THREAD                     205
/** EPIK region ID for MPI_Recv */
#define EPK__MPI_RECV                             206
/** EPIK region ID for MPI_Recv_init */
#define EPK__MPI_RECV_INIT                        207
/** EPIK region ID for MPI_Reduce */
#define EPK__MPI_REDUCE                           208
/** EPIK region ID for MPI_Reduce_local */
#define EPK__MPI_REDUCE_LOCAL                     209
/** EPIK region ID for MPI_Reduce_scatter */
#define EPK__MPI_REDUCE_SCATTER                   210
/** EPIK region ID for MPI_Reduce_scatter_block */
#define EPK__MPI_REDUCE_SCATTER_BLOCK             211
/** EPIK region ID for MPI_Register_datarep */
#define EPK__MPI_REGISTER_DATAREP                 212
/** EPIK region ID for MPI_Request_c2f */
#define EPK__MPI_REQUEST_C2F                      213
/** EPIK region ID for MPI_Request_f2c */
#define EPK__MPI_REQUEST_F2C                      214
/** EPIK region ID for MPI_Request_free */
#define EPK__MPI_REQUEST_FREE                     215
/** EPIK region ID for MPI_Request_get_status */
#define EPK__MPI_REQUEST_GET_STATUS               216
/** EPIK region ID for MPI_Rsend */
#define EPK__MPI_RSEND                            217
/** EPIK region ID for MPI_Rsend_init */
#define EPK__MPI_RSEND_INIT                       218
/** EPIK region ID for MPI_Scan */
#define EPK__MPI_SCAN                             219
/** EPIK region ID for MPI_Scatter */
#define EPK__MPI_SCATTER                          220
/** EPIK region ID for MPI_Scatterv */
#define EPK__MPI_SCATTERV                         221
/** EPIK region ID for MPI_Send */
#define EPK__MPI_SEND                             222
/** EPIK region ID for MPI_Send_init */
#define EPK__MPI_SEND_INIT                        223
/** EPIK region ID for MPI_Sendrecv */
#define EPK__MPI_SENDRECV                         224
/** EPIK region ID for MPI_Sendrecv_replace */
#define EPK__MPI_SENDRECV_REPLACE                 225
/** EPIK region ID for MPI_Sizeof */
#define EPK__MPI_SIZEOF                           226
/** EPIK region ID for MPI_Ssend */
#define EPK__MPI_SSEND                            227
/** EPIK region ID for MPI_Ssend_init */
#define EPK__MPI_SSEND_INIT                       228
/** EPIK region ID for MPI_Start */
#define EPK__MPI_START                            229
/** EPIK region ID for MPI_Startall */
#define EPK__MPI_STARTALL                         230
/** EPIK region ID for MPI_Status_c2f */
#define EPK__MPI_STATUS_C2F                       231
/** EPIK region ID for MPI_Status_f2c */
#define EPK__MPI_STATUS_F2C                       232
/** EPIK region ID for MPI_Status_set_cancelled */
#define EPK__MPI_STATUS_SET_CANCELLED             233
/** EPIK region ID for MPI_Status_set_elements */
#define EPK__MPI_STATUS_SET_ELEMENTS              234
/** EPIK region ID for MPI_Test */
#define EPK__MPI_TEST                             235
/** EPIK region ID for MPI_Test_cancelled */
#define EPK__MPI_TEST_CANCELLED                   236
/** EPIK region ID for MPI_Testall */
#define EPK__MPI_TESTALL                          237
/** EPIK region ID for MPI_Testany */
#define EPK__MPI_TESTANY                          238
/** EPIK region ID for MPI_Testsome */
#define EPK__MPI_TESTSOME                         239
/** EPIK region ID for MPI_Topo_test */
#define EPK__MPI_TOPO_TEST                        240
/** EPIK region ID for MPI_Type_c2f */
#define EPK__MPI_TYPE_C2F                         241
/** EPIK region ID for MPI_Type_commit */
#define EPK__MPI_TYPE_COMMIT                      242
/** EPIK region ID for MPI_Type_contiguous */
#define EPK__MPI_TYPE_CONTIGUOUS                  243
/** EPIK region ID for MPI_Type_create_darray */
#define EPK__MPI_TYPE_CREATE_DARRAY               244
/** EPIK region ID for MPI_Type_create_f90_complex */
#define EPK__MPI_TYPE_CREATE_F90_COMPLEX          245
/** EPIK region ID for MPI_Type_create_f90_integer */
#define EPK__MPI_TYPE_CREATE_F90_INTEGER          246
/** EPIK region ID for MPI_Type_create_f90_real */
#define EPK__MPI_TYPE_CREATE_F90_REAL             247
/** EPIK region ID for MPI_Type_create_hindexed */
#define EPK__MPI_TYPE_CREATE_HINDEXED             248
/** EPIK region ID for MPI_Type_create_hvector */
#define EPK__MPI_TYPE_CREATE_HVECTOR              249
/** EPIK region ID for MPI_Type_create_indexed_block */
#define EPK__MPI_TYPE_CREATE_INDEXED_BLOCK        250
/** EPIK region ID for MPI_Type_create_keyval */
#define EPK__MPI_TYPE_CREATE_KEYVAL               251
/** EPIK region ID for MPI_Type_create_resized */
#define EPK__MPI_TYPE_CREATE_RESIZED              252
/** EPIK region ID for MPI_Type_create_struct */
#define EPK__MPI_TYPE_CREATE_STRUCT               253
/** EPIK region ID for MPI_Type_create_subarray */
#define EPK__MPI_TYPE_CREATE_SUBARRAY             254
/** EPIK region ID for MPI_Type_delete_attr */
#define EPK__MPI_TYPE_DELETE_ATTR                 255
/** EPIK region ID for MPI_Type_dup */
#define EPK__MPI_TYPE_DUP                         256
/** EPIK region ID for MPI_Type_extent */
#define EPK__MPI_TYPE_EXTENT                      257
/** EPIK region ID for MPI_Type_f2c */
#define EPK__MPI_TYPE_F2C                         258
/** EPIK region ID for MPI_Type_free */
#define EPK__MPI_TYPE_FREE                        259
/** EPIK region ID for MPI_Type_free_keyval */
#define EPK__MPI_TYPE_FREE_KEYVAL                 260
/** EPIK region ID for MPI_Type_get_attr */
#define EPK__MPI_TYPE_GET_ATTR                    261
/** EPIK region ID for MPI_Type_get_contents */
#define EPK__MPI_TYPE_GET_CONTENTS                262
/** EPIK region ID for MPI_Type_get_envelope */
#define EPK__MPI_TYPE_GET_ENVELOPE                263
/** EPIK region ID for MPI_Type_get_extent */
#define EPK__MPI_TYPE_GET_EXTENT                  264
/** EPIK region ID for MPI_Type_get_name */
#define EPK__MPI_TYPE_GET_NAME                    265
/** EPIK region ID for MPI_Type_get_true_extent */
#define EPK__MPI_TYPE_GET_TRUE_EXTENT             266
/** EPIK region ID for MPI_Type_hindexed */
#define EPK__MPI_TYPE_HINDEXED                    267
/** EPIK region ID for MPI_Type_hvector */
#define EPK__MPI_TYPE_HVECTOR                     268
/** EPIK region ID for MPI_Type_indexed */
#define EPK__MPI_TYPE_INDEXED                     269
/** EPIK region ID for MPI_Type_lb */
#define EPK__MPI_TYPE_LB                          270
/** EPIK region ID for MPI_Type_match_size */
#define EPK__MPI_TYPE_MATCH_SIZE                  271
/** EPIK region ID for MPI_Type_set_attr */
#define EPK__MPI_TYPE_SET_ATTR                    272
/** EPIK region ID for MPI_Type_set_name */
#define EPK__MPI_TYPE_SET_NAME                    273
/** EPIK region ID for MPI_Type_size */
#define EPK__MPI_TYPE_SIZE                        274
/** EPIK region ID for MPI_Type_struct */
#define EPK__MPI_TYPE_STRUCT                      275
/** EPIK region ID for MPI_Type_ub */
#define EPK__MPI_TYPE_UB                          276
/** EPIK region ID for MPI_Type_vector */
#define EPK__MPI_TYPE_VECTOR                      277
/** EPIK region ID for MPI_Unpack */
#define EPK__MPI_UNPACK                           278
/** EPIK region ID for MPI_Unpack_external */
#define EPK__MPI_UNPACK_EXTERNAL                  279
/** EPIK region ID for MPI_Unpublish_name */
#define EPK__MPI_UNPUBLISH_NAME                   280
/** EPIK region ID for MPI_Wait */
#define EPK__MPI_WAIT                             281
/** EPIK region ID for MPI_Waitall */
#define EPK__MPI_WAITALL                          282
/** EPIK region ID for MPI_Waitany */
#define EPK__MPI_WAITANY                          283
/** EPIK region ID for MPI_Waitsome */
#define EPK__MPI_WAITSOME                         284
/** EPIK region ID for MPI_Win_c2f */
#define EPK__MPI_WIN_C2F                          285
/** EPIK region ID for MPI_Win_call_errhandler */
#define EPK__MPI_WIN_CALL_ERRHANDLER              286
/** EPIK region ID for MPI_Win_complete */
#define EPK__MPI_WIN_COMPLETE                     287
/** EPIK region ID for MPI_Win_create */
#define EPK__MPI_WIN_CREATE                       288
/** EPIK region ID for MPI_Win_create_errhandler */
#define EPK__MPI_WIN_CREATE_ERRHANDLER            289
/** EPIK region ID for MPI_Win_create_keyval */
#define EPK__MPI_WIN_CREATE_KEYVAL                290
/** EPIK region ID for MPI_Win_delete_attr */
#define EPK__MPI_WIN_DELETE_ATTR                  291
/** EPIK region ID for MPI_Win_f2c */
#define EPK__MPI_WIN_F2C                          292
/** EPIK region ID for MPI_Win_fence */
#define EPK__MPI_WIN_FENCE                        293
/** EPIK region ID for MPI_Win_free */
#define EPK__MPI_WIN_FREE                         294
/** EPIK region ID for MPI_Win_free_keyval */
#define EPK__MPI_WIN_FREE_KEYVAL                  295
/** EPIK region ID for MPI_Win_get_attr */
#define EPK__MPI_WIN_GET_ATTR                     296
/** EPIK region ID for MPI_Win_get_errhandler */
#define EPK__MPI_WIN_GET_ERRHANDLER               297
/** EPIK region ID for MPI_Win_get_group */
#define EPK__MPI_WIN_GET_GROUP                    298
/** EPIK region ID for MPI_Win_get_name */
#define EPK__MPI_WIN_GET_NAME                     299
/** EPIK region ID for MPI_Win_lock */
#define EPK__MPI_WIN_LOCK                         300
/** EPIK region ID for MPI_Win_post */
#define EPK__MPI_WIN_POST                         301
/** EPIK region ID for MPI_Win_set_attr */
#define EPK__MPI_WIN_SET_ATTR                     302
/** EPIK region ID for MPI_Win_set_errhandler */
#define EPK__MPI_WIN_SET_ERRHANDLER               303
/** EPIK region ID for MPI_Win_set_name */
#define EPK__MPI_WIN_SET_NAME                     304
/** EPIK region ID for MPI_Win_start */
#define EPK__MPI_WIN_START                        305
/** EPIK region ID for MPI_Win_test */
#define EPK__MPI_WIN_TEST                         306
/** EPIK region ID for MPI_Win_unlock */
#define EPK__MPI_WIN_UNLOCK                       307
/** EPIK region ID for MPI_Win_wait */
#define EPK__MPI_WIN_WAIT                         308
/** EPIK region ID for MPI_Wtick */
#define EPK__MPI_WTICK                            309
/** EPIK region ID for MPI_Wtime */
#define EPK__MPI_WTIME                            310

/** Number of MPI functions defined */
#define EPK__MPI_NUMFUNCS                         312

/** Artificial root for MPI-only experiments when no user-code
 * instrumenation is available */
#define EPK_PARALLEL__MPI           EPK__MPI_NUMFUNCS

/** Region IDs of MPI functions */
EXTERN int epk_mpi_regid[EPK__MPI_NUMFUNCS + 1];

/**
 * Register MPI functions and initialize data structures
 */
EXTERN void epk_mpi_register();

EXTERN elg_ui1 epk_is_mpi_collective(const char* str);
EXTERN elg_ui1 epk_is_mpi_point2point(const char* str);
EXTERN elg_ui1 epk_mpi_eventtype(const char* str);

/**
 * Enable measurement for specific subgroups of MPI functions
 */
EXTERN void epk_mpi_enable_init();

#endif
