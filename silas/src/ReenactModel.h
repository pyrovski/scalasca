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

#ifndef SILAS_REENACT_MODEL_H
#define SILAS_REENACT_MODEL_H

#include <map>
#include <set>

#include <mpi.h>

#include "Model.h"
#include "ReenactCallbackData.h"

namespace silas
{

/**
 * Simulation model based on reenaction of user time and message
 * transfers. User regions will be simulated by usleep calls and MPI
 * calls will be reenacted with dummy data.
 */
class ReenactModel : public Model
{
    public:
        /// @name Constructors & Destructors
        /// @{
        
        ReenactModel();
        virtual ~ReenactModel();
        
        /// @}
        
        /**
         * Return the model name as a std::string to be used in 
         * log functions etc.
         * @return name of the model
         */
        virtual const std::string get_name() const;
        /**
         * Trigger callback registration
         */
        virtual void register_callbacks(const int run, pearl::CallbackManager* cbmanager);
        /**
         * Retrieve configuration of simulation runs
         */
        virtual std::vector<Model::Config> get_run_configuration() const;
    private:
        /// @name Preparation step callbacks
        /// @{
        void cb_start_prepare(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_finish_prepare(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_send_prepare(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_collective_prepare(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_any_prepare(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        /// @}
        /// @name Idle time aggregation step callbacks
        /// @{
        void cb_start_aggregate(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_any_aggregate(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_aggregate_ignore(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_finish_aggregate(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        /// @}
        /// @name Simulation step callbacks
        /// @{
        void cb_start(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_finish(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_begin(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_end(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_send(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_ssend(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_bsend(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_isend(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_issend(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_ibsend(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_recv(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_irecv(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_start(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_sendrecv(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_wait(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_waitany(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_waitsome(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_waitall(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_test(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_testany(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_testsome(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_testall(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_idle(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_idle_write(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_any(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_barrier(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_bcast(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_allreduce(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_allgather(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_alltoall(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_reduce(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_scan(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_mpi_exscan(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);

        /// @}
        /// @name Postprocessing callbacks
        /// @{
        void cb_start_postprocess(const pearl::CallbackManager& cbmanager,
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_any_postprocess(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_recalc(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_ignore_postprocess(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        /// @}
        /// @name Verification callbacks
        /// @{
        void cb_start_verify(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_finish_verify(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        void cb_any_verify(const pearl::CallbackManager& cbmanager, 
                int user_event, const pearl::Event& event, 
                pearl::CallbackData* cdata);
        /// @}

        /**
         * Determine correct action for MPI_Test exit events 
         *
         */

        bool has_completions(const pearl::Event& exit);

        /**
         * Predraw a completion for event req_ev to location of req_ev. 
         * Essentially swaps req_ev and req_ev.completion() and deletes all
         * subsequent tests for the request id associated with req_ev.
         * @param data          current callback data context
         * @param req_ev        the event to predraw
         */

        void predraw_completion(silas::CallbackData* data, const pearl::Event& req_ev);

        /**
         * Delete a MPI request event and, if possible, the surrounding
         * enter/exit events
         * @param event         the MPI request event to check
         */

        void delete_event(silas::CallbackData* data, const pearl::Event& event);

        /**
         * Simulate timespan between two events
         * @param time          time till the function returns
         */
        void simulate_timespan(pearl::timestamp_t time);
        /**
         * Get high resolution wallclock time
         */
        pearl::timestamp_t get_wtime();

        /// Internal types
        struct EventInfo {
            silas::CallbackData::requestmap_t::iterator it;
            pearl::Event                                ev;    
        };

        /// zero-point of original trace
        pearl::timestamp_t zero_point;
        /// wallclock time at simulation replay start
        pearl::timestamp_t reference_timestamp;
        
        int get_commsize(MPI_Comm comm);
        std::map<MPI_Comm,int> m_comm_sz;
        
        /// @name Flags and Options
        /// @{
        std::set<std::string> m_flags;
        std::map<std::string,std::string> m_options;
        /// @}
    };
}

#endif
