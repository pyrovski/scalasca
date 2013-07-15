/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "CollectiveQueue.h"

#include <algorithm>

#include "Communicator.h"
#include "Grid.h"

using namespace earl;
using namespace std;

CollectiveQueue::CollectiveQueue(const Grid* grid) : 
  rmacexitv(grid->get_ncoms()),
  completed(NULL)
{
  for ( size_t com_id = 0; com_id < rmacexitv.size(); com_id++ )
    for ( int rank = 0; rank < grid->get_com(com_id)->size(); rank++ )
      rmacexitv[com_id].push_back(list<Event>());
}

void   
CollectiveQueue::get_rmacoll(vector<Event>& out) 
{
  out.clear();
  if ( !completed )
    return;
  
    long com_id = completed->get_id();
    out.reserve(completed->size());
    for (int rank = 0; rank < completed->size(); rank++)
        out.push_back(rmacexitv[com_id][rank].front());

  sort(out.begin(), out.end());
} 

void   
CollectiveQueue::queue_rmacoll(Event cexit)
{
  bool is_completed = true;
  
  Communicator* com = cexit.get_com();

  long com_id = com->get_id();
  long rank   = com->get_rank(cexit.get_loc());

  rmacexitv[com_id][rank].push_back(cexit);
            
  for ( rank = 0; rank < com->size(); rank++ )
    if ( rmacexitv[com_id][rank].empty() )
      is_completed = false;
  
  if ( is_completed )
    completed = com;
}

void   
CollectiveQueue::dequeue_rmacoll()
{
  if ( completed )
    for (int rank = 0; rank < completed->size(); rank++)
      {
        long com_id = completed->get_id();
        
        rmacexitv[com_id][rank].pop_front() ;
      }
  completed = NULL;
}

