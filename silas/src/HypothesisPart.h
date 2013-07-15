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

#ifndef SILAS_HYPOTHESIS_PART_H
#define SILAS_HYPOTHESIS_PART_H

#include "CallbackManager.h"

namespace silas
{
    /**
     * Base class for partial hypotheses
     */
    class HypothesisPart
    {
        public:
            /**
             * Destructor.
             */
            virtual ~HypothesisPart() {};
            /**
             * Register the needed callbacks to enable the management of
             * user defined modifications on specific events.
             */
            virtual void register_callbacks(const int run, pearl::CallbackManager* cb_manager) = 0;
            
    };
}

#endif


