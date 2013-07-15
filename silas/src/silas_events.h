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
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SILAS_EVENTS_H
#define SILAS_EVENTS_H

namespace silas
{
    const int START                 =   0;
    const int FINISHED              =   1;
    const int ANY                   =   2;

    const int AGGREGATE_IGNORE      =   3;

    const int SKIP                  =   4;
    const int DELETE                =   5;
    const int IGNORE                =   6;
    const int SKIP_END              =   7;

    const int INIT                  =   8;
    const int BEGIN                 =   9;
    const int NOOP                  =   10;
    const int END                   =   11;
    const int IDLE                  =   12;
    const int SEND                  =   13;
    const int SSEND                 =   14;
    const int BSEND                 =   15;
    const int RECV                  =   16;
    const int SENDRECV              =   17;
    const int BARRIER               =   18;
    const int BCAST                 =   19;
    const int ALLREDUCE             =   20;
    const int ALLGATHER             =   21;
    const int SCAN                  =   22;
    const int EXSCAN                =   23;
    const int REDUCE                =   24;
    const int RECALC                =   25;
   
    const int ATOMIC_REGION_EXIT    =   26;
    const int SEND_EXIT             =   27;
    const int RECV_EXIT             =   28;
    const int BALANCE_EXIT          =   29;
    const int SCALE_EXIT            =   30;

    const int IRECV                 =   31;
    const int WAIT                  =   32;
    const int WAITANY               =   33;
    const int WAITSOME              =   34;
    const int WAITALL               =   35;
    const int TEST                  =   36;
    const int TESTANY               =   37;
    const int TESTSOME              =   38;
    const int TESTALL               =   39;
    const int ISEND                 =   40;
    const int ISSEND                =   41;
    const int IBSEND                =   42;
    const int MPISTART              =   43;
    const int MPISTARTALL           =   44;

    const int IDLE_WRITE            =   45;
    const int ALLTOALL              =   46;

    const int NUM_ACTION_TYPES      =   47;
}
#endif

