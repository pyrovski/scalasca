/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_PATTERNS_GEN_H
#define SCOUT_PATTERNS_GEN_H


namespace scout
{

//--- Constants -------------------------------------------------------------

const long PAT_NONE                           =  -1;
const long PAT_TIME                           =   0;
const long PAT_VISITS                         =   1;
const long PAT_EXECUTION                      =   2;
const long PAT_OVERHEAD                       =   3;
const long PAT_MPI                            =   4;
const long PAT_MPI_SYNCHRONIZATION            =   5;
const long PAT_MPI_COMMUNICATION              =   6;
const long PAT_MPI_IO                         =   7;
const long PAT_MPI_IO_COLLECTIVE              =   8;
const long PAT_MPI_INIT_EXIT                  =   9;
const long PAT_MPI_SYNC_COLLECTIVE            =  10;
const long PAT_MPI_BARRIER_WAIT               =  11;
const long PAT_MPI_BARRIER_COMPLETION         =  12;
const long PAT_MPI_POINT2POINT                =  13;
const long PAT_MPI_LATESENDER                 =  14;
const long PAT_MPI_LATESENDER_WO              =  15;
const long PAT_MPI_LSWO_DIFFERENT             =  16;
const long PAT_MPI_LSWO_SAME                  =  17;
const long PAT_MPI_LATERECEIVER               =  18;
const long PAT_MPI_COLLECTIVE                 =  19;
const long PAT_MPI_EARLYREDUCE                =  20;
const long PAT_MPI_EARLYSCAN                  =  21;
const long PAT_MPI_LATEBROADCAST              =  22;
const long PAT_MPI_WAIT_NXN                   =  23;
const long PAT_MPI_NXN_COMPLETION             =  24;
const long PAT_OMP_IDLE_THREADS               =  25;
const long PAT_OMP_LIMITED_PARALLELISM        =  26;
const long PAT_OMP_TIME                       =  27;
const long PAT_OMP_FLUSH                      =  28;
const long PAT_OMP_MANAGEMENT                 =  29;
const long PAT_OMP_FORK                       =  30;
const long PAT_OMP_SYNCHRONIZATION            =  31;
const long PAT_OMP_BARRIER                    =  32;
const long PAT_OMP_EBARRIER                   =  33;
const long PAT_OMP_EBARRIER_WAIT              =  34;
const long PAT_OMP_IBARRIER                   =  35;
const long PAT_OMP_IBARRIER_WAIT              =  36;
const long PAT_OMP_CRITICAL                   =  37;
const long PAT_OMP_LOCK_API                   =  38;
const long PAT_OMP_ORDERED                    =  39;
const long PAT_SYNCS                          =  40;
const long PAT_SYNCS_P2P                      =  41;
const long PAT_SYNCS_SEND                     =  42;
const long PAT_SYNCS_RECV                     =  43;
const long PAT_SYNCS_COLL                     =  44;
const long PAT_COMMS                          =  45;
const long PAT_COMMS_P2P                      =  46;
const long PAT_COMMS_SEND                     =  47;
const long PAT_COMMS_RECV                     =  48;
const long PAT_COMMS_COLL                     =  49;
const long PAT_COMMS_CXCH                     =  50;
const long PAT_COMMS_CSRC                     =  51;
const long PAT_COMMS_CDST                     =  52;
const long PAT_BYTES                          =  53;
const long PAT_BYTES_P2P                      =  54;
const long PAT_BYTES_SENT                     =  55;
const long PAT_BYTES_RCVD                     =  56;
const long PAT_BYTES_COLL                     =  57;
const long PAT_BYTES_COUT                     =  58;
const long PAT_BYTES_CIN                      =  59;
const long PAT_BYTES_RMA                      =  60;
const long PAT_BYTES_GET                      =  61;
const long PAT_BYTES_PUT                      =  62;
const long PAT_MPI_CLS_COUNT                  =  63;
const long PAT_MPI_CLSWO_COUNT                =  64;
const long PAT_MPI_CLR_COUNT                  =  65;
const long PAT_MPI_SLS_COUNT                  =  66;
const long PAT_MPI_SLSWO_COUNT                =  67;
const long PAT_MPI_SLR_COUNT                  =  68;
const long PAT_MPI_FILE_OPS                   =  69;
const long PAT_MPI_FILE_IOPS                  =  70;
const long PAT_MPI_FILE_IROPS                 =  71;
const long PAT_MPI_FILE_IWOPS                 =  72;
const long PAT_MPI_FILE_COPS                  =  73;
const long PAT_MPI_FILE_CROPS                 =  74;
const long PAT_MPI_FILE_CWOPS                 =  75;
const long PAT_MPI_FILE_BYTES                 =  76;
const long PAT_MPI_FILE_IBYTES                =  77;
const long PAT_MPI_FILE_IRBYTES               =  78;
const long PAT_MPI_FILE_IWBYTES               =  79;
const long PAT_MPI_FILE_CBYTES                =  80;
const long PAT_MPI_FILE_CRBYTES               =  81;
const long PAT_MPI_FILE_CWBYTES               =  82;
const long PAT_MPI_RMA_SYNCHRONIZATION        =  83;
const long PAT_MPI_RMA_COMMUNICATION          =  84;
const long PAT_MPI_RMA_LATE_POST              =  85;
const long PAT_MPI_RMA_EARLY_WAIT             =  86;
const long PAT_MPI_RMA_LATE_COMPLETE          =  87;
const long PAT_MPI_RMA_EARLY_TRANSFER         =  88;
const long PAT_MPI_RMA_WAIT_AT_FENCE          =  89;
const long PAT_MPI_RMA_EARLY_FENCE            =  90;
const long PAT_MPI_RMA_PAIRSYNC_COUNT         =  91;
const long PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT =  92;
const long PAT_IMBALANCE                      =  93;
const long PAT_IMBALANCE_ABOVE                =  94;
const long PAT_IMBALANCE_ABOVE_SINGLE         =  95;
const long PAT_IMBALANCE_BELOW                =  96;
const long PAT_IMBALANCE_BELOW_BYPASS         =  97;
const long PAT_IMBALANCE_BELOW_SINGULARITY    =  98;
const long PAT_STATISTICS                     =  99;


//--- Forward declarations --------------------------------------------------

class PatternDetectionTask;


//--- Function prototypes ---------------------------------------------------

void create_patterns(PatternDetectionTask* analyzer);


}   /* namespace scout */


#endif   /* !SCOUT_PATTERNS_GEN_H */
