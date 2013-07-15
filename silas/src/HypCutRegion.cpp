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

#include <string>
#include <mpi.h>

// Scalasca includes
#include <CallbackManager.h>
#include <Event.h>
#include <CNode.h>
#include <Region.h>
#include <CallbackData.h>
#include <MpiComm.h>
#include <Location.h>

// Pythia includes
#include <HypothesisPart.h>
#include <HypCutRegion.h>
//#include <ReenactCallbackData.h>
#include <silas_events.h>
#include <silas_util.h>

using namespace std;
using namespace pearl;
using namespace silas;

HypCutRegion::HypCutRegion()
{
}

HypCutRegion::~HypCutRegion()
{
}

void HypCutRegion::register_callbacks(const int run, pearl::CallbackManager* cbmanager)
{
    if ((run == 0) && (!registered_regions.empty()))
    {
        log("Registering surgeon hypothesis part for ENTER\n");
        cbmanager->register_callback(ENTER, 
                PEARL_create_callback(this, 
                    &HypCutRegion::cb_enter_handler));
        log("Registering surgeon hypothesis part for EXIT\n");
        cbmanager->register_callback(EXIT, 
                PEARL_create_callback(this, 
                    &HypCutRegion::cb_exit_handler));
    }
}

void HypCutRegion::register_region(std::string region)
{
}

void HypCutRegion::cb_enter_handler(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);
    //std::string region = lowercase(event.get_cnode()->get_region()->get_name());

    std::map<std::string,double>::iterator entry =
            registered_regions.find(event.get_cnode()->get_region()->get_name());

    /* check if current region is registered to be scaled */
    if (deletion_phase)
    {
        data->set_action(silas::DELETE, event); 
    }
    else
    {
        if (entry == registered_regions.end())
                return;
        else
        {
            deletion_phase = true;
            data->set_action(silas::DELETE, event); 
        }
    }
}

void HypCutRegion::cb_collexit_handler(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);

    MpiComm *comm = event->get_comm();
    int cut = (calltree_depth > 0 ? 1 : 0);
    
    MPI_Allreduce(&cut, 1, MPI_INT, MPI_MAX, comm->get_comm());
}

void HypCutRegion::cb_exit_handler(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);
}

