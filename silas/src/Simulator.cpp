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

#include <mpi.h>

#include <CallbackManager.h>
#include <Callsite.h>
#include <Cartesian.h>
#include <CNode.h>
#include <Error.h>
#include <GlobalDefs.h>
#include <LocalTrace.h>
#include <Location.h>
#include <Machine.h>
#include <Node.h>
#include <Process.h>
#include <Region.h>
#include <Thread.h>
#include <Timer.h>
//#include <pearl_replay.h>
#include <pearl_types.h>

#include <epk_archive.h>
#include <epk_conf.h>

#include "silas_replay.h"
#include "silas_events.h"
#include "Simulator.h"
#include "ReenactCallbackData.h"


extern void log(const char* fmt, ...);

using namespace std;
using namespace pearl;
using namespace silas;

/**
 * Create a simulator object with references on the event trace, a
 * pointer to the model, as well as the hypothesis.
 * @param trace Reference of the local event trace
 * @param model Pointer to the model used for simulation
 * @param hypothesis Pointer to the simulation hypothesis
 */
Simulator::Simulator(pearl::LocalTrace& trace,
                     Model* model,
                     Hypothesis* hypothesis) 
  : m_trace(trace)
{
    /* initialize local objects */
    m_model      = model;
    m_hypothesis = hypothesis;
}

/**
 * Clean up model and hypothesis objects
 */
Simulator::~Simulator()
{
    /* free objects */
    delete m_model;
    delete m_hypothesis;
}

/**
 * Start the simulation.
 */
void Simulator::run() const 
{
    Timer timer;
    
    /* Setup callback data */
    silas::CallbackData        data(m_trace.get_definitions(),m_trace,*m_model);
    std::vector<Model::Config> runtime_config(m_model->get_run_configuration());

    for (unsigned run = 0; run < runtime_config.size(); run++)
    {
        pearl::CallbackManager m_cbmanager;
        
        m_model->register_callbacks(run, &m_cbmanager);
        m_hypothesis->register_callbacks(run, &m_cbmanager);
       
        log("%s ...\n", runtime_config[run].description);
        timer.start();

        m_cbmanager.notify(START, m_trace.begin(), &data);

        if (runtime_config[run].forward)
            SILAS_forward_replay(m_trace, m_cbmanager, &data, 
                                 runtime_config[run].mincbevent);
        else
            SILAS_backward_replay(m_trace, m_cbmanager, &data, 
                                  runtime_config[run].mincbevent);

        m_cbmanager.notify(FINISHED, m_trace.begin(), &data);

        log("                               done (%s).\n", timer.value_str().c_str());
    }
}
