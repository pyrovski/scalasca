/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2006-2013                                                **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stack>

#include <elg_error.h>
#include <elg_readcb.h>
#include <epk_archive.h>

#include "Calltree.h"
#include "CNode.h"
#include "Enter_rep.h"
#include "Error.h"
#include "GlobalDefs.h"
#include "LocalTrace.h"
#include "MappingTable.h"
#include "Region.h"
#include "readcb.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class LocalTrace
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

LocalTrace::LocalTrace(const GlobalDefs& defs,
                       const string&     dirname,
                       uint32_t          rank,
                       uint32_t          thread_id,
                       int32_t           sion_id)
  : m_defs(defs),
    m_omp_parallel(0),
    m_mpi_regions(0),
    m_location(NULL)
{
  // Create mapping table
  MappingTable* table = new MappingTable(defs, dirname, rank);

  // Determine file name
  string filename;
  if (sion_id >= 0) {
    ostringstream sion_prefix;
    sion_prefix << "SID=" << setw(6) << setfill('0') << sion_id << ":";
    filename = sion_prefix.str() + epk_archive_get_name();
  } else {
    char* tmp_str = epk_archive_rankname(EPK_TYPE_ELG, dirname.c_str(), rank);
    filename = tmp_str;
    free(tmp_str);
  }

  // Open local event trace file
  ElgRCB* file = elg_read_open(filename.c_str());
  if (!file)
    throw RuntimeError("Cannot open EPILOG event trace file \"" +
                       filename + "\".");

  // Read data
  TraceCbData data(defs, *this, *table, elg_read_version(file), thread_id, sion_id);
  while (elg_read_next_event(file, &data))
  {
    // Check for errors
    if (!data.m_message.empty())
      throw Error(data.m_message);
  }
  elg_read_close(file);

  // Free mapping table
  delete table;

  // Retrieve location
  m_location = defs.get_location(data.m_location);

  // Ensure consistency of ENTER_TRACING/EXIT_TRACING records, and
  // count number of OpenMP parallel and MPI regions
  preprocess();
}


LocalTrace::~LocalTrace()
{
  /* Delete events */
  container_type::reverse_iterator it = m_events.rbegin();
  while (it != m_events.rend()) {
    delete *it;
    ++it;
  }
}


//--- Iterator handling -----------------------------------------------------

LocalTrace::iterator LocalTrace::begin() const
{
  return Event(this);
}

LocalTrace::iterator LocalTrace::end() const
{
  return Event(this, true);
}


LocalTrace::reverse_iterator LocalTrace::rbegin() const
{
  return reverse_iterator(end());
}


LocalTrace::reverse_iterator LocalTrace::rend() const
{
  return reverse_iterator(begin());
}


//--- Inserting & removing elements -----------------------------------------

LocalTrace::iterator LocalTrace::remove_if(bool (*predicate)(const Event_rep* event))
{
  uint32_t index = std::remove_if(m_events.begin(),
                                  m_events.end(),
                                  predicate) - m_events.begin();

  return Event(this, index);
}


LocalTrace::iterator LocalTrace::insert(LocalTrace::iterator pos,
                                        Event_rep*           event)
{
  m_events.insert(m_events.begin() + pos.m_index, event);
  m_defs.get_calltree()->set_modified();

  return pos;
}


LocalTrace::iterator LocalTrace::erase(LocalTrace::iterator pos)
{
  assert(pos.m_index < size());

  m_events.erase(m_events.begin() + pos.m_index);
  m_defs.get_calltree()->set_modified();

  return pos;
}

LocalTrace::iterator LocalTrace::erase(LocalTrace::iterator begin,
                                       LocalTrace::iterator end)
{
  assert(begin.m_index < size());
  //assert(end.m_index < size());
  //assert(end.m_index >= 0);

  m_events.erase(m_events.begin() + begin.m_index, 
                 m_events.begin() + end.m_index);
  m_defs.get_calltree()->set_modified();

  return begin;
}

LocalTrace::iterator LocalTrace::replace(LocalTrace::iterator pos, Event_rep* event)
{
  m_events[pos.m_index] = event;
  m_defs.get_calltree()->set_modified();

  return pos;
}

LocalTrace::iterator LocalTrace::swap(const LocalTrace::iterator& a, const LocalTrace::iterator& b)
{
  std::iter_swap(m_events.begin()+a.m_index, m_events.begin()+b.m_index);
  m_defs.get_calltree()->set_modified();

  return a;
}


//--- Get trace information -------------------------------------------------

uint32_t LocalTrace::size() const
{
  return m_events.size();
}


uint32_t LocalTrace::num_events() const
{
  return m_events.size();
}


uint32_t LocalTrace::num_omp_parallel() const
{
  return m_omp_parallel;
}


uint32_t LocalTrace::num_mpi_regions() const
{
  return m_mpi_regions;
}


Location* LocalTrace::get_location() const
{
  return m_location;
}


const GlobalDefs& LocalTrace::get_definitions() const
{
  return m_defs;
}


//--- Store new events ------------------------------------------------------

void LocalTrace::add_event(Event_rep* event)
{
  m_events.push_back(event);
}


//--- Private methods -------------------------------------------------------

Event_rep* LocalTrace::operator[](uint32_t index) const
{
  return m_events[index];
}


void LocalTrace::preprocess()
{
  const uint32_t omp_mask = Region::CLASS_OMP | Region::CAT_OMP_PARALLEL;
  const uint32_t mpi_mask = Region::CLASS_MPI;

  // Ensure consistency for EXIT_TRACING/EXIT event combinations
  //   NOTE: This consistency check should eventually be carried out by the
  //         measurement system.
  uint32_t count = m_events.size();
  for (uint32_t i = 1; i < count; ++i) {
    if (m_events[i-1]->is_typeof(EXIT) && m_events[i]->is_typeof(EXIT) &&
        m_events[i-1]->get_time()      >  m_events[i]->get_time()) {

      std::swap(m_events[i-1], m_events[i]);

      for (uint32_t depth = 1, jj = i-2; depth; --jj) {
        if (m_events[jj]->is_typeof(EXIT))
          ++depth;
        if (m_events[jj]->is_typeof(ENTER))
          --depth;

        std::swap(m_events[jj], m_events[jj+1]);
      }
    }

    // When entering an OpenMP parallel or MPI region, increment counter
    if (m_events[i-1]->is_typeof(ENTER)) {
      Enter_rep* enter     = static_cast<Enter_rep*>(m_events[i-1]);
      uint32_t   reg_class = enter->get_regionEntered()->get_class();

      if ((reg_class & omp_mask) == omp_mask)
        m_omp_parallel++;

      if ((reg_class & mpi_mask) == mpi_mask)
        m_mpi_regions++;
    }
  }

  // Precompute next/prev request offsets
  std::map<ident_t, uint32_t> requestmap;

  for (uint32_t i = 1; i < count; ++i) {
    Event_rep* ev = m_events[i];

    if (ev->is_typeof(MPI_P2P) || ev->is_typeof(MPI_REQUEST)) {
      ident_t reqid = ev->get_reqid();

      if (reqid == PEARL_NO_ID)
        continue;

      std::map<ident_t, uint32_t>::iterator rmapit = requestmap.find(reqid);
      if (rmapit != requestmap.end()) {
        m_events[rmapit->second]->set_next_reqoffs(i - rmapit->second);
        ev                      ->set_prev_reqoffs(i - rmapit->second);

        if (ev->is_typeof(MPI_REQUEST_TESTED))
          rmapit->second = i;
        else
          requestmap.erase(rmapit);
      } else {
        requestmap.insert(std::make_pair(reqid, i));
      }
    }
  }
}
