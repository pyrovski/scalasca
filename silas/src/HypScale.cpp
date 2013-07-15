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
#include <HypScale.h>
#include <ReenactCallbackData.h>
#include <silas_events.h>
#include <silas_util.h>

using namespace silas;
using namespace std;

HypScale::HypScale()
{
}

HypScale::~HypScale()
{
}

void HypScale::register_callbacks(const int run, pearl::CallbackManager* cbmanager)
{
    if ((run == 0) && (!registered_regions.empty()))
    {
        log("Registering HypScale for ATOMIC_REGION_EXIT\n");
        cbmanager->register_callback(ATOMIC_REGION_EXIT, 
                PEARL_create_callback(this, 
                    &HypScale::cb_scale_region));
    }
}

void HypScale::cb_scale_region(const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);

    double local_timespan,new_timespan;
    
    std::map<std::string,double>::iterator entry =
            registered_regions.find(event.get_cnode()->get_region()->get_name());

    /* check if current region is registered to be scaled */
    if (entry == registered_regions.end())
            return;
    
    /* calculate original timespan of current region */
    local_timespan = event->get_time() - event.prev()->get_time();
    /* scale to new timespan */ 
    new_timespan = local_timespan * entry->second;
   
    /* set new time */
    event->set_time(event.prev()->get_time() + new_timespan);
    data->set_delta_t(data->get_delta_t() + new_timespan - local_timespan);
}

void HypScale::register_region(string region_name, double ratio)
{
    log("Registering region \"%s\" for scaling\n", region_name.c_str());
    registered_regions.insert(make_pair(region_name,ratio));
}

