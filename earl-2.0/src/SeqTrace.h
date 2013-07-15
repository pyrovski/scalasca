/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_SEQTRACE_H
#define EARL_SEQTRACE_H

/*
 *----------------------------------------------------------------------------
 *
 * class SeqTrace
 *
 * Trace with sequential access to events
 *
 *----------------------------------------------------------------------------
 */

#include <cstdio>
#include <string>
#include <map>

#include "Callsite.h"
#include "Event.h"
#include "EventBuffer.h"
#include "Grid.h"
#include "HWMetric.h"
#include "Region.h"
#include "State.h"
#include "MPIWindow.h"

#include "elg_defs.h"
#include "elg_readcb.h"

typedef struct ElgRCB_struct ElgRCB;


namespace earl
{
  class SeqTrace
  {
  public:  
    SeqTrace(std::string path);
    virtual ~SeqTrace();

    long       get_nevents()  const { return nevents; }

    long       get_nfiles()   const { return filem.size(); }
    long       get_nregs()    const { return regm.size();  }
    long       get_ncsites()  const { return csitem.size();}
    long       get_nmets()    const { return metm.size();  }

    earl::Grid*      get_grid()               const { return grid; }
    
    /* Find file name by file ID. Throw RuntimeError if not exist */
    std::string      get_file(long file_id)   const;

    /* Find region by ID. Throw RuntimeError if not exist */
    earl::Region*    get_reg(long reg_id)     const;
    
    /* Find callsite by ID. Throw RuntimeError if not exist */
    earl::Callsite*  get_csite(long csite_id) const;
    
    /* Find metric by ID. Throw RuntimeError if not exist */
    earl::Metric*    get_met(long met_id)     const;
    
    /* Find string by ID. Throw RuntimeError if not exist */
    std::string      get_str(long strid)      const;
    
    /* Find region by name.  Returns NULL if not exist */
    earl::Region*    get_reg(const std::string& name ) const;

  protected:

    earl::Event next_event(earl::State& state, earl::EventBuffer* buffer);
    void        set_offset(const fpos_t* offset);
    fpos_t      get_offset();

  private:
    typedef std::map<long, earl::Region*>   RegionMap;
    typedef std::map<long, std::string>     StringMap;
    typedef std::map<long, earl::Callsite*> CallsiteMap;
    typedef std::map<long, earl::Metric*>   MetricMap;

    StringMap   strm;
    StringMap   filem;
    RegionMap   regm;
    CallsiteMap csitem;
    MetricMap   metm;
    
    // EPILOG callback functions elg_readcb_*  doesn't forward C++ exeptions but
    // does call abort() instead.
    // To forward error message do following:  
    //   catch all exceptions in the implementation of elg_readcb_* functions
    //   and save the error message in this member variable.
    std::string err_msg;


    earl::Region* tracing;
    earl::Region* no_logging;

    earl::Grid* grid;
    long nevents;

    ElgRCB* file;
   
    /* predicate which compares name of region, stored in RegionMap */
    struct eq_name {
        const std::string name;
        eq_name(const std::string& name):name(name){};
        bool operator() ( const RegionMap::value_type& item ) {
           return name == item.second->get_name();
        }
    };

  private:
    void set_cart_coords ( earl::Cartesian* cart, earl::Location* loc, 
                           const std::vector<long>& coordvec)
    { cart->set_coords(loc, coordvec); };

    std::map<long, earl::Metric*>& get_metm() {return metm;};
    earl::Region* get_tracing() {return tracing;};
    earl::Region* get_no_logging() {return no_logging;};
   
    earl::State& get_state() {return *cur_state;};
    earl::EventBuffer* get_buffer(){return cur_buffer;};

  private:
    /* data requiered for read call back funktions */ 

    /* input data*/ 
    earl::State* cur_state;
    earl::EventBuffer* cur_buffer;
    bool lastdef;
 
    /* output data*/
    earl::Event       cur_event;

    /* temp data */
    elg_ui4     cur_strid;

    /* declare EPILOG read callback functions as firends */
    friend
    void ::elg_readcb_ALL( elg_ui1 type, 
                           elg_ui1 length, void* userdata);

    friend
    void ::elg_readcb_STRING( elg_ui4 strid,      /* string identifier */
                              elg_ui1 cntc,       /* number of continuation records */
                              const char* str,    /* string */
                              void* userdata);

    friend
    void ::elg_readcb_STRING_CNT( const char* str, void* userdata);

    friend
    void ::elg_readcb_MACHINE( elg_ui4 mid,       /* machine identifier */
                               elg_ui4 nodec,     /* number of nodes */
                               elg_ui4 mnid,      /* machine name identifier */
                               void* userdata);

    friend
    void ::elg_readcb_NODE( elg_ui4 nid,          /* node identifier */
                            elg_ui4 mid,          /* machine identifier */
                            elg_ui4 cpuc,         /* number of CPUs */
                            elg_ui4 nnid,         /* node name identifer */
                            elg_d8  cr,           /* number of clock cycles/sec */
                            void* userdata );

    friend
    void ::elg_readcb_PROCESS( elg_ui4 pid,       /* process identifier */
                               elg_ui4 pnid,      /* process name identifier */
                               void* userdata);

    friend
    void ::elg_readcb_THREAD( elg_ui4 tid,        /* thread identifier */
                              elg_ui4 pid,        /* process identifier */
                              elg_ui4 tnid,        /* thread name identifier */
                              void* userdata);

    friend
    void ::elg_readcb_LOCATION( elg_ui4 lid,      /* location identifier */
                                elg_ui4 mid,      /* machine identifier */
                                elg_ui4 nid,      /* node identifier */
                                elg_ui4 pid,      /* process identifier */
                                elg_ui4 tid,      /* thread identifier */
                                void* userdata);

    friend
    void ::elg_readcb_FILE( elg_ui4 fid,          /* file identifier */
                            elg_ui4 fnid,         /* file name identifier */
                            void* userdata );

    friend
    void ::elg_readcb_REGION( elg_ui4 rid,        /* region identifier */
                              elg_ui4 rnid,       /* region name identifier */
                              elg_ui4 fid,        /* source file identifier */
                              elg_ui4 begln,      /* begin line number */
                              elg_ui4 endln,      /* end line number */
                              elg_ui4 rdid,       /* region description identifier */
                              elg_ui1 rtype,      /* region type */
                              void* userdata   );

    friend
    void ::elg_readcb_CALL_SITE( elg_ui4 csid,    /* call site identifier */
                                 elg_ui4 fid,     /* source file identifier */
                                 elg_ui4 lno,     /* line number */
                                 elg_ui4 erid,    /* region identifer to be entered */
                                 elg_ui4 lrid,    /* region identifer to be left */
                                 void* userdata);

    friend
    void ::elg_readcb_CALL_PATH( elg_ui4 cpid,    /* call-path identifier */
                                 elg_ui4 rid,     /* node region identifier */
                                 elg_ui4 ppid,    /* parent call-path identifer */
                                 elg_ui8 order,   /* node order specifier */
                                 void* userdata);

    friend
    void ::elg_readcb_METRIC( elg_ui4 metid,      /* metric identifier */
                              elg_ui4 metnid,     /* metric name identifier */
                              elg_ui4 metdid,     /* metric description identifier */
                              elg_ui1 metdtype,   /* metric data type */
                              elg_ui1 metmode,    /* metric mode */
                              elg_ui1 metiv,      /* time interval referenced */
                              void* userdata );

    friend
    void ::elg_readcb_MPI_COMM( elg_ui4 cid,      /* communicator identifier */
                                elg_ui1 mode,     /* mode flags */
                                elg_ui4 grpc,     /* size of bitstring in bytes */
                                elg_ui1 grpv[],   /* bitstring defining the group */
                                void* userdata);
      
    friend
    void ::elg_readcb_CART_TOPOLOGY(elg_ui4 topid,    /* topology id */
                                    elg_ui4 tnid,     /* topology name id */
                                    elg_ui4 cid,      /* communicator id */
                                    elg_ui1 ndims,    /* number of dimensions */
                                    elg_ui4 dimv[],   /* number of processes in each dim */  
                                    elg_ui1 periodv[],/* periodicity in each dim */
                                    elg_ui4 dimids[], /* string ids of dimension names */
                                    void* userdata);     

    friend
    void ::elg_readcb_CART_COORDS   ( elg_ui4 topid,   /* topology id */
                                      elg_ui4 lid,     /* location id */
                                      elg_ui1 ndims,   /* number of dimensions */
                                      elg_ui4 coordv[],/* coordinates in each dimension */
                                      void* userdata);

    friend
    void ::elg_readcb_OFFSET( elg_d8  ltime,     /* local time */
                              elg_d8  offset,    /* offset to global time */
                              void* userdata);

    friend
    void ::elg_readcb_NUM_EVENTS( elg_ui4 eventc, void* userdata );

    friend
    void ::elg_readcb_EVENT_TYPES( elg_ui4 ntypes, elg_ui1 typev[], void* userdata );

    friend
    void ::elg_readcb_EVENT_COUNTS( elg_ui4 ntypes, elg_ui4 countv[], void* userdata );

    friend
    void ::elg_readcb_LAST_DEF( void* userdata );

    friend
    void ::elg_readcb_MPI_WIN( elg_ui4 wid,      /* window identifier */
                               elg_ui4 cid,      /* communicator identifier */
                               void* userdata);

    friend
    void ::elg_readcb_ENTER( elg_ui4 lid,       /* location identifier             */
                             elg_d8  time,      /* time stamp                      */
                             elg_ui4 rid,       /* region identifier of the region being entered */
                             elg_ui1 metc,      /* number of metrics               */
                             elg_ui8 metv[],    /* metric values                   */
                             void* userdata);

    friend
    void ::elg_readcb_ENTER_CS( elg_ui4 lid,       /* location identifier          */
                                elg_d8  time,      /* time stamp                   */
                                elg_ui4 csid,      /* call-site identifier of the call-site being entered */
                                elg_ui1 metc,      /* number of metrics            */
                                elg_ui8 metv[],    /* metric values                */
                                void* userdata);
        
    friend
    void ::elg_readcb_EXIT( elg_ui4 lid,       /* location identifier              */
                            elg_d8  time,      /* time stamp                       */
                            elg_ui1 metc,      /* number of metrics                */
                            elg_ui8 metv[],    /* metric values                    */
                            void* userdata);

  /* -- MPI-1 -- */

    friend
    void ::elg_readcb_MPI_SEND( elg_ui4 lid,       /* location identifier              */
                                elg_d8  time,      /* time stamp                       */
                                elg_ui4 dlid,      /* destination location identifier  */
                                elg_ui4 cid,       /* communicator identifier          */
                                elg_ui4 tag,       /* message tag                      */
                                elg_ui4 sent,      /* message length in bytes          */
                                void* userdata);

    friend
    void ::elg_readcb_MPI_RECV( elg_ui4 lid,       /* location identifier              */
                                elg_d8  time,      /* time stamp                       */
                                elg_ui4 slid,      /* source location identifier       */
                                elg_ui4 cid,       /* communicator identifier          */
                                elg_ui4 tag,       /* message tag                      */
                                void* userdata);

    friend
    void ::elg_readcb_MPI_COLLEXIT( elg_ui4 lid,       /* location identifier              */
                                    elg_d8  time,      /* time stamp                       */
                                    elg_ui1 metc,      /* number of metrics                */
                                    elg_ui8 metv[],    /* metric values                    */
                                    elg_ui4 rlid,      /* root location identifier         */
                                    elg_ui4 cid,       /* communicator identifier          */
                                    elg_ui4 sent,      /* bytes sent                       */
                                    elg_ui4 recvd,     /* bytes received                   */
                                    void* userdata);

  /* -- MPI-2 One-sided Operations -- */

    friend
    void ::elg_readcb_MPI_PUT_1TS( elg_ui4 lid,       /* location identifier              */
                                   elg_d8  time,      /* time stamp                       */
                                   elg_ui4 dlid,      /* destination location identifier  */
                                   elg_ui4 wid,       /* window identifier                */
                                   elg_ui4 rmaid,     /* RMA operation identifier         */
                                   elg_ui4 nbytes,    /* message length in bytes          */
                                   void* userdata);
           
    friend
    void ::elg_readcb_MPI_PUT_1TE( elg_ui4 lid,       /* location identifier              */
                                   elg_d8  time,      /* time stamp                       */
                                   elg_ui4 slid,      /* source location identifier       */
                                   elg_ui4 wid,       /* window identifier                */
                                   elg_ui4 rmaid,     /* RMA operation identifier         */
                                   void* userdata);

    friend
    void ::elg_readcb_MPI_GET_1TO( elg_ui4 lid,       /* location identifier              */
                                   elg_d8  time,      /* time stamp                       */
                                   elg_ui4 rmaid,     /* RMA operation identifier         */
                                   void* userdata);

    friend
    void ::elg_readcb_MPI_GET_1TS( elg_ui4 lid,       /* location identifier              */
                                   elg_d8  time,      /* time stamp                       */
                                   elg_ui4 dlid,      /* destination location identifier  */
                                   elg_ui4 wid,       /* window identifier                */
                                   elg_ui4 rmaid,     /* RMA operation identifier         */
                                   elg_ui4 nbytes,    /* message length in bytes          */
                                   void* userdata);

    friend
    void ::elg_readcb_MPI_GET_1TE( elg_ui4 lid,       /* location identifier       */
                                   elg_d8  time,      /* time stamp                */
                                   elg_ui4 slid,      /* source location identifier*/
                                   elg_ui4 wid,       /* window identifier         */
                                   elg_ui4 rmaid,     /* RMA operation identifier  */
                                   void* userdata);
           
    friend
    void ::elg_readcb_MPI_WINEXIT( elg_ui4 lid,       /* location identifier       */
                                   elg_d8  time,      /* time stamp                */
                                   elg_ui1 metc,      /* number of metrics         */
                                   elg_ui8 metv[],    /* metric values             */
                                   elg_ui4 wid,       /* window identifier         */
                                   elg_ui4 cid,       /* communicator identifier   */
                                   elg_ui1 synex,     /* synchronization exit flag */
                                   void* userdata);

    friend
    void ::elg_readcb_MPI_WINCOLLEXIT( elg_ui4 lid,       /* location identifier   */
                                       elg_d8  time,      /* time stamp            */
                                       elg_ui1 metc,      /* number of metrics     */
                                       elg_ui8 metv[],    /* metric values         */
                                       elg_ui4 wid,       /* window identifier     */
                                       void* userdata);

    friend
    void ::elg_readcb_MPI_WIN_LOCK( elg_ui4 lid,       /* location identifier      */
                                    elg_d8  time,      /* time stamp               */
                                    elg_ui4 llid,      /* lock location identifier */
                                    elg_ui4 wid,       /* window identifier        */
                                    elg_ui1 ltype,     /* lock type                */
                                    void* userdata);
            
    friend
    void ::elg_readcb_MPI_WIN_UNLOCK( elg_ui4 lid,       /* location identifier      */
                                      elg_d8  time,      /* time stamp               */
                                      elg_ui4 llid,      /* lock location identifier */
                                      elg_ui4 wid,       /* window identifier        */
                                      void* userdata);

  /* -- Generic One-sided Operations -- */

    friend
    void ::elg_readcb_PUT_1TS( elg_ui4 lid,       /* location identifier              */
                               elg_d8  time,      /* time stamp                       */
                               elg_ui4 dlid,      /* destination location identifier  */
                               elg_ui4 rmaid,     /* RMA operation identifier         */
                               elg_ui4 nbytes,    /* message length in bytes          */
                               void* userdata);
           
    friend
    void ::elg_readcb_PUT_1TE( elg_ui4 lid,       /* location identifier              */
                               elg_d8  time,      /* time stamp                       */
                               elg_ui4 slid,      /* source location identifier       */
                               elg_ui4 rmaid,     /* RMA operation identifier         */
                               void* userdata);

    friend
    void ::elg_readcb_GET_1TS( elg_ui4 lid,       /* location identifier              */
                               elg_d8  time,      /* time stamp                       */
                               elg_ui4 dlid,      /* destination location identifier  */
                               elg_ui4 rmaid,     /* RMA operation identifier         */
                               elg_ui4 nbytes,    /* message length in bytes          */
                               void* userdata);

    friend
    void ::elg_readcb_GET_1TE( elg_ui4 lid,       /* location identifier              */
                               elg_d8  time,      /* time stamp                       */
                               elg_ui4 slid,      /* source location identifier       */
                               elg_ui4 rmaid,     /* RMA operation identifier         */
                               void* userdata);
           
    friend
    void ::elg_readcb_COLLEXIT( elg_ui4 lid,      /* location identifier              */
                                elg_d8  time,     /* time stamp                       */
                                elg_ui1 metc,     /* number of metrics                */
                                elg_ui8 metv[],   /* metric values                    */
                                elg_ui4 rlid,     /* root location identifier         */
                                elg_ui4 cid,      /* communicator identifier          */
                                elg_ui4 sent,     /* bytes sent                       */
                                elg_ui4 recvd,    /* bytes received                   */
                                void* userdata);

    friend
    void ::elg_readcb_ALOCK(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        elg_ui4 lkid,      /* identifier of the lock being acquired */
                        void* userdata);

    friend
    void ::elg_readcb_RLOCK(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        elg_ui4 lkid,      /* identifier of the lock being released */
                        void* userdata);

   /* -- OpenMP -- */

    friend
    void ::elg_readcb_OMP_FORK(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        void* userdata);

    friend
    void ::elg_readcb_OMP_JOIN(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        void* userdata);

    friend
    void ::elg_readcb_OMP_ALOCK(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        elg_ui4 lkid,      /* identifier of the lock being acquired */
                        void* userdata);

    friend
    void ::elg_readcb_OMP_RLOCK(
                        elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        elg_ui4 lkid,      /* identifier of the lock being released */
                        void* userdata);

    friend
    void ::elg_readcb_OMP_COLLEXIT( elg_ui4 lid,       /* location identifier      */
                                    elg_d8  time,      /* time stamp               */
                                    elg_ui1 metc,      /* number of metrics        */
                                    elg_ui8 metv[],    /* metric values            */
                                    void* userdata);

  /* -- EPILOG Internal -- */

    friend
    void ::elg_readcb_LOG_OFF( elg_ui4 lid,       /* location identifier           */
                               elg_d8  time,      /* time stamp                    */
                               elg_ui1 metc,      /* number of metrics             */
                               elg_ui8 metv[],    /* metric values                 */
                               void* userdata);
       
    friend
    void ::elg_readcb_LOG_ON( elg_ui4 lid,       /* location identifier            */
                              elg_d8  time,      /* time stamp                     */
                              elg_ui1 metc,      /* number of metrics              */
                              elg_ui8 metv[],    /* metric values                  */
                              void* userdata);
      
    friend
    void ::elg_readcb_ENTER_TRACING( elg_ui4 lid,       /* location identifier        */
				     elg_d8  time,      /* time stamp                 */
				     elg_ui1 metc,      /* number of metrics          */
				     elg_ui8 metv[],    /* metric values              */
				     void* userdata );

    friend
    void ::elg_readcb_EXIT_TRACING( elg_ui4 lid,       /* location identifier         */
				    elg_d8  time,      /* time stamp                  */
				    elg_ui1 metc,      /* number of metrics           */
				    elg_ui8 metv[],    /* metric values               */
				    void* userdata );
  };

}

#endif

