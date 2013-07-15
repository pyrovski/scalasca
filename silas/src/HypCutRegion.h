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

#ifndef SILAS_HYP_CUT_REGION_H
#define SILAS_HYP_CUT_REGION_H

#include <set>
#include <string>

#include "CallbackManager.h"

namespace silas
{

    /**
     * Cut a region out of the trace
     *
     * INFO: As it is not efficiently possible to delete events from the
     * trace in memory, a flag is used to identify the events to be
     * skipped in later processing.
     */
    class HypCutRegion : public HypothesisPart
    {
        public:
            /// @name Constructors & Destructors
            /// @{
            
            HypCutRegion();
            virtual ~HypCutRegion();
            
            /// @}
            /**
             * Register the needed callbacks to enable the management of
             * user defined modifications on specific events.
             */
            virtual void register_callbacks(const int run, pearl::CallbackManager* cb_manager);
           
            virtual void register_region(std::string region);
        private:
            void cb_enter_handler( const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);
            void cb_exit_handler( const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);
            /**
             * Specifies the region to be cut in cut mode
             */
            std::string active_region;
            /**
             * specifies the mode
             * - 0 no cutting active
             * - 1 cutting active
             */
            int cut_mode;
            
            std::set<std::string> registered_regions;
    };
}

#endif



