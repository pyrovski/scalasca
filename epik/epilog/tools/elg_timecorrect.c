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

/* elg_timecorrect.c */

/**
 * correct events time --  
 *  timestamp correction with the controlled logical clock.
 * (C) 1995 ZAM/ KFA Juelich (Trace file handling)
 * (C) 1997 Rolf Rabenseifner RUS Stuttgart (Timestamp Correction)
 * (C) 2005 ZAM/ FZ Juelich (Epilog trace file format)
 */

/* --- standard includes for tracefile handling : */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

/* --- epilog includes  */
#include "elg_rw.h"
#include "elg_error.h"
#include "epk_archive.h"

/* --- error handling */
void MemError()
{
    fprintf(stderr, "\nelg_timecorrect: out of memory\n");
    exit(3);
}

void print_usage()
{
    fprintf(stderr,
            "calling convention:\n"
            "   elg_timecorrect [options] (<input.elg> | <experiment_archive>)"
            " <output.elg>\n\n"
            "try 'elg_timecorrect -h' for more information\n\n");
}

void print_help()
{
    print_usage();

    fprintf(stderr,
            "options:\n"
            "   -c   <float>   : expected maximal clock difference, value>=0. Default: 1000 [usec]\n"
            "   -d   <float>   : minimal local timestamp difference, value>0. Default: 0.1 [usec]\n"
            "   -e   <float>   : maximal error rate. if the value is > 0 than the amortisation should\n"
            "                    reduce the error to this maximal error rate;\n"
            "                    if value=0 then no amortisation is done. Default: 0.5 %%\n"
            "   -f   <float>   : factor between 0 and 1, default=(0.8), to multiply each\n"
            "                    mmd and immd value read from the options file given with option -r\n"
            "   -i   <from>_<to>_<float> : individual minimal message delay, >0. [usec]\n"
            "                    <from> and <to> are node numbers, 0..#nodes-1\n"
            "   -m   <float>   : minimal message delay, value>0. Default: 5 [usec]\n"
            "   -R   <filename>: read -c, -m, -i, -d options from options file\n"
            "                    before starting the timestamp corrections, they overwrite\n"
            "                    previous command line options.\n"
            "                    Options file kann be generated with -w or -W option\n"
            "   -r               same as -R but uses default options file name 'timecorrect.opt'\n" 
            "   -v             : output trace statistics\n"
            "   -W   <filename>: write analyzed 'clock difference', 'minimal message delay',\n"
            "                    'individual minimal message delay', 'minimal local timestamp difference'\n"
            "                    to options file after all timestamp corrections.\n"
            "                    Use created options file with -r or -R option\n"
            "   -w               same as -W but uses default options file name 'timecorrect.opt'" 
            );

    fprintf(stderr,
            "Examples:\n"
            "   Correct timestamps using default options:\n"
            "      elg_timecorrect bad_trace.elg good_trace.elg\n"
            "\n"
            "   Create  options file 'timecorrect.opt':\n"
            "     elg_timecorrect -w ping_pong.elg dummy.elg\n"
            "   Correct timestamps using options file 'timecorrect.opt':\n"
            "     elg_timecorrect -r bad_trace.elg good_trace.elg\n");
    exit(4);
}


/*--------------------------------------------------------------------*/
/* --- declarations for correct time */


/* This code includes                                         */
/* - the algorithmus for a logical clock with gamma_i^j = 0,  */
/* - the algorithmus for a logical clock with gamma_i^j       */
/*   controled by a controller                                */
/* - the controller                                           */
/* - the amortization                                         */

# define MAX_ANALYSE_POINTS 20
# define PRECISION_OF_DOUBLE 1e-14
# define MAX_DOUBLE 9.9999e30
# define max(A,B) ((A)>(B) ?   (A)  : (B))
# define min(A,B) ((A)<(B) ?   (A)  : (B))
# define Abs(A)   ((A)< 0  ?  (-A)  : (A))
# define sqr(A)   ((A)*(A))
# define pow16(dbl) sqr(sqr(sqr(sqr(dbl))))
/* # define pow16(dbl)  pow(dbl,16.0) */
# define root16(dbl) pow(dbl,0.0625)

typedef elg_ui1 Booli;          /* using epilog datatypes */
typedef elg_ui4 procnum;        /* using epilog datatypes */
typedef elg_d8 timestamp;       /* using epilog datatypes */
typedef elg_ui4 msgid1type;     /* using epilog datatypes */
typedef elg_ui4 msgid2type;     /* using epilog datatypes */
# define MAX_TIMESTAMP MAX_DOUBLE       /* for vampir datatypes */

/* event type -- unordered set */
typedef enum
{ EV_receive, EV_send, EV_internal } EVtype;
typedef enum
{ Delta_LCa_C, Delta_LCa_LC } qij_type;
typedef enum
{ p_alg_max, p_alg_avg, p_alg_sqr, p_alg_p16 } p_alg_type;
typedef enum
{ p_crv_spl, p_crv_lin, p_crv_pbl, p_crv_cub } p_crv_type;

typedef struct _event_entry
{
    EVtype event_type;
    procnum msg_from;           /* if event_type == EV_receive  */
    timestamp LC_from;          /* if event_type == EV_receive  */
    timestamp LCa_from;         /* if event_type == EV_receive  */
    procnum msg_to;             /* if event_type == EV_send     */
    msgid1type msg_id1;         /* if event_type != EV_internal */
    msgid2type msg_id2;         /* if event_type != EV_internal */
    timestamp Cj;               /* C_i^j (t(e_i^j))   */
    timestamp LCj;              /* LC_i^j  (e_i^j)    */
    timestamp LCaj;             /* LC'_i^j (e_i^j)    */
    struct _event_entry *next;  /* next newer in the input queue */
    /* next newer in the amortisation list */
    /* next (unsorted) in output groups */
    Booli not_the_first_event;
    timestamp discrete_advance; /* if event_type == EV_receive */
    struct _event_entry *corresponding_send;    /*if recv.event processable */
    timestamp corresponding_recv_LCa;   /* if send event, after corresp.
                                           receive event is processed */
    Booli corresponding_recv_LCa_known;
    struct _event_entry *next_newer_recv;       /* in the amortisation list */
    struct _event_entry *next_newer_send;       /* in the amortisation list */
    double amortisation;        /* amortisation at this ... */
    struct _event_entry *next_AM_point; /* next ...amortisation point */
    ElgRec *record;
    timestamp C_from;
    procnum i;
}
event_entry;

typedef struct _csel_entry
{                               /* corresponding send event list entry */
    timestamp LC;
    timestamp LCa;
    procnum msg_from;
    msgid1type msg_id1;
    msgid2type msg_id2;
    struct _csel_entry *next;
    event_entry *send_event;
    /* this pointer stays valid until the corresponding receive
       was computed, because only send events with known
       corresponding receive's LC' can move from AM_postponed to
       AM_allowed which is the precondition to go to the output
       which will free this send_event pointer */
    timestamp C;
}
csel_entry;

typedef struct _merge_node
{
    struct _event_entry *event;
    struct _merge_node *left;
    struct _merge_node *right;
    Booli final;
}
merge_node;

typedef struct
{
    event_entry *In_oldest;     /* input queue -- oldest pointer */
    event_entry *In_newest;     /* input queue -- newest pointer */
    csel_entry *csel_first;     /* sorted corresponding send ... */
    csel_entry *csel_last;      /*                ... event list */
    procnum input_processable_next;     /* input scheduler : */
    /* list of processes with possibly
       executable events in In_oldest */
    Booli input_processable;
    Booli initialized;          /* if first event processed? */
    timestamp Cj1;              /* C_i (t(e_i^j-1)) */
    timestamp LCj1;             /* LC_i  (e_i^j-1)  */
    timestamp LCaj1;            /* LC'_i (e_i^j-1)  */
    float gammaj;               /* gamma_i^j */
    timestamp delta;            /* <minimal local timestamp difference */
    timestamp LCaj_Cj;          /* LC'_i(e_i^j) - C_i(t(e_i^j)) */
    timestamp LCaj1_Cj1;        /* LC'_i(e_i^j-1) - C_i(t(e_i^j-1)) */
    double pow16_LCaj1_Cj1;     /* (LC'-C)**16  */
    event_entry *AM_oldest;
    event_entry *AM_oldest_send;
    event_entry *AM_postponed_send;
    timestamp AM_LCa_limit;
    timestamp AM_LCa_next_before_oldest;
    event_entry *AM_todo_recv;
    event_entry *AM_newest_recv;
    event_entry *AM_newest_send;
    event_entry *AM_newest;
    procnum AM_processable_next;        /* amortisation list scheduler */
    Booli AM_processable;
    struct _merge_node *output_merge_node;
    struct _event_entry *output_merge_newest_event;
    /* additional data for other analysis of proc. i */
    timestamp Out_Cj1;          /* measured at output from event_entry */
    timestamp Out_LCaj1;
}
process_data;

typedef struct _immd_entry
{                               /* individual minimal message delay defaults list entry */
    struct _immd_entry *next;
    procnum from, to;
    timestamp immd;
}
immd_entry;

/*--------------------------------------------------------------------*/

/* global data for command line options */
static double clock_period;
static timestamp delta_default, mmd_default, *mmd;
static int mmd_individual;      /* 0=same mmd for all k,i; 1=individual mmds */
static immd_entry *immd_first, *immd_last;
static float mmd_factor;        /* default is 0.8 */
static char *to_optfilename, *from_optfilename;
static elg_ui1 verbose = ELG_FALSE;
static float qmin, qmax, pmin, pmax, D_slow_factor, gamma1_init;
static timestamp min_LCa_C;
static double sum16_LCa_C;
static double max_sum16_LCa_C;
static timestamp AM_cldiff_default;
static double AM_maxerr;


/* support for single location */
static elg_ui4 single_lid;

void ElgRec_read_MPI_SEND(ElgRec* inRec, elg_ui4* lid, elg_d8* time,
                          elg_ui4* dlid, elg_ui4* cid, elg_ui4* tag,
                          elg_ui4* sent)
{
    elg_ui1 length = ElgRec_get_length(inRec);
    ElgRec_seek(inRec, 0);
    if (length == sizeof(*lid) + sizeof(*time) + sizeof(*dlid)
                + sizeof(*cid) + sizeof(*tag) + sizeof(*sent))
        *lid = ElgRec_read_ui4(inRec);
    else
        *lid = single_lid;

    *time = ElgRec_read_d8(inRec);
    *dlid = ElgRec_read_ui4(inRec);
    *cid = ElgRec_read_ui4(inRec);
    *tag = ElgRec_read_ui4(inRec);
    *sent = ElgRec_read_ui4(inRec);
}

void ElgRec_read_MPI_RECV(ElgRec* inRec, elg_ui4* lid, elg_d8* time,
                          elg_ui4* slid, elg_ui4* cid, elg_ui4* tag)
{
    elg_ui1 length = ElgRec_get_length(inRec);
    ElgRec_seek(inRec, 0);
    if (length == sizeof(*lid) + sizeof(*time) + sizeof(*slid)
                + sizeof(*cid) + sizeof(*tag))
        *lid = ElgRec_read_ui4(inRec);
    else
        *lid = single_lid;

    *time = ElgRec_read_d8(inRec);
    *slid = ElgRec_read_ui4(inRec);
    *cid = ElgRec_read_ui4(inRec);
    *tag = ElgRec_read_ui4(inRec);
}


void default_opt()
{
    clock_period = 1.0;         /* = 1.0 sec, no option, only internal */
    delta_default = 0.1e-6 / clock_period;
    /* = 0.1 us, minimal difference between
       two events in the same process */
    mmd_default = 5e-6 / clock_period;
    /* = 5 us, minimal message delay */
    mmd_individual = 0;
    immd_first = NULL;
    immd_last = NULL;
    mmd = &mmd_default;
    mmd_factor = 0.8;           /* due to clock drifts the given
                                   mmd's from the last run on the
                                   from_optfile may be larger than the
                                   real mmd's and must be reduced
                                   therefore by an mmd_factor < 1 */
    to_optfilename = NULL;
    from_optfilename = NULL;
    qmin = 1.2;                 /* when LC'-C > qmin * max(LC-C)
                                   the controller starts its work, i.e.
                                   it reduces gamma.
                                   allowed: qmin >= 1
                                   advice:  1.2 (1.0 .. 2.0) */
    qmax = 3.0;                 /* when LC'-C >= qmax * max(LC-C)
                                   the controller reduces gamma to zero,
                                   i.e. the controlled logical clock
                                   runs like the simple logical clock.
                                   allowed: qmax > qmin
                                   advice:  2*(qmin+0.5) */
    pmin = 0.0;                 /* when min(LC'-C) > pmin * max(LC'-C)
                                   the controller starts its work, i.e.
                                   it reduces gamma.
                                   allowed: pmin >= 0
                                   advice:  0.0  */
    pmax = 1.0;                 /* when min(LC'-C) >= pmax * max(LC'-C)
                                   the controller reduces gamma to zero,
                                   i.e. the controlled logical clock
                                   runs like the simple logical clock.
                                   allowed: pmax > pmin
                                   advice:  1.0 (0.5..1.0) */
    D_slow_factor = 0.5;        /* computing max(LC-C) the controller
                                   forgets with D_slow_factor*gamma1_init
                                   the past. This prohibits that some time
                                   after a pertubation the controller
                                   starts to reduce gamma due to the
                                   rule based on qmin.
                                   0 means that a pertubation won't be
                                   forgotten in computing max(LC-C).
                                   allowed: 0 <= D_slow_factor < 1
                                   advice:  0.5 (0.5 .. 0.8) */
    gamma1_init = 2e-5;         /* a pertubation of delta will be for-
                                   gotten in delta/gamma1_init, e.g. with
                                   gamma1_init=1e-3, a clock error correc-
                                   tion of 1ms (i.e. LC-C = LC'-C = 1ms)
                                   will influence LC' for 1ms/1e-3 = 1sec;
                                   gamma1_init=1  implies LC' := LC.
                                   allowed: 0 <= gamma1_init <= 1
                                   advice:  2e-5 (0.05 .. 1e-4)   */
    AM_cldiff_default = 1000e-6 / clock_period;
    /* expected maximal clock diff. in  us */
    AM_maxerr = 0.005;          /* wished maximal error */
}



/*--------------------------------------------------------------------*/
/* Korrigieren der Zeitstempel in einer Datei: */
/* global between Init, several invocations of Exec, and Finish */
static procnum n;
static process_data *P;
static merge_node *merge_root, *node, *nextnode;
static event_entry final_event_struct;
static event_entry *final_event = &final_event_struct;
static timestamp Dmaxj, LC_Dmaxj;
static procnum index_min_LCa_C;

static float gamma_max;
static long In_length;

static timestamp AM_interval;   /* amortisation interval in cl.periods */
static timestamp AM_cldiff;     /* exp. max. clock diff. in cl.periods */
static procnum AM_out_filled;   /* number of fille output lists
                                   to be merged */

static double min_delta = MAX_DOUBLE;
static double *ki_mmd;
static long count_err_gt_maxerr = 0;
static double max_err_gt_maxerr = 0;
static double sum_err_gt_maxerr = 0;
static long count_err_le_maxerr = 0;
static double max_err_le_maxerr = 0;
static double sum_err_le_maxerr = 0;
static long count_err_eq_zero = 0;
static double min_err = MAX_DOUBLE;



void correct_time_init()
{
    n = 0;
    P = NULL;
    In_length = 0;
    gamma_max = 1.0 - gamma1_init;      /* fixed definition */
    Dmaxj = 0;
    index_min_LCa_C = 0;
    min_LCa_C = 0.0;
    sum16_LCa_C = 0;
    max_sum16_LCa_C = 0;
    AM_cldiff = 0;
    AM_interval = (AM_maxerr > 0 ? AM_cldiff_default / AM_maxerr : 0);
    final_event->LCaj = MAX_TIMESTAMP;
    final_event->next = final_event;
}


elg_ui4 handle_def_record(ElgRec* InRec, ElgOut* OutFile)
{
    elg_ui1 type;
    procnum i, k;
    merge_node *merge_node_array, *upper_lev_merge_node_array;
    int merge_node_array_lng, upper_lev_merge_node_array_lng;
    elg_ui4 lastdef = ELG_FALSE;

    type = ElgRec_get_type(InRec);
    switch (type)
    {
    case ELG_LOCATION:
        n++;
        break;
    case ELG_LAST_DEF:
        AM_out_filled = 0;
        elg_cntl_msg("num locations = %d", n);

        /*
         *    Convert minimal delay of messages from k to i, given  in parameter 
         *    list to adj. matrix. 
         *    Destroy input list.
         *    /param n           [in]         - number of procs
         *    /param immd_first  [in]         - pointer to first list element
         *    /param mmd         [out]        - n x n matrix of delays
         */
        if (immd_first != NULL)
        {
            immd_entry *immd;
            mmd_individual = 1;
            if ((mmd = malloc(n * n * sizeof(timestamp))) == NULL)
                MemError();
            for (k = 0; k < n; k++)
                for (i = 0; i < n; i++)
                {
                    mmd[k * n + i] = mmd_default;
                }

            /* =minimal delay of messages from k to i */
            while (immd_first != NULL)
            {
                immd = immd_first;
                mmd[immd->from * n + immd->to] = immd->immd;
                immd_first = immd->next;
                free(immd);
            }
        }

        /*
         *    Create and initialize \a P                 - array of  Process Data 
         *    Create and initialize \a merge_node_array  - array of numproc size
         *    Each process points to one merge_node.
         */
        if ((P = malloc(n * sizeof(process_data))) == NULL)
            MemError();
        merge_node_array_lng = n;
        if ((merge_node_array = malloc(merge_node_array_lng *
                                       sizeof(merge_node))) == NULL)
            MemError();
        for (i = 0; i < n; i++)
        {                       /* init... */
            P[i].In_oldest = NULL;
            P[i].csel_first = NULL;
            P[i].csel_last = NULL;
            P[i].input_processable_next = ELG_NO_ID;
            P[i].input_processable = ELG_FALSE;
            P[i].initialized = ELG_FALSE;
            P[i].gammaj = gamma_max;
            P[i].LCaj_Cj = 0;
            P[i].Cj1 = 0;       /* for initial step of p-algorithm */
            P[i].LCaj1 = 0;     /* for initial step of p-algorithm */
            P[i].LCaj1_Cj1 = 0; /* for initial step of p-algorithm */
            P[i].AM_oldest = NULL;
            P[i].AM_oldest_send = NULL;
            P[i].AM_postponed_send = NULL;
            P[i].AM_LCa_limit = 0;
            P[i].AM_todo_recv = NULL;
            P[i].AM_newest_recv = NULL;
            P[i].AM_newest_send = NULL;
            P[i].AM_newest = NULL;
            P[i].AM_processable_next = ELG_NO_ID;
            P[i].AM_processable = ELG_FALSE;
            P[i].output_merge_node = &(merge_node_array[i]);
            P[i].output_merge_node->final = ELG_TRUE;
            P[i].output_merge_node->event = NULL;
            P[i].output_merge_node->left = NULL;        /* unused because .. */
            P[i].output_merge_node->right = NULL;       /*  .... final node */
            P[i].output_merge_newest_event = NULL;
        }

        /*
         *   Create tree of \a merge nodes
         */
        while (merge_node_array_lng > 1)
        {
            upper_lev_merge_node_array = merge_node_array;
            upper_lev_merge_node_array_lng = merge_node_array_lng;
            merge_node_array_lng = (upper_lev_merge_node_array_lng + 1) / 2;
            if ((merge_node_array = malloc(merge_node_array_lng *
                                           sizeof(merge_node))) == NULL)
                MemError();
            for (i = 0; i < merge_node_array_lng; i++)
            {
                merge_node_array[i].final = ELG_FALSE;
                merge_node_array[i].event = NULL;
                merge_node_array[i].left =
                    &(upper_lev_merge_node_array[2 * i]);
                merge_node_array[i].right =
                    (2 * i + 1 <
                     upper_lev_merge_node_array_lng ?
                     &(upper_lev_merge_node_array[2 * i + 1]) : NULL);
                /* delete intermediate upper-level-nodes that have only
                   one link */
                if ((merge_node_array[i].left != NULL) &&
                    (!merge_node_array[i].left->final) &&
                    (merge_node_array[i].left->right == NULL))
                    merge_node_array[i].left = merge_node_array[i].left->left;
                if ((merge_node_array[i].right != NULL)
                    && (!merge_node_array[i].right->final)
                    && (merge_node_array[i].right->right == NULL))
                    /* !!!right->right  changed to right->left by Andrej Kuehnal */
                    merge_node_array[i].right =
                        merge_node_array[i].right->left;
            }
        }

        /*
         *   Init  ki_mmd matrix ( n x n ) with MAX_DOUBLE
         */
        merge_root = &(merge_node_array[0]);
        if ((ki_mmd = malloc(n * n * sizeof(double))) == NULL)
            MemError();
        for (k = 0; k < n; k++) /* sender */
            for (i = 0; i < n; i++)     /* receiver */
                ki_mmd[k * n + i] = +MAX_DOUBLE;
        lastdef = ELG_TRUE;
        break;
    default:
        break;
    }
    /* write header records direct to the output */
    ElgOut_write_record(OutFile, InRec);
    ElgRec_free(InRec);
    return lastdef;
}

void handle_event_record(ElgRec* InRec, procnum* input_processable_next)
{
    procnum i;
    event_entry *input_entry;
    timestamp Cij;
    elg_ui4 lid, slid, dlid, cid, tag, sent;
    elg_d8 time;


    /* This is now a record with timestamp >= 0,
       i.e. ELG_MPI_SEND, ELG_MPI_RECV, OMP_FORK or OMP_JOIN.
       All have a defined lovation (machine, node, process, tread).
       n is number of locations
       i is id of current location  i is in intervall [0..n-1] 
     */
    if (n == 0)
        elg_error_msg
            ("no ELG_LOCATION records found in input file header part");

    i = single_lid = ElgRec_read_location(InRec);
    if (i == ELG_NO_ID || i >= n)
        elg_error_msg("invalid location id %d", i);

    /*
     *  store entry into the input queue 
     */
    if ((input_entry = malloc(sizeof(event_entry))) == NULL)
        elg_error_msg("memory allocation error");

    input_entry->next = NULL;
    if (P[i].In_oldest == NULL)
    {                           /* damit bearbeitbar */
        *input_processable_next = i;    /* the list in P[].input_processable_next is empty */
        P[i].input_processable = ELG_TRUE;
        P[i].In_oldest = input_entry;
    }
    else
    {                           /* nicht bearbeitbar, da aeltere Events
                                   noch nicht bearbeitet */
        P[i].In_newest->next = input_entry;
    }
    P[i].In_newest = input_entry;
    input_entry->record = InRec;
    Cij = ElgRec_read_time(InRec);
    input_entry->Cj = Cij;

    /*
     *    HANDLE MPI_SEND,  MPI_RECV and internal messages
     */
    switch (ElgRec_get_type(InRec))
    {
    case ELG_MPI_SEND:
        ElgRec_read_MPI_SEND(InRec, &lid, &time, &dlid, &cid, &tag, &sent);
        input_entry->event_type = EV_send;
        input_entry->msg_to = dlid;
        input_entry->msg_id1 = cid;
        input_entry->msg_id2 = tag;
        break;
    case ELG_MPI_RECV:
        ElgRec_read_MPI_RECV(InRec, &lid, &time, &slid, &cid, &tag);
        input_entry->event_type = EV_receive;
        input_entry->msg_from = slid;
        input_entry->msg_id1 = cid;
        input_entry->msg_id2 = tag;
        break;
    default:
        input_entry->event_type = EV_internal;
    }

    input_entry->corresponding_recv_LCa_known = ELG_FALSE;
    input_entry->next_newer_send = NULL;
    input_entry->next_newer_recv = NULL;
    In_length++;
    input_entry->i = i;
}

void correct_time_exec(ElgRec* InRec, ElgOut* OutFile)
{
    ElgRec *OutRec;
    event_entry *output_entry;
    procnum i, k, input_processable_next;
    event_entry *input_entry;
    Booli input_processable;
    procnum AM_processable_next;
    csel_entry *one_csel, *prev_csel;
    timestamp Cij, LCij, LCaij, diff_LCa_C;
    float pij, qij, p_gammaij, q_gammaij;
    double divisor;
    double err, abs_err;
    static elg_ui4 lastdef = ELG_FALSE;

    input_processable_next = ELG_NO_ID;
    AM_processable_next = ELG_NO_ID;

    /* Handle deffinition records */
    if (InRec && !ElgRec_is_event(InRec))
    {  
        /* build lookup tables */
        lastdef = handle_def_record(InRec, OutFile);    
        return;
    }

    if (lastdef == ELG_FALSE)
        elg_error_msg("event record before initialisation complete");

    /* fill message queue */
    if (InRec != NULL)
    {
        handle_event_record(InRec, &input_processable_next);
        Cij = ElgRec_read_time(InRec);
    }

    /* process message queue */
    do     /* while (input_processable_next != ELG_NO_ID) */
    {      /* processing all processable events           */

        if (input_processable_next != ELG_NO_ID)
        {
            i = input_processable_next;
            input_entry = P[i].In_oldest;
            if (input_entry->event_type == EV_receive)
            {   /* whether corresponding sent event already processed */
                /* search from the _beginning_ of the list */
                one_csel = P[i].csel_first;
                input_processable = ELG_FALSE;
                prev_csel = NULL;
                while (one_csel != NULL)
                {
                    if ((one_csel->msg_from ==
                         input_entry->msg_from)
                        && (one_csel->msg_id1 ==
                            input_entry->msg_id1)
                        && (one_csel->msg_id2 == input_entry->msg_id2))
                    {
                        input_processable = ELG_TRUE;
                        input_entry->C_from = one_csel->C;
                        input_entry->LC_from = one_csel->LC;
                        input_entry->LCa_from = one_csel->LCa;
                        input_entry->corresponding_send =
                            one_csel->send_event;
                        /* remove one_csel from csel list */
                        if (prev_csel == NULL)
                        {
                            P[i].csel_first = one_csel->next;
                        }
                        else
                        {
                            prev_csel->next = one_csel->next;
                        }
                        if (one_csel == P[i].csel_last)
                            P[i].csel_last = prev_csel;
                        free((void *) one_csel);
                        one_csel = NULL;
                    }
                    else
                    {
                        prev_csel = one_csel;
                        one_csel = one_csel->next;
                    }
                }               /* while (one_csel != NULL) */
            }
            else
            {                   /* EV_send or EV_internal */
                input_processable = ELG_TRUE;
            }

            /* if EV_send, EV_internal or EV_recv with coresponded EV_send */
            if (input_processable)
            {                   /* input_processable */
                /* removing from input queue */
                P[i].In_oldest = input_entry->next;
                if (P[i].In_oldest == NULL)
                {   /* remove this input queue from input_processable_next list */
                    input_processable_next = P[i].input_processable_next;
                    P[i].input_processable = ELG_FALSE;
                    P[i].input_processable_next = ELG_NO_ID;
                }
                In_length--;

                /* processing the controlled logical clock */
                input_entry->not_the_first_event = P[i].initialized;
                Cij = input_entry->Cj;
                LCij = Cij;
                LCaij = Cij;
                if (!P[i].initialized)
                {
                    P[i].delta = delta_default;
                }
                else            /* P[i].initialized */
                {
                    if ((Cij - P[i].Cj1) < min_delta)
                        min_delta = Cij - P[i].Cj1;
                    LCij = max(LCij, P[i].LCj1 + P[i].delta);
                    LCaij = max(LCaij, P[i].LCaj1 + P[i].delta);
                    LCaij = max(LCaij, P[i].LCaj1 +
                            (timestamp) (P[i].gammaj * (Cij - P[i].Cj1)));
                    /*  ^- type cast to prohibit rounding errors
                       if timestamp != double */
                }
                if (input_entry->event_type == EV_receive)
                {
                    timestamp LCaij_shifted;
                    k = input_entry->msg_from;
                    if ((Cij - input_entry->C_from) < ki_mmd[k * n + i])
                        ki_mmd[k * n + i] = Cij - input_entry->C_from;
                    LCij = max(LCij, input_entry->LC_from +
                               mmd[mmd_individual * (k * n + i)]);
                    LCaij_shifted = LCaij;
                    LCaij = max(LCaij, input_entry->LCa_from +
                                mmd[mmd_individual * (k * n + i)]);
                    input_entry->discrete_advance = LCaij - LCaij_shifted;
                    if (((LCaij - Cij) > AM_cldiff) && (AM_maxerr > 0))
                    {
                        AM_cldiff = LCaij - Cij;
                        AM_interval =
                            max(AM_cldiff, AM_cldiff_default) / AM_maxerr;
                    }
                }
                input_entry->LCj = LCij;
                input_entry->LCaj = LCaij;
                if (input_entry->event_type == EV_receive)
                {
                    input_entry->corresponding_send->corresponding_recv_LCa =
                        LCaij;
                    input_entry->corresponding_send->
                        corresponding_recv_LCa_known = ELG_TRUE;
                    /* add k to AM_processable_next if the
                       input_entry->corresponding_send is the beginning
                       of the AM_postponed_send list, and if it is not
                       already in the AM_processable list */
                    if ((input_entry->corresponding_send
                         == P[k].AM_postponed_send) && (!P[k].AM_processable))
                    {
                        P[k].AM_processable_next = AM_processable_next;
                        P[k].AM_processable = ELG_TRUE;
                        AM_processable_next = k;
                    }
                }
                /* storing sent events into csel and putting corresponding
                   non-empty queues into input_processable_next */
                if (input_entry->event_type == EV_send)
                {
                    /* create new csel element */
                    if ((one_csel = malloc(sizeof(csel_entry))) == NULL)
                        MemError();
                    one_csel->C = Cij;
                    one_csel->LC = LCij;
                    one_csel->LCa = LCaij;
                    one_csel->msg_from = i;
                    one_csel->msg_id1 = input_entry->msg_id1;
                    one_csel->msg_id2 = input_entry->msg_id2;
                    one_csel->send_event = input_entry;
                    /* add one_csel at the _end_ of the csel list of P[k] */
                    k = input_entry->msg_to;
                    one_csel->next = NULL;
                    if (P[k].csel_last == NULL)
                        P[k].csel_first = one_csel;
                    else
                        P[k].csel_last->next = one_csel;
                    P[k].csel_last = one_csel;
                    /* add k to input_processable_next list if it has an entry in the
                       input queue and if it is not already in the list */
                    if ((P[k].In_oldest != NULL) && (!P[k].input_processable))
                    {
                        P[k].input_processable_next = input_processable_next;
                        P[k].input_processable = ELG_TRUE;
                        input_processable_next = k;
                    }
                }
                /* logical clock processed */
                P[i].LCaj_Cj = LCaij - Cij;

                /* recalculating gamma -- p-algorithm */
                /* pij = divident / divisor */

                /* computing divident = min(LC'-C) */
                diff_LCa_C = LCaij - Cij;
                if (i == index_min_LCa_C)
                {
                    if (diff_LCa_C > min_LCa_C)
                    {           /*look for new minimum */
                        index_min_LCa_C = 0;
                        min_LCa_C = P[0].LCaj_Cj;
                        for (k = 1; k < n; k++)
                        {
                            if (P[k].LCaj_Cj < min_LCa_C)
                            {
                                index_min_LCa_C = k;
                                min_LCa_C = P[k].LCaj_Cj;
                            }
                        }
                    }
                    else
                    {
                        min_LCa_C = diff_LCa_C;
                    }
                }
                else
                {
                    if (diff_LCa_C < min_LCa_C)
                    {
                        index_min_LCa_C = i;
                        min_LCa_C = diff_LCa_C;
                    }
                }

                /* computing divisor  */

                if (min_LCa_C == 0.0)
                {
                    sum16_LCa_C = 0.0;    /* forces start condition when min_LCa_C > 0 */
                }
                else
                {
                    if (sum16_LCa_C == 0.0)
                    {
                        /* new computation due to - start condition (sum16_LCa_C==0) */
                        sum16_LCa_C = 0;
                        for (k = 0; k < n; k++)
                        {
                            register double diff, pow16_LCa_C;
                            diff = P[k].LCaj_Cj;
                            pow16_LCa_C = pow16(diff);
                            sum16_LCa_C = sum16_LCa_C + pow16_LCa_C;
                            P[k].pow16_LCaj1_Cj1 = pow16_LCa_C;
                        }
                        max_sum16_LCa_C = sum16_LCa_C;
                    }
                    else
                    {
                        /* incremental change */
                        register double pow16_LCa_C;
                        pow16_LCa_C = pow16(diff_LCa_C);
                        sum16_LCa_C =
                            sum16_LCa_C + pow16_LCa_C - P[i].pow16_LCaj1_Cj1;
                        P[i].pow16_LCaj1_Cj1 = pow16_LCa_C;
                        max_sum16_LCa_C = max(max_sum16_LCa_C, sum16_LCa_C);
                        if (sum16_LCa_C <=
                            (PRECISION_OF_DOUBLE * 1e4) * max_sum16_LCa_C)
                        {
                            /* new computation due to risc of incorrectness */
                            sum16_LCa_C = 0;
                            for (k = 0; k < n; k++)
                            {
                                register double diff, pow16_LCa_C;
                                diff = P[k].LCaj_Cj;
                                pow16_LCa_C = pow16(diff);
                                sum16_LCa_C = sum16_LCa_C + pow16_LCa_C;
                                P[k].pow16_LCaj1_Cj1 = pow16_LCa_C;
                            }
                            max_sum16_LCa_C = sum16_LCa_C;
                        }
                    }
                }
                divisor = root16(sum16_LCa_C / sqrt((double) n));
                /* computing divisor and pij */
                pij = (divisor == 0.0 ? 0.0 : (min_LCa_C / divisor));

                if (pij <= pmin)
                {
                    p_gammaij = gamma_max;
                }
                else if (pij >= pmax)
                {
                    p_gammaij = 0.0;
                }
                else
                {
                    float x;
                    x = (pij - pmin) / (pmax - pmin);   /* 0<x<1 */
                    p_gammaij = gamma_max * (1 - x * x);
                }

                /* recalculating gamma -- q-algorithm */
                if (P[i].initialized)
                {
                    Dmaxj = Dmaxj - D_slow_factor * gamma1_init *
                        max(0, (LCij - LC_Dmaxj));
                    Dmaxj = max(0, Dmaxj);
                    LC_Dmaxj = max(LC_Dmaxj, LCij);
                }
                if (LCij - Cij > Dmaxj)
                {
                    Dmaxj = LCij - Cij;
                    LC_Dmaxj = LCij;
                }

                qij = (diff_LCa_C > 0 ? diff_LCa_C / Dmaxj : 1.0);
                if (qij <= qmin)
                {
                    q_gammaij = gamma_max;
                }
                else if (qij >= qmax)
                {
                    q_gammaij = 0.0;
                }
                else
                {
                    float x;
                    x = (qij - qmin + qij - qmax) / (qmax - qmin);      /* -1 < x < 1 */
                    q_gammaij = gamma_max * 0.25 * ((x * x - 3) * x + 2.0);
                }
                P[i].gammaj = min(p_gammaij, q_gammaij);

                /* controller processed */

                /* logical clock processed */
                P[i].initialized = ELG_TRUE;
                P[i].Cj1 = Cij;
                P[i].LCj1 = LCij;
                P[i].LCaj1 = LCaij;

                /* storing the event into the amortisation list of P[i] */
                input_entry->next = NULL;
                if (P[i].AM_newest != NULL)
                    P[i].AM_newest->next = input_entry;
                P[i].AM_newest = input_entry;
                switch (input_entry->event_type)
                {
                case EV_send:
                    if (P[i].AM_newest_send != NULL)
                        P[i].AM_newest_send->next_newer_send = input_entry;
                    P[i].AM_newest_send = input_entry;
                    if (P[i].AM_postponed_send == NULL)
                    {
                        P[i].AM_postponed_send = input_entry;
                        P[i].AM_LCa_limit = input_entry->LCaj;
                    }
                    if (P[i].AM_oldest_send == NULL)
                        P[i].AM_oldest_send = input_entry;
                    break;
                case EV_receive:
                    if (P[i].AM_newest_recv != NULL)
                        P[i].AM_newest_recv->next_newer_recv = input_entry;
                    P[i].AM_newest_recv = input_entry;
                    if (P[i].AM_todo_recv == NULL)
                        P[i].AM_todo_recv = input_entry;
                    break;
                default:
                    ;
                }
                if (P[i].AM_oldest == NULL)
                {
                    P[i].AM_oldest = input_entry;
                }
                if (P[i].AM_postponed_send == NULL)
                    P[i].AM_LCa_limit = input_entry->LCaj;
                /* add i to AM_processable_next if it is not
                   already in the AM_processable list */
                if (!P[i].AM_processable)
                {
                    P[i].AM_processable_next = AM_processable_next;
                    P[i].AM_processable = ELG_TRUE;
                    AM_processable_next = i;
                }
            }
            else
            {                   /* else -- not input_processable */
                /* remove this input queue from input_processable_next list */
                input_processable_next = P[i].input_processable_next;
                P[i].input_processable = ELG_FALSE;
                P[i].input_processable_next = ELG_NO_ID;
            }                   /* else -- not input_processable */
        }                       /* if (input_processable_next != ELG_NO_ID) */
        /* processing all amortisation lists */
        /* in general this are at maximum 2 lists: the list of the
           input event, and the list of a corresponding send event */
        while (AM_processable_next != ELG_NO_ID)
        {
            i = AM_processable_next;
            /* process all postponed send events */
            while ((P[i].AM_postponed_send !=
                    NULL)
                   && P[i].AM_postponed_send->corresponding_recv_LCa_known)
            {
                P[i].AM_postponed_send =
                    P[i].AM_postponed_send->next_newer_send;
                P[i].AM_LCa_limit =
                    (P[i].AM_postponed_send ==
                     NULL ? P[i].AM_newest->LCaj : P[i].AM_postponed_send->
                     LCaj);
            }
            /* process the amortization of all receive events
               with LCaj <= AM_LCa_limit */
            while ((P[i].AM_todo_recv != NULL)
                   && (P[i].AM_todo_recv->LCaj <= P[i].AM_LCa_limit))
            {
                if ((P[i].AM_todo_recv->
                     discrete_advance > 0)
                    && (P[i].AM_oldest != P[i].AM_todo_recv)
                    && (AM_maxerr > 0))
                {   /* ---- AMORTISATION ---- */
                    event_entry *first_AM_point;
                    event_entry *current_event;
                    event_entry *found;
                    event_entry *last_found_AM_point;
                    event_entry *left_AM_point;
                    event_entry *right_AM_point;
                    timestamp LCa_lower, LCa_left, LCa_current;
                    timestamp LCa_upper, LCa_right;
                    double AMORTI_lower, AMORTI_left;
                    double AMORTI_upper, AMORTI_right;
                    double allowed_current;
                    double min_gradient, gradient;

                    /* initialisation */
                    AMORTI_upper = P[i].AM_todo_recv->discrete_advance;
                    LCa_upper = P[i].AM_todo_recv->LCaj - AMORTI_upper;

                    /* if first event in the amortisation list is equal to
                       the overall first event on this process then
                       AMORTI_lower can be choosen as minimum of all
                       allowed amortisations at intermediate send events
                       and the AMORTI_upper */

                    if (P[i].AM_oldest->not_the_first_event)
                    {
                        LCa_lower =
                            min(P[i].AM_oldest->LCaj,
                                max(P[i].AM_LCa_next_before_oldest,
                                    P[i].AM_LCa_limit - AM_interval));
                        AMORTI_lower = 0;
                    }
                    else
                    {
                        LCa_lower = P[i].AM_oldest->LCaj;
                        AMORTI_lower = AMORTI_upper;
                        current_event = P[i].AM_oldest_send;
                        while ((current_event != NULL)
                               && (current_event->LCaj < LCa_upper))
                        {
                            procnum kk;
                            kk = current_event->msg_to;
                            LCa_current = current_event->LCaj;
                            allowed_current =
                                current_event->
                                corresponding_recv_LCa
                                - mmd[mmd_individual * (i * n + kk)] -
                                current_event->LCaj;
                            AMORTI_lower = min(AMORTI_lower, allowed_current);
                            current_event = current_event->next_newer_send;
                        }       /*while (...&& (current_event->LCaj < LCa_upper)) */
                    }
                    /* look for the amortisation points, i.e. the lower
                       convex hull from the begin of the amortisation interval
                       over all send events until the receive event with
                       the discrete_advance > 0 */
                    if (P[i].AM_oldest_send == NULL)
                    {
                        /* there is no intermediate send event
                           between lower_C and upper_C */
                        first_AM_point = NULL;
                    }
                    else
                    {           /*(P[i].AM_oldest_send != NULL) */
                        /* computing the convex hull under all
                           (LCa_max_allowed - LCa) values at each send event
                           and AMORTI_lower at LCa_lower
                           P[i].AM_todo_recv->discrete_advance at LCa_upper,
                           with
                           LCa_max_allowed = corresponding_recv_LCa - mmd */
                        first_AM_point = NULL;
                        last_found_AM_point = NULL;
                        do      /* while (found != NULL) */
                        {
                            found = NULL;
                            if (last_found_AM_point == NULL)
                            {
                                LCa_left = LCa_lower;
                                AMORTI_left = AMORTI_lower;
                                current_event = P[i].AM_oldest_send;
                                if (current_event->LCaj == LCa_lower)
                                    current_event =
                                        current_event->next_newer_send;
                            }
                            else
                            {
                                LCa_left = last_found_AM_point->LCaj;
                                AMORTI_left =
                                    last_found_AM_point->amortisation;
                                current_event =
                                    last_found_AM_point->next_newer_send;
                            }
                            min_gradient =
                                (AMORTI_upper - AMORTI_left) / (LCa_upper -
                                                                LCa_left);
                            while ((current_event != NULL)
                                   && (current_event->LCaj < LCa_upper))
                            {
                                procnum kk;
                                kk = current_event->msg_to;
                                LCa_current = current_event->LCaj;
                                allowed_current
                                    =
                                    current_event->
                                    corresponding_recv_LCa
                                    - mmd[mmd_individual * (i * n + kk)] -
                                    current_event->LCaj;
                                gradient =
                                    (allowed_current -
                                     AMORTI_left) / (LCa_current - LCa_left);
                                if (gradient <= min_gradient)
                                {
                                    found = current_event;
                                    min_gradient = gradient;
                                }
                                current_event =
                                    current_event->next_newer_send;
                            }   /*while (current_event->LCaj < LCa_upper) */
                            if (found != NULL)
                            {
                                if (first_AM_point == NULL)
                                {
                                    first_AM_point = found;
                                }
                                else
                                {
                                    last_found_AM_point->next_AM_point =
                                        found;
                                }
                                last_found_AM_point = found;
                                last_found_AM_point->
                                    amortisation
                                    =
                                    AMORTI_left
                                    +
                                    min_gradient *
                                    (last_found_AM_point->LCaj - LCa_left);
                            }   /*if (found != NULL) */
                        }
                        while (found != NULL);
                        if (last_found_AM_point != NULL)
                            last_found_AM_point->next_AM_point =
                                P[i].AM_todo_recv;
                    }           /*(P[i].AM_oldest_send != NULL) */
                    P[i].AM_todo_recv->amortisation = AMORTI_upper;
                    P[i].AM_todo_recv->next_AM_point = NULL;

                    /* now the amortisation can be done */
                    left_AM_point = P[i].AM_oldest;
                    right_AM_point = first_AM_point;
                    if (right_AM_point == NULL)
                        right_AM_point = P[i].AM_todo_recv;
                    left_AM_point->
                        amortisation =
                        AMORTI_lower +
                        (right_AM_point->
                         amortisation -
                         AMORTI_lower) *
                        (left_AM_point->LCaj -
                         LCa_lower) / (right_AM_point->LCaj - LCa_lower);
                    while (right_AM_point != NULL)
                    {
                        LCa_left = left_AM_point->LCaj;
                        LCa_right = right_AM_point->LCaj;
                        AMORTI_left = left_AM_point->amortisation;
                        AMORTI_right = right_AM_point->amortisation;
                        if (right_AM_point == P[i].AM_todo_recv)
                            LCa_right = LCa_right - AMORTI_right;
                        if (AMORTI_right > 0)
                        {
                            current_event = left_AM_point;
                            gradient =
                                (AMORTI_right - AMORTI_left) / (LCa_right -
                                                                LCa_left);
                            while (current_event != right_AM_point)
                            {
                                current_event->
                                    LCaj =
                                    current_event->
                                    LCaj +
                                    AMORTI_left + (current_event->LCaj -
                                                   LCa_left) * gradient;
                                /* */
                                current_event = current_event->next;
                            }
                        }       /*if (AMORTI_right > 0) */
                        left_AM_point = right_AM_point;
                        right_AM_point = right_AM_point->next_AM_point;
                    }           /* while (right_AM_point != NULL) */
                }               /* ---- AMORTISATION ---- */
                P[i].AM_todo_recv = P[i].AM_todo_recv->next_newer_recv;
            }
            /* move all events before (AM_LCa_limit - AM_interval)
               to the output queue */
            while ((P[i].AM_oldest != NULL)
                   && (P[i].AM_oldest->LCaj <
                       P[i].AM_LCa_limit - AM_interval))
            {
                input_entry = P[i].AM_oldest;
                LCaij = input_entry->LCaj;
                /* delete input_entry on amortisation list */
                if (P[i].AM_oldest_send == P[i].AM_oldest)
                    P[i].AM_oldest_send =
                        P[i].AM_oldest_send->next_newer_send;
                if (P[i].AM_newest_recv == P[i].AM_oldest)
                    P[i].AM_newest_recv = NULL;
                if (P[i].AM_newest_send == P[i].AM_oldest)
                    P[i].AM_newest_send = NULL;
                if (P[i].AM_newest == P[i].AM_oldest)
                    P[i].AM_newest = NULL;
                P[i].AM_LCa_next_before_oldest = P[i].AM_oldest->LCaj;
                P[i].AM_oldest = P[i].AM_oldest->next;
                /* move input_entry to the output buffer */
                input_entry->next = NULL;
                if (P[i].output_merge_node->event == NULL)
                {
                    AM_out_filled++;
                    P[i].output_merge_node->event = input_entry;
                    P[i].output_merge_newest_event = input_entry;
                }
                else
                {
                    P[i].output_merge_newest_event->next = input_entry;
                    P[i].output_merge_newest_event = input_entry;
                }
            }                   /* while ((... < P[i].AM_LCa_limit - AM_interval)) */
            /* remove this AM list from AM_processable_next list */
            AM_processable_next = P[i].AM_processable_next;
            P[i].AM_processable = ELG_FALSE;
            P[i].AM_processable_next = ELG_NO_ID;
        }                       /* while (AM_processable_next != ELG_NO_ID) */

        /* output of all allowed elements from the output queue */
        /* begin of merge-sort algorithm */
        if (InRec == NULL)
        {
            for (i = 0; i < n; i++)
            {
                if (P[i].output_merge_node->event == NULL)
                {
                    AM_out_filled++;
                    if (P[i].AM_oldest == NULL)
                        P[i].output_merge_node->event = final_event;
                    else
                    {
                        P[i].output_merge_node->event = P[i].AM_oldest;
                        P[i].AM_newest->next = final_event;
                    }
                }
                else
                {
                    if (P[i].AM_oldest == NULL)
                        P[i].output_merge_newest_event->next = final_event;
                    else
                    {
                        P[i].output_merge_newest_event->next = P[i].AM_oldest;
                        P[i].AM_newest->next = final_event;
                    }
                }
                P[i].output_merge_newest_event = NULL;  /*unused in the future */
            }
        }



        while ((AM_out_filled == n) && (merge_root->event != final_event))
        {
            if (merge_root->event != NULL)
            {
                output_entry = merge_root->event;
                merge_root->event = NULL;
                /* write output_entry to output file and free it */
                OutRec = output_entry->record;
                ElgRec_write_time(OutRec, output_entry->LCaj);
                ElgOut_write_record(OutFile, OutRec);
                i = output_entry->i;
                Cij = output_entry->Cj;
                LCij = output_entry->LCj;
                LCaij = output_entry->LCaj;
                if (output_entry->not_the_first_event)
                {
                    err =
                        (Cij > P[i].Out_Cj1
                         ?
                         ((LCaij - P[i].Out_LCaj1) -
                          (Cij - P[i].Out_Cj1)) / (Cij -
                                                   P[i].Out_Cj1) : 99.999);
                    abs_err = Abs(err);
                    if (abs_err == 0.0)
                    {
                        count_err_eq_zero++;
                    }
                    else if (abs_err <= AM_maxerr)
                    {
                        count_err_le_maxerr++;
                        sum_err_le_maxerr = sum_err_le_maxerr + abs_err;
                        if (abs_err > max_err_le_maxerr)
                            max_err_le_maxerr = abs_err;
                    }
                    else        /* (abs_err > AM_maxerr) */
                    {
                        count_err_gt_maxerr++;
                        sum_err_gt_maxerr = sum_err_gt_maxerr + abs_err;
                        if (abs_err > max_err_gt_maxerr)
                            max_err_gt_maxerr = abs_err;
                    }
                    if (abs_err < min_err)
                        min_err = abs_err;
                }
                P[i].Out_Cj1 = Cij;     /* for next iteration */
                P[i].Out_LCaj1 = LCaij;
                ElgRec_free(OutRec);
                free(output_entry);
            }
            /* else  the tree is not yet fully filled,
               i.e. we are in the initalization phase */
            /* now get the next event */
            node = merge_root;
            while (!node->final)
            {
                if (node->left->event == NULL)
                    /* goto left to init. parts of the left tree  */
                {
                    node = node->left;
                }
                else if (node->right->event == NULL)
                    /* goto right to init. parts of the right tree */
                {
                    node = node->right;
                }
                else
                {               /* compare, fetch smaller one and goto its node */
                    /* comparison */
                    if (node->left->event->LCaj <= node->right->event->LCaj)
                        nextnode = node->left;
                    else
                        nextnode = node->right;
                    /* fetch */
                    node->event = nextnode->event;
                    if (nextnode->final)
                    {
                        nextnode->event = nextnode->event->next;
                        if (nextnode->event == NULL)
                            AM_out_filled--;
                    }
                    else
                    {
                        nextnode->event = NULL;
                    }
                    /* goto nextnode */
                    node = nextnode;
                }
            }                   /* while (!node.final) */
        }                       /* while (AM_out_filled == n) */
        /* end of merge-sort algorithm */
    }
    while (input_processable_next != ELG_NO_ID);
}

void correct_time_finish()
{
    FILE *to_optfile = NULL;
    procnum i, k;
    double max_LCa_C, multiplier;
    char *format, *unit;
    double min_ki_mmd, max_ki_mmd, sum_ki_mmd, avg_ki_mmd;
    int cnt_ki_mmd;

    if (to_optfilename != NULL)
        to_optfile = fopen(to_optfilename, "w");

    printf("number of nodes = %1u\n", n);
    if (verbose)
    {
        max_LCa_C = 0;
        for (i = 0; i < n; i++)
            max_LCa_C = max(max_LCa_C, P[i].LCaj1 - P[i].Cj1);
        max_LCa_C = max_LCa_C * clock_period;
        if (max_LCa_C >= 1e+0)
        {
            multiplier = 1e+0;
            unit = "sec";
        }
        else if (max_LCa_C >= 1e-3)
        {
            multiplier = 1e+3;
            unit = "ms";
        }
        else if (max_LCa_C >= 1e-6)
        {
            multiplier = 1e+6;
            unit = "us";
        }
        else if (max_LCa_C >= 1e-9)
        {
            multiplier = 1e+9;
            unit = "ns";
        }
        else
        {
            multiplier = 1e+12;
            unit = "ps";
        }
        if (max_LCa_C * multiplier >= 100)
            format = " %6.2lf";
        else if (max_LCa_C * multiplier >= 10)
            format = " %6.3lf";
        else
            format = " %6.4lf";
        printf
            ("\nDifferences new timestamps - old timestamps at the last event in [%s]\n",
             unit);
        printf("Node  i=");
        for (i = 0; i < min(10, n); i++)
            printf(" %6u", i);
        for (i = 0; i < n; i++)
        {
            if ((i % 10) == 0)
                printf("\n%5u+i ", i);
            printf(format,
                   (P[i].LCaj1 - P[i].Cj1) * clock_period * multiplier);
        }
        printf("\n");
    }                           /*if (verbose) */
    if (AM_maxerr > 0)
    {
        AM_cldiff = AM_cldiff * clock_period * 1e6;
        if (verbose)
        {
            printf
                ("\nMaximal clock differences = %4.2lf us,  used start-value = %4.2lf us\n",
                 AM_cldiff, AM_cldiff_default * clock_period * 1e6);
            if (AM_cldiff >
                1.10 /* +10% */  * AM_cldiff_default * clock_period * 1e6)
                printf
                    ("\nADVICE -- use for correcting similar tracefiles:  -c %1.0lf\n",
                     AM_cldiff);
        }                       /*if (verbose) */
    }
    min_delta = min_delta * clock_period * 1e6  /*[us] */
        ;
    if (verbose)
    {
        printf
            ("\nMinimal difference between the original timestamps of two\n"
             "events in the same process = %5.3lf us\n", min_delta);

        if ((min_delta > 0)
            && (min_delta < delta_default * clock_period * 1e6))
        {
            printf
                ("\nADVICE -- use for correcting similar tracefiles:  -d %5.3lf\n",
                 min_delta);
        }
    }                           /*if (verbose) */
    max_ki_mmd = -MAX_DOUBLE;
    min_ki_mmd = MAX_DOUBLE;
    cnt_ki_mmd = 0;
    sum_ki_mmd = 0;
    for (k = 0; k < n; k++)     /* sender */
        for (i = 0; i < k; i++) /* receiver */
            if ((ki_mmd[k * n + i] < MAX_DOUBLE)
                && (ki_mmd[i * n + k] < MAX_DOUBLE))
            {
                double delay;
                delay =
                    (ki_mmd[k * n + i] +
                     ki_mmd[i * n + k]) / 2 * clock_period * 1e6
                    /*[us] */
                    ;
                ki_mmd[k * n + i] = delay;
                ki_mmd[i * n + k] = delay;
                if (delay < min_ki_mmd)
                    min_ki_mmd = delay;
                if (delay > max_ki_mmd)
                    max_ki_mmd = delay;
                cnt_ki_mmd++;
                sum_ki_mmd = sum_ki_mmd + delay;
            }
            else
            {
                ki_mmd[k * n + i] = MAX_DOUBLE;
                ki_mmd[i * n + k] = MAX_DOUBLE;
            }
    avg_ki_mmd = (cnt_ki_mmd > 0 ? sum_ki_mmd / cnt_ki_mmd : 0);
    if (verbose)
    {
        if (cnt_ki_mmd > 0)
        {
            printf("\nMinimal message delay between 2 processes =\n"
                   "   (min=%5.3lfus,  avg=%5.3lfus,  max=%5.3lfus)\n",
                   min_ki_mmd, avg_ki_mmd, max_ki_mmd );
            printf ("with min/avg/max over all paires of processes with messages in both directions\n");
            printf ("These values have include an error <= 2*(max. clock drifts over the whole time)\n");
            if ((min_ki_mmd > 0) &&
                (Abs (mmd_default * clock_period * 1e6 - mmd_factor * min_ki_mmd) >
                (0.05 * mmd_default * clock_period * 1e6)))
            {
                /* the 80% are necessary because the minimum is computed
                   as average between 2 messages in the opposite directions
                   and we assume that one has the minimal message delay (mmd),
                   but the other has 1.50*mmd, and therefore the average has
                   1.25*mmd, and therefore mmd is 80% of 1.25*mmd. */
                printf
                    ("\nADVICE -- use for correcting similar tracefiles:  -m %5.3lf\n",
                     0.80 * min_ki_mmd);
                printf
                    ("but this value can be meaningless if the clock drifts are to large.\n");
            }
            if ((min_ki_mmd > 0) && (max_ki_mmd > 1.5 * min_ki_mmd) &&
                 ! mmd_individual)
            {
                printf("\n"
                       "ADVICE -- for correcting this tracefile one may get better results if one\n"
                       "uses individual minimal message delays for each pair of processes, i.e.\n");
                if (to_optfilename == NULL)
                {
                    printf ("start two passes, the first with -w timecorrect.opt, "
                            "and the second with -r timecorrect.opt \n");
                }
                else
                {
                    printf("start a second pass with the same input file and "
                           "the option -r %s\n", to_optfilename);
                    if (to_optfile == NULL)
                        printf ("and make that in this first pass the optionfile '%s' is writable!\n",
                                to_optfilename);
                }
            }
        }                       /* cnt_ki_mmd > 0 */
        else
        {                       /* cnt_ki_mmd == 0 */
            printf("\n"
                   "The minimal message delay cannot be computed because there is \n"
                   "not a pair of processes that have exchanged messages in both "
                   "directions.\n");
        }                       /* cnt_ki_mmd == 0 */
    }                           /*if (verbose) */
    if (verbose)
    {
        printf ("\nThe correction of the timestamps causes additional errors on the length of\n"
                "time intervals between two successive events in the same process:\n");
        printf(" %7ld intervals with            error  =      0%%\n",
               count_err_eq_zero);
        printf(" %7ld intervals with      0%% <  error <= %6.3f%%",
               count_err_le_maxerr, 100 * AM_maxerr);
        if (count_err_le_maxerr > 0)
            printf (" : avg.=%6.3f%%, max=%6.3f%%",
                     100 * sum_err_le_maxerr / count_err_le_maxerr,
                     100 * max_err_le_maxerr);
        printf("\n");
        printf (" %7ld intervals with %6.3f%% <  error           ",
                count_err_gt_maxerr, 100 * AM_maxerr);
        if (count_err_gt_maxerr > 0)
            printf (" : avg.=%6.3f%%, max=%6.3f%%",
                    100 * sum_err_gt_maxerr / count_err_gt_maxerr,
                    100 * max_err_gt_maxerr);
        printf("\n");
        printf("Error summary:\n");
        printf
            (" %7ld intervals with %6.3f%% <= error <= %6.3f%%",
             count_err_eq_zero
             +
             count_err_le_maxerr
             + count_err_gt_maxerr, 100 * min_err,
             100 * max(max_err_le_maxerr, max_err_gt_maxerr));
        if ((count_err_eq_zero + count_err_le_maxerr + count_err_gt_maxerr) > 0)
            printf(" : avg.=%6.3f%%\n",
                   100 * (sum_err_le_maxerr +
                          sum_err_gt_maxerr) / (count_err_eq_zero +
                                                count_err_le_maxerr +
                                                count_err_gt_maxerr));
        if (mmd_individual)
        {
            printf("\n"
                   "The local error rates may be greater due to the use of individual minimal\n"
                   "message delays, but the error for time intervals between events on different\n"
                   "nodes may be significantly smaller, but this can not be measured.\n");
        }
    }                           /*if (verbose) */
    if (to_optfile != NULL)
    {
        if (AM_maxerr > 0)
            fprintf(to_optfile, "-c %4.2lf\n", AM_cldiff);
        if (min_delta > 0)
            fprintf(to_optfile, "-d %5.3lf\n", min_delta);
        if ((cnt_ki_mmd > 0) && (min_ki_mmd > 0))
        {
            fprintf(to_optfile, "-m %1.0lf\n", min_ki_mmd);
            for (k = 0; k < n; k++)     /* sender */
                for (i = 0; i < n; i++) /* receiver */
                    if (ki_mmd[k * n + i] != MAX_DOUBLE)
                        fprintf(to_optfile, "-i %1u_%1u_%1.0lf\n", k, i,
                                ki_mmd[k * n + i]);
        }
        fclose(to_optfile);
        printf("write options to file '%s'\n", to_optfilename);
    }
    if (In_length > 0)
    {
        elg_error_msg("%1d records not processed\n"
                      "e.g. due to the lack of a corresponding SENDMSG record\n",
                      In_length);

    }
    if (P != NULL)
        free(P);
    if (mmd_individual)
        free(mmd);
}

/*--------------------------------------------------------------------*/
/**  main message loop 
 *   
 *   - open input and output trace files 
 *   - initialize record handler
 *   - read input file record by record
 *   - call record handlig function
 *   - finalize record handler
 *   - print statistics
 *   - close input and output files
 */
void correct_time(const char* ifn, const char* ofn)
{
    void *in, *out;
    ElgRec *rec = NULL;
    elg_ui4 numrec = 0, done = 0;
    struct stat statbuf;

    elg_cntl_msg("correcting %s-->%s...", ifn, ofn);

    /* open input file */
    if ((in = ElgIn_open(ifn)) == NULL)
        elg_error_msg("Cannot open file %s\n", ifn);
    stat(ifn, &statbuf);

    /* open output file */
    if ((out = ElgOut_open(ofn, ElgIn_get_byte_order(in),
                           ELG_UNCOMPRESSED)) == NULL)
        elg_error_msg("Cannot open file %s\n", ofn);

    correct_time_init();

    /* This loop is executed for each input record */
    while ((rec = ElgIn_read_record(in)) != NULL)
    {
        correct_time_exec(rec, out);

        if (numrec % 5000 == 0)
            fprintf(stderr, "%3.0f%%\r",
                    ((double) done / statbuf.st_size) * 100.0);
        numrec++;
        if (rec)
            done += 2 + ElgRec_get_length(rec); /* header size + payload size */
    }
    /* empty message queue */
    correct_time_exec(NULL, out);

    printf("record count: %6u\n", numrec);

    /* print statistics */
    correct_time_finish();

    /* close files */
    ElgIn_close(in);
    ElgOut_close(out);
}

void parse_option(const char ch, const char* val)
{
    procnum immd_from, immd_to;
    double immd_us;
    immd_entry *immd;
    switch (ch)
    {
    case 'c':
        AM_cldiff_default = atof(val);  /*[usec] */
        if (AM_cldiff_default < 0)
            elg_error_msg("'-c' option must be positive number");
        AM_cldiff_default *= 1e-6 / clock_period;
        break;
    case 'd':
        delta_default = atof(val);
        if (delta_default <= 0)
            elg_error_msg("'-d' option must be positive number");
        delta_default *= 1e-6 / clock_period;
        break;
    case 'e':
        AM_maxerr = atof(val) * 0.01 /*[1%] */ ;
        if (AM_maxerr < 0)
            elg_error_msg("'-e' option must be not negative number");
        break;
    case 'f':
        mmd_factor = atof(val);
        if (mmd_factor <= 0)
            elg_error_msg("'-f' option must be positive number");
        break;
    case 'i':
        if (3 != sscanf(val, "%u_%u_%lf", &immd_from, &immd_to, &immd_us))
            elg_error_msg("'-i' option: invalid format");
        if (immd_us <= 0)
            elg_error_msg("'immd_us' must be positive");
        if ((immd = malloc(sizeof(immd_entry))) == NULL)
            MemError();
        if (immd_first == NULL)
            immd_first = immd;
        else
            immd_last->next = immd;

        immd_last = immd;
        immd->next = NULL;
        immd->from = immd_from;
        immd->to = immd_to;
        immd->immd = immd_us * 1e-6 / clock_period;
        break;
    case 'm':
        mmd_default = atof(val);        /*[usec] */
        if (mmd_default <= 0)
            elg_error_msg("'-m' option  must be positive number");
        mmd_default *= 1e-6 / clock_period;
        break;
    case 'r':
        from_optfilename = "timecorrect.opt";
        break;
    case 'R':
        from_optfilename = strdup(val);
        break;
    case 'v':
        verbose = ELG_TRUE;
        break;
    case 'w':
        to_optfilename = "timecorrect.opt";
        break;
    case 'W':
        to_optfilename = strdup(val);
        break;
    case '?':
    case 'h':
        print_help();
        exit(0);
        break;
    default:
        print_usage();
        elg_error_msg("invalid option: '%c'", ch);
    }
}

void read_options(const char* filename)
{
    FILE *optfile;
    char optname[100];
    char optvalue[100];
    int ret;

    optfile = fopen(filename, "r");
    if (optfile == NULL)
        elg_error_msg("Cannot open file %s for reading\n", filename);

    while ((ret = fscanf(optfile, "-%s %s ", optname, optvalue)) != EOF)
    {
        if (ret != 2)
            elg_error_msg
                ("invalid options file '%s' optname='%s', optvalue='%s'",
                 filename, optname, optvalue);
        parse_option(optname[0], optvalue);
    }
    fclose(optfile);
    printf("read options from file '%s'\n", filename);
}

int parse_options(int argc, char* argv[])
{
    int ch;
    immd_entry *immd;

    /* read command line options */
    while ((ch = getopt(argc, argv, "c:d:e:f:i:m:R:W:rwvh?")) != EOF)
        parse_option(ch, optarg);

    /* read option file if any */
    if (from_optfilename)
        read_options(from_optfilename);

    /* aplay minimal message delay factor to all related items */
    mmd_default *= mmd_factor;
    immd = immd_first;
    while (immd != NULL)
    {
        immd->immd *= mmd_factor;
        immd = immd->next;
    }
    return optind;
}

/*--------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    char *infile = NULL;
    char *outfile = NULL;
    int opt_ind;

    /* set default options */
    default_opt();

    /* parse command line, read options from file */
    opt_ind = parse_options(argc, argv);

    /* check if input and output file names are present */
    if ((argc - opt_ind) != 2)
    {
        print_usage();
        return 1;
    }

    /* check if input and output file names differs */
    infile = epk_get_elgfilename(argv[optind]);
    outfile = argv[optind + 1];
    if (strcmp(infile, outfile) == 0)
        elg_error_msg("<infile> has same name as <outfile> has");

    /* correct timestamps for event records */
    correct_time(infile, outfile);

    return 0;
}
