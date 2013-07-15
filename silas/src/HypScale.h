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

#ifndef SILAS_REENACT_SCALE_H
#define SILAS_REENACT_SCALE_H

#include <set>
#include "CallbackManager.h"
#include "HypothesisPart.h"

namespace silas
{

    /**
     * Balance the timespan of atomic regions
     */
    class HypScale : public HypothesisPart
    {
        public:
            /// @name Constructors & Destructors
            /// @{
            
            HypScale();
            virtual ~HypScale();
            
            /// @}
            /**
             * Register the needed callbacks to enable the management of
             * user defined modifications on specific events.
             */
            virtual void register_callbacks(const int run, pearl::CallbackManager* cb_manager);
            /**
             * register a region to be balanced
             */
            virtual void register_region(std::string region_name, double ratio);
            
        private:
            /**
             * callback to perform scaling of region timespans
             */
            void cb_scale_region(const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);

            std::map<std::string,double> registered_regions;
    };
}

#endif



