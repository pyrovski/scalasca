/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SalsaCallback.h"

#include "MpiGroup.h"
#include "Region.h"
#include "Event.h"
#include "Location.h"
#include "Process.h"


#include <algorithm>

using namespace pearl;


// Borrowed from Scout
inline bool is_mpi_finalize(const Region* region)
{
  uint32_t mask = pearl::Region::CLASS_MPI
                  | pearl::Region::CAT_MPI_SETUP
                  | pearl::Region::TYPE_MPI_FINALIZE;

  return (region->get_class() & mask) == mask;
}

SalsaCallback::SalsaCallback(int r, int s, funcs f, modes m, MessageChecker mc)
		: result(s, 0), counter(s, 0), mc(mc) {
	rank = r;
	size = s;
	func = f;
	mode = m;
}

SalsaCallback::~SalsaCallback() {

}

void SalsaCallback::send(const CallbackManager& cbmanager, int user_event,
			const Event& event, CallbackData* data) {

  // Generate Message object
  MpiMessage* msg = new MpiMessage(*(event->get_comm()),512);
  // Register send event
  msg->put_event(event);
  // Send message
  msg->isend(event->get_dest(), event->get_tag());
  // Store Message object in vector
  m_pending.push_back(msg);
  m_requests.push_back(msg->get_request());

  // Check for completion of previous messages
  completion_check();
}
void SalsaCallback::recv(const CallbackManager& cbmanager, int user_event,
			const Event& event, CallbackData* data) {

  CallbackDataneu* dneu = (CallbackDataneu*) data;
  double tmp_val = 0.0;

  MpiMessage msg(*(event->get_comm()),512);
  msg.recv(event->get_source(), event->get_tag());

  RemoteEvent send_event = msg.get_event(*(dneu->defs));

  if(!mc.is_applicable(send_event)) return;

  uint64_t sent = (uint64_t) send_event->get_sent();
  timestamp_t send_time = send_event->get_time();
  timestamp_t recv_time = event->get_time();
  timestamp_t diff = recv_time - send_time;
  int id = send_event.get_location()->get_process()->get_id();

  switch(func) {
  case ::count:
	  result[id]++;
	  break;

  case length:
	  tmp_val = sent;
	  break;

  case duration:
	  tmp_val = diff;
	  break;

  case rate:
	  tmp_val = sent / diff;
	  break;

  default:
	  tmp_val = 0.0;
	  break;
  }

  if(func != ::count) {
	  switch(mode) {
	  case minimum:
		  if(result[id] == 0 || tmp_val < result[id]) {
			  result[id] = tmp_val;
		  }
		  break;

	  case maximum:
		  if(result[id] == 0 || tmp_val > result[id]) {
			  result[id] = tmp_val;
		  }
		  break;

	  case avg:
		  result[id] += tmp_val;
		  counter[id]++;
		  break;

	  case sum:
		  result[id] += tmp_val;
		  break;
	  }
  }

  // Check for completion of previous messages
  completion_check();
}

void SalsaCallback::enter(const CallbackManager& cbmanager, int user_event,
		const Event& event, CallbackData* data) {
	//Check whether entering MPI_Finalize
	if(is_mpi_finalize(event->get_region())) {
		completion_check();
	}
}

double* SalsaCallback::get_results() {

  if(mode==avg) {
    for(int i=0; i<size; i++) {
      if(counter[i]!=0) {
	      result[i]/=counter[i];
      }
    }
  }

  return &(result.front());
}

void SalsaCallback::completion_check() {
	// No pending requests? ==> continue
    if (m_pending.empty())
      return;

    // Check for completed messages
    int completed;
    int count = m_pending.size();

    m_indices.resize(count);
    m_statuses.resize(count);


    MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

    // Update array of pending messages
    for (int i = 0; i < completed; ++i) {
      int index = m_indices[i];

      delete m_pending[index];
      m_pending[index] = NULL;
    }
    m_pending.erase(std::remove(m_pending.begin(), m_pending.end(),
                           static_cast<MpiMessage*>(NULL)),
                    m_pending.end());
    m_requests.erase(std::remove(m_requests.begin(), m_requests.end(),
                            static_cast<MPI_Request>(MPI_REQUEST_NULL)),
                     m_requests.end());

}


