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

#ifndef SILAS_HYPOTHESIS_H
#define SILAS_HYPOTHESIS_H

#include <vector>

#include "CallbackManager.h"

#include "ReenactCallbackData.h"
#include "HypothesisPart.h"

namespace silas
{
    /** 
     * A hypothesis is a set of smaller hypothesis parts each defining a
     * hypothetic behaviour of some kind. Each part defines its own callback
     * functions an the hypothesis serves as a container to ease callback
     * registration for the simulator.
     */
    class Hypothesis
    {
        public:
            /**
             * Constructor for Model
             */
            Hypothesis();
            /**
             * Standard destructor for Model
             */
            virtual ~Hypothesis();
            /**
             * Add a partial hypothesis
             * @param part Reference of partial hypothesis
             */
            void add_part(HypothesisPart* part);
            /**
             * Register all callbacks of the included partial hypotheses
             * @param run       ID of the replay run the callbacks will
             *                  be registered for
             * @param cbmanager Reference of callback manager used for
             *                  regitration
             */
            void register_callbacks(const int run, pearl::CallbackManager* cbmanager) const;

        private:
            /**
             * List of partial hypotheses
             */
            std::vector<HypothesisPart*> parts;
    };
}

#endif

