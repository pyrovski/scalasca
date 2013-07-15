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

#ifndef REENACT_CALLBACKDATA_H
#define REENACT_CALLBACKDATA_H

#include <set>
#include <stack>
#include <string>
#include <vector>

#include <mpi.h>

#include "pearl_types.h"

#include "Event.h"

#include "CallbackData.h"
#include "Buffer.h"
#include "Model.h"

/*-------------------------------------------------------------------------*/
/**
 * @file    ReenactCallbackData.h
 * @brief   Declaration of the class CallbackData.
 *
 * This header file provides the declaration of the class CallbackData.
 */
/*-------------------------------------------------------------------------*/

namespace silas
{

//--- Forward declarations --------------------------------------------------

class Event;


/*-------------------------------------------------------------------------*/
/**
 * @class   RequestInfo
 * @brief   Contains information for nonblocking request object tracking.
 */
/*-------------------------------------------------------------------------*/

struct RequestInfo
{
    MPI_Request    request;
    silas::Buffer* buffer;
    pearl::Event   event;
};


/*-------------------------------------------------------------------------*/
/**
 * @class   CallbackData
 * @brief   Class for data objects passed to callback function during
 *          simulation.
 */
/*-------------------------------------------------------------------------*/

class CallbackData : public pearl::CallbackData
{
    // type for the list of actions
    typedef std::vector<int> action_list_t;
    // type for a single block of ratios
    typedef std::vector<double> ratio_block_t;
    // type for the list of ratio blocks
    typedef std::vector<ratio_block_t> ratio_list_t;
    // type for time intervals (used in idle aggregation)
    typedef struct 
    { 
        pearl::timestamp_t start; 
        pearl::timestamp_t end; 
    } interval_t;
    // type for idle epoch list
    typedef std::vector<interval_t> interval_list_t;

  public:
    /// @name Public types
    /// @{

    // nonblocking communication request map
    typedef std::map<uint32_t, RequestInfo> requestmap_t;
    typedef std::set<uint32_t>              requestset_t;
    
    /// @}
    /// @name Constructors & destructor
    /// @{
      
    CallbackData(const pearl::GlobalDefs& defs,
            const pearl::LocalTrace& trace,
            const silas::Model& model);
    virtual ~CallbackData();

    /// @}
    /// @name Pre- and postprocessing
    /// @{

    virtual void preprocess(const pearl::Event& event);
    virtual void postprocess(const pearl::Event& event);

    /// @}
    /// @name Global definition handling
    /// @{
    
    virtual const pearl::GlobalDefs& get_global_defs() const;
    
    /// @}
    /// @name Handling timestamp offset delta_t
    /// @{
    
    pearl::timestamp_t get_delta_t() const;
    void set_delta_t(pearl::timestamp_t timespan);
    void mod_delta_t(pearl::timestamp_t timespan);

    /// @}
    /// @name Handling dummy buffers
    /// @{

    void  init_buffers();
        
    void* get_send_buffer(std::size_t capacity);
    void* get_p2p_send_buffer(std::size_t capacity);
    void* get_recv_buffer(std::size_t capacity);

    silas::Buffer* pop_nb_recv_buffer();
    void  push_nb_recv_buffer(silas::Buffer* buf);

    /// @}
    /// @name Tracking of send/recv information
    /// @{
   
    void track_max_count(int count);
    int  get_max_count();
    void set_max_count(int count);

    void track_max_pt2pt_count(int count);
    int  get_max_pt2pt_count();
    void set_max_pt2pt_count(int count);
    
    void track_max_buffered_count(int count);
    int  get_max_buffered_count();

    void track_max_mult_comp_count(int count);
    int  get_max_mult_comp_count();

    void inc_nb_recv_count();
    void dec_nb_recv_count();

    /// @}
    /// @name Tracking nonblocking communication requests
    /// @{

    requestmap_t& get_requestmap() {
        return m_requests;
    }

    requestset_t& get_uncomplete_requests() {
        return m_uncomplete_requests;
    }

    MPI_Request*            get_request_array();
    MPI_Status*             get_status_array();
    requestmap_t::iterator* get_requestmap_iter_array();
   
    /// @}
    /// @name Timestamp access
    /// @{

    pearl::timestamp_t get_original_timestamp() const
            { return m_original_timestamp; }
    void set_original_timestamp(pearl::timestamp_t timestamp)
            { m_original_timestamp = timestamp; }

    pearl::timestamp_t get_preceeding_timestamp() const         
            { return m_preceeding_timestamp;}
    void set_preceeding_timestamp(pearl::timestamp_t timestamp) 
            { m_preceeding_timestamp = timestamp; }

    /// @}
    /// @name Debug routines
    /// @{
    
    void clear_diff_vector();
    int get_diff_vector_size();
    void write_diff_vector();

    void save_ts(bool orig, pearl::timestamp_t diff);
    void write_ts();
    void estimate_thresholds();

    /// @}
    /// @name Actin list handling
    /// @{
    
    void activate_action_list();
    void deactivate_action_list();
    void init_action_list();

    void del_action(const pearl::Event& event)
            { m_action_list.erase(m_action_list.begin() + event.get_id()); }
    void set_action(const int action, const pearl::Event& event);
    void reset_action_idx();
    int  get_next_action()       
            { return *(m_action_idx++); }
    int  get_current_action()    
            { return *(m_action_idx-1); }
    int  get_action(const pearl::Event& event) const
            { return m_action_list[event.get_id()]; }

    pearl::Event get_prev_event(const pearl::Event& event);
    unsigned int get_aggregation_count(const pearl::Event& event) const;
    
    /// @}
    /// @name Idle aggregation handling
    /// @{
    
    void track_idle_time(const pearl::Event& event);
    void aggregate_idle_time(const pearl::Event& event);
    bool is_aggregatable(const pearl::Event& event) const;

    void reverse_epoch_list();
    void reverse_ratio_list();
    void calculate_timespans(const pearl::Event& event,
            pearl::timestamp_t start, pearl::timestamp_t end);
    /// @}

    pearl::LocalTrace& get_trace() const 
        { return m_local_trace; }
    
    void purge_deleted_events();

  private:
    const pearl::GlobalDefs& m_global_defs;
    pearl::LocalTrace& m_local_trace;
    const silas::Model& m_model;
    
    bool is_idle(int action) const;
    
    /// @}
    /// @name Variables for timestamp adjustment
    /// @{
    
    /// standard time adjustment of an event
    pearl::timestamp_t  m_delta_t;
    /// Original timestamp of current event before modification
    pearl::timestamp_t  m_original_timestamp;
    /// Original timestamp of preceeding event before modification
    pearl::timestamp_t  m_preceeding_timestamp;
    
    /// @}
    /// @name Buffer handling for reenactment of communication 
    /// @{ 

    /** Buffer for collective sending operations */
    silas::Buffer* m_send_buffer;
    /** Buffer for collective and blocking receive operations */
    silas::Buffer* m_recv_buffer;
    /** buffer list for p2p sending communication */
    std::vector<silas::Buffer*> m_p2p_send_buffers;
    /** buffer list for nonblocking receives */
    std::vector<silas::Buffer*> m_nb_recv_buffers;
    /** maximum number of send buffers */
    int m_max_sendbufs;
    /** maximum number of send buffers */
    int m_max_recvbufs;
    /** maximal count of bytes used in communication */
    int m_max_count;
    /** maximal count of bytes used in pt2pt communication */
    int m_max_pt2pt_count;
    /** maximal count of bytes used in buffered communication */
    int m_max_buffered_count;
    /** maximal buffer size for nonblocking receives */
    int m_max_nb_recv_size;
    /** current and maximal count of simultaneous (nonblocking) receive requests */
    int m_cur_nb_recv_count;
    int m_max_nb_recv_count;
    /** maximal message size (runtime configurable) */
    int m_max_msg_size;

    /// @}
    /// @name Nonblocking communication handling
    /// @{

    /** request tracking map */
    requestmap_t m_requests;
    requestset_t m_uncomplete_requests;

    int          m_max_mult_comp_count;

    /** preallocated request array for multiple completion events */
    MPI_Request*            m_request_array;
    MPI_Status*             m_status_array;
    requestmap_t::iterator* m_requestmap_iter_array;

    /// @}

    std::vector<pearl::timestamp_t> ts_diff;
    std::vector<pearl::timestamp_t> orig_ts;
    std::vector<pearl::timestamp_t> sim_ts;
    unsigned int m_event_count;

    ratio_list_t m_event_ratios;
    int m_idle_count;
    pearl::timestamp_t m_aggregated_time;

    interval_list_t m_idle_epochs;

    /// @name Model options
    /// @{
    
    double m_noop_threshold;
    double m_skip_threshold;

    /// @}
    /// @name Action list handling
    /// @{
    /// list of predefined actions
    action_list_t m_action_list;
    /// current index into action list
    action_list_t::iterator m_action_idx;
    bool m_action_list_active;
    /// @}
};


}   /* namespace silas */


#endif   /* !REENACT_CALLBACKDATA_H */
