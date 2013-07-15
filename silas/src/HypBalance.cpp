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

// Pythia includes
#include <HypothesisPart.h>
#include <HypBalance.h>
#include <ReenactCallbackData.h>
#include <silas_events.h>
#include <silas_util.h>

using namespace silas;
using namespace std;

HypBalance::HypBalance()
{
}

HypBalance::~HypBalance()
{
}

void HypBalance::register_callbacks(const int run, pearl::CallbackManager* cbmanager)
{
    if ((run == 0) && (!registered_regions.empty()))
    {
        log("Registering balance hypothesis part for ATOMIC_REGION_EXIT\n");
        cbmanager->register_callback(ATOMIC_REGION_EXIT, 
                PEARL_create_callback(this, 
                    &HypBalance::cb_balance_region));
    }
}

void HypBalance::cb_balance_region(const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);

    double local_timespan,new_timespan;
    double sum_timespan;
    int num_processes;

    // log("Trying Region\"%s\"\n", event.get_cnode()->get_region()->get_name().c_str());

    /* check if current region is registered to be balanced */
    if (registered_regions.find(event.get_cnode()->get_region()->get_name()) 
            == registered_regions.end())
            return;
    
    /* calculate original timespan of current region */
    local_timespan = event->get_time() - event.prev()->get_time();
    
    MPI_Allreduce(&local_timespan, &sum_timespan, 1, MPI_DOUBLE, MPI_SUM, 
            MPI_COMM_WORLD);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    /* calculate balanced timespan */
    new_timespan = sum_timespan / num_processes;

    // log("Region %s: old timespan: %.2gs :: balanced timespan: %.2gs\n", 
    //     event.get_cnode()->get_region()->get_name().c_str(), local_timespan, new_timespan);

    /* set new time */
    event->set_time(event.prev()->get_time() + new_timespan);
    data->set_delta_t(data->get_delta_t() + new_timespan - local_timespan);
}

void HypBalance::register_region(string region_name)
{
    log("Registering region \"%s\" for balancing\n", region_name.c_str());
    registered_regions.insert(region_name);
}

