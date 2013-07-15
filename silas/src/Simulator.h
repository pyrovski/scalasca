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


#ifndef SILAS_SIMULATOR_H
#define SILAS_SIMULATOR_H

#include "Model.h"
#include "Hypothesis.h"

namespace pearl
{
    class CallbackManager;
}

namespace silas
{

    /**
     * Core component of the simulator, responsible for execution of the
     * simulation.
     */
    class Simulator
    {
        public:
            /**
             * Construct a simulator object working on the given trace.
             * The trace object will be modified by the simulation.
             */
            Simulator(pearl::LocalTrace& trace, 
                      Model* model, 
                      Hypothesis* hypothesis);
            /**
             * Cleaning up simulator
             */
            ~Simulator();
            /**
             * Starting the simulator
             */
            void run() const;
        
         private:
            /** local event trace */
            pearl::LocalTrace& m_trace;
            /** model to be used */
            Model* m_model;
            /** hypothesis to be simulated */
            Hypothesis* m_hypothesis;
    };
    
}

#endif


