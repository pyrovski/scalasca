/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_USER_EVENTS_H
#define SCOUT_USER_EVENTS_H


/*-------------------------------------------------------------------------*/
/**
 * @file  user_events.h
 * @brief Declaration of constants for user-defined events.
 *
 * This header file provides a set of numerical constants that provide
 * symbolic names for various user events that can be triggered during
 * the pattern search replay.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Constants -------------------------------------------------------------

const int INIT                   =   1;
const int FINALIZE               =   2;
const int FINISHED               =   3;
const int PENDING                =   4;
const int PREPARE                =   5;

const int PRE_SEND               =  10;
const int POST_SEND              =  11;
const int PRE_RECV               =  12;
const int POST_RECV              =  13;
const int COLL_12N               =  14;
const int COLL_N21               =  15;
const int COLL_N2N               =  16;
const int COLL_SCAN              =  17;
const int SYNC_COLL              =  18;
const int PROFILE                =  19;
const int LATE_SENDER            =  20;
const int LATE_SENDER_WO         =  21;
const int LATE_RECEIVER          =  22;
const int WAIT_NXN               =  23;
const int WAIT_BARRIER           =  24;
const int EARLY_REDUCE           =  25;
const int EARLY_SCAN             =  26;
const int LATE_BCAST             =  27;
const int NXN_COMPL              =  28;
const int BARRIER_COMPL          =  29;

const int GPRE_SEND              =  50;
const int GPOST_SEND             =  51;
const int GPRE_RECV              =  52;
const int GPOST_RECV             =  53;
const int GCOLL_12N              =  54;
const int GCOLL_N21              =  55;
const int GCOLL_N2N              =  56;
const int GCOLL_BARRIER          =  57;
const int GLATE_SENDER           =  58;
const int GWAIT_NXN              =  59;

const int NGPRE_SEND             =  60;
const int NGPOST_SEND            =  61;
const int NGPRE_RECV             =  62;
const int NGPOST_RECV            =  63;
const int NGCOLL_12N             =  64;
const int NGCOLL_N21             =  65;
const int NGCOLL_N2N             =  66;
const int NGCOLL_BARRIER         =  67;
const int NGLATE_SENDER          =  68;
const int NGWAIT_NXN             =  69;

const int MPI_RMA_POST_WAIT          = 70;
const int MPI_RMA_POST_COMPLETE      = 71;
const int MPI_RMA_EARLY_TRANSFER     = 72;
const int MPI_RMA_PUT_LATEST_OP      = 73;
const int MPI_RMA_WAIT_AT_FENCE      = 74;
const int MPI_RMA_EARLY_FENCE        = 75;
const int MPI_RMA_PWS_COUNT          = 76;
const int MPI_RMA_PWS_UNNEEDED_COUNT = 77;
const int MPI_RMA_SET_REMOTE_BIT     = 78;

const int OMP_MGMT_FORK          =  90;
const int OMP_MGMT_JOIN          =  91;
const int OMP_EBARRIER           =  92;
const int OMP_IBARRIER           =  93;
const int OMP_EBARRIER_WAIT      =  94;
const int OMP_IBARRIER_WAIT      =  95;

const int CCV_P2P                = 100;
const int CCV_COLL               = 101;

}   // namespace scout


#endif   // !SCOUT_USER_EVENTS_H
