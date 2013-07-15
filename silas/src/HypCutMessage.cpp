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

#include <cstdio>
#include <string>
#include <mpi.h>

// BGL includes
//#include <rts.h>

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
#include <HypCutMessage.h>
//#include <ReenactCallbackData.h>
#include <silas_events.h>
#include <silas_util.h>

using namespace silas;
using namespace std;
using namespace pearl;

HypCutMessage::HypCutMessage()
{
}

HypCutMessage::~HypCutMessage()
{
}

void HypCutMessage::register_callbacks(const int run, pearl::CallbackManager* cbmanager)
{
    if ((run == 0) && (m_reg_msg_size.size() || m_reg_msg_tag.size()))
    {
        log("Registering message surgeon for SEND_EXIT\n");
        cbmanager->register_callback(SEND_EXIT, 
                PEARL_create_callback(this, 
                    &HypCutMessage::cb_check_send));
        log("Registering message surgeon for RECV_EXIT\n");
        cbmanager->register_callback(RECV_EXIT, 
                PEARL_create_callback(this, 
                    &HypCutMessage::cb_check_recv));
    }
}

void HypCutMessage::remove_message(const pearl::Event& exit, silas::CallbackData* data)
{
    pearl::Event enter      = exit.prev().prev();
    pearl::Event transfer   = exit.prev();

    pearl::timestamp_t time_to_next_event = exit.next()->get_time() +
                data->get_delta_t() - exit->get_time();
    
    if (time_to_next_event > 1.5e-6)
    {    
        time_to_next_event = 0;
        //log("time to next event: %e s\n", time_to_next_event);
    }
    
//    time_to_next_event = 1.e-6;

    // set delta to new offset 
    //double delta_t = data->get_delta_t();
    /*
    printf("Shifting events: old delta_t: %e new_delta_t: %e diff: %e\n",
            delta_t, delta_t - (exit->get_time() - enter->get_time()),
            exit->get_time() - enter->get_time());
    */
    data->set_delta_t(data->get_delta_t() - time_to_next_event -
            (exit->get_time() - enter->get_time()));
   
    // mark events as deleted
    data->set_action(silas::DELETE, enter);
    enter->set_time(-1);
    data->set_action(silas::DELETE, transfer);
    transfer->set_time(-1);
    data->set_action(silas::DELETE, exit);
    exit->set_time(-1);

    // for nonblocking requests, parse through trace and check
    // if test/wait calls can be deleted

    ident_t reqid = transfer->get_reqid();

    if (reqid == PEARL_NO_ID)
        return;

    pearl::Event ev_it = exit.next();
    event_t      creqt = transfer->is_typeof(MPI_RECV_REQUEST) ? MPI_RECV : MPI_SEND_COMPLETE;

    while (ev_it.is_valid()) {
        if (ev_it->is_typeof(MPI_REQUEST_TESTED) && ev_it->get_reqid() == reqid)
            check_call_removal(ev_it, data);

        if (ev_it->is_typeof(creqt) && ev_it->get_reqid() == reqid) {
            check_call_removal(ev_it, data);
            break;
        }
    }
}

void HypCutMessage::check_call_removal(const pearl::Event& completion, silas::CallbackData* data)
{
    pearl::Event enter = completion.enterptr();
    pearl::Event ev_it = enter.next();

    while (!ev_it->is_typeof(EXIT)) {
        if (data->get_action(ev_it) != silas::DELETE)
            return;

        ++ev_it;
    }

    // mark enter and exit event as deleted
    data->set_action(silas::DELETE, enter);
    enter->set_time(-1);
    data->set_action(silas::DELETE, ev_it);
    ev_it->set_time(-1);
}

void HypCutMessage::cb_noop(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
}

void HypCutMessage::cb_check_send(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);

    /* 
     * decide about deletion on sender side and notify the receiver
     */
    pearl::Event    send       = event.prev();
    pearl::MpiComm* comm       = send->get_comm();
    int             delete_msg = is_registered(send) ? 1 : 0;

    //printf("Send: delete_msg=%i dest=%i tag=%i\n", delete_msg,
    //comm->get_rank(send->get_dest()->get_process()), send->get_tag());
    MPI_Send(&delete_msg, 1, MPI_INT, 
             send->get_dest(),
             send->get_tag(), comm->get_comm());
    //printf("Post send\n");
    
    if (delete_msg)
        remove_message(event, data);
}


void HypCutMessage::cb_check_recv(
        const pearl::CallbackManager& cbmanager,
        int user_event, const pearl::Event& event, 
        pearl::CallbackData* cdata)
{
    //size_t startMemory,endMemory;
    //rts_getavailablememory(&startMemory);
    
    silas::CallbackData* data = static_cast<silas::CallbackData*>(cdata);

    /* 
     * Receive deletion flag from sender to enable this check on receiver
     * side.
     */
    pearl::Event recv = event.prev();

    if (recv->get_reqid() != PEARL_NO_ID)
        recv = recv.completion();

    pearl::MpiComm* comm = recv->get_comm();
    int             delete_msg = 1;

    //printf("Recv: delete_msg=%i src=%i tag=%i\n", delete_msg,
    //comm->get_rank(recv->get_source()->get_process()), recv->get_tag());
    MPI_Recv(&delete_msg, 1, MPI_INT, 
             recv->get_source(),
             recv->get_tag(), comm->get_comm(), MPI_STATUS_IGNORE);
    //printf("Post recv\n");

    if (delete_msg)
        remove_message(event, data);

    //rts_getavailablememory(&endMemory);
    //printf("Memory consumtion in recv: %i %i %i\n", startMemory,
    //endMemory, endMemory-startMemory);
}

void HypCutMessage::register_message_size(
        const std::string& rel, int rval)
{
    log("Registering message surgeon for size %s %i\n", rel.c_str(), rval);
    m_reg_msg_size[rel] = rval;
}

void HypCutMessage::register_message_tag(
        const std::string& rel, int rval)
{
    log("Registering message surgeon for tag %s %i\n", rel.c_str(), rval);
    m_reg_msg_tag[rel] = rval;
}

bool HypCutMessage::is_registered(const pearl::Event& event)
{
    bool ret = false;
   
    for (registration_t::iterator it = m_reg_msg_size.begin();
            it != m_reg_msg_size.end(); ++it)
    {
        if (cmp(it->first, event->get_sent(), it->second))
            ret = true;
    }
    
    for (registration_t::iterator it = m_reg_msg_tag.begin();
            it != m_reg_msg_tag.end(); ++it)
    {
        if (cmp(it->first, event->get_tag(), it->second))
            ret = true;
    }
    
    return ret;
}

bool HypCutMessage::cmp(const std::string& rel, int a, int b)
{
    if (rel == "!=") return ne(a,b);
    if (rel == "==") return eq(a,b);
    if (rel == ">=") return ge(a,b);
    if (rel == ">")  return gt(a,b);
    if (rel == "<=") return le(a,b);
    if (rel == "<")  return lt(a,b);

    return false;
}
