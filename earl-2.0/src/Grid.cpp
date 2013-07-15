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

#include "elg_defs.h"

#include "Grid.h"
#include "Error.h"

using namespace earl;
using namespace std;

Grid::~Grid()
{
  map<long, Machine*>::const_iterator mit = machm.begin();
  while ( mit != machm.end() )
    {
      delete mit->second;
      mit++;
    }
  map<pair<long, long>, Node*>::const_iterator sit = nodem.begin();
  while (sit != nodem.end())
    {
      delete sit->second;
      sit++;
    }
  map<long, Process*>::const_iterator pit = procm.begin();
  while (pit != procm.end())
    {
      delete pit->second;
      pit++;
    }
  map<pair<long, long>, Thread*>::const_iterator tit = thrdm.begin();
  while (tit != thrdm.end())
    {
      delete tit->second;
      tit++;
    }
  map<long, Location*>::const_iterator lit = locm.begin();
  while (lit != locm.end())
    {
      delete lit->second;
      lit++;
    }
  map<long, Communicator*>::const_iterator cit = comm.begin();
  while (cit != comm.end())
    {
      delete cit->second;
      cit++;
    }
  map<long, Cartesian*>::const_iterator topit = cartm.begin();
  while ( topit != cartm.end() )
    {
      delete topit->second;
      topit++;
    }
}

Machine*      
Grid::get_mach(long mach_id) const 
{ 
  map<long, Machine*>::const_iterator it = machm.find(mach_id);
  
  if (it == machm.end())
    throw RuntimeError("Error in Grid::get_mach(long).");
  else
    return it->second;
}
  
Node*          
Grid::get_node(long mach_id, long node_id) const 
{ 
  map<pair<long, long>, Node*>::const_iterator it = nodem.find(pair<long, long>(mach_id, node_id));

  if (it == nodem.end())
    throw RuntimeError("Error in Grid::get_node(long, long).");
  else
    return it->second;
}
  
Process*      
Grid::get_proc(long proc_id) const 
{ 
  map<long, Process*>::const_iterator it = procm.find(proc_id);

  if (it == procm.end())
    throw RuntimeError("Error in Grid::get_proc(long).");
  else
    return it->second;
}
  
Thread*       
Grid::get_thrd(long proc_id, long thrd_id) const 
{ 
  map<pair<long, long>, Thread*>::const_iterator it = thrdm.find(pair<long, long>(proc_id, thrd_id));

  if (it == thrdm.end())
    throw RuntimeError("Error in Grid::get_thrd(long, long).");
  else
    return it->second;
}

Location*     
Grid::get_loc(long loc_id) const 
{ 
  map<long, Location*>::const_iterator it = locm.find(loc_id);

  if (it == locm.end())
    throw RuntimeError("Error in Grid::get_loc(long).");
  else
    return it->second;
}

Communicator* 
Grid::get_com(long com_id) const 
{ 
  map<long, Communicator*>::const_iterator it = comm.find(com_id);

  if (it == comm.end())
    throw RuntimeError("Error in Grid::get_com(long).");
  else
    return it->second;
}

MPIWindow*       
Grid::get_win(long win_id) const 
{ 
  map<long, MPIWindow*>::const_iterator it = winm.find(win_id);

  if (it == winm.end())
    throw RuntimeError("Error in SeqTrace::get_win().");
  else
    return it->second;
}

Cartesian*
Grid::get_cart(long cart_id) const
{
  map<long, Cartesian*>::const_iterator it = cartm.find(cart_id);

  if ( it == cartm.end() ) 
    throw RuntimeError("Error in Grid::get_cart(long).");
  else
    return it->second;
}

Machine* 
Grid::create_mach(long id, string name)    
{ 
  Machine* mach = new Machine(id, name); 

  machm[id] = mach;
  return mach;
}

Node* 
Grid::create_node(long mach_id, long node_id, string name, long cpuc, double cr)       
{ 
  Node* node    = new Node(mach_id, node_id, name, cpuc, cr);
  Machine* mach = this->get_mach(mach_id); 

  nodem[pair<long, long>(mach_id, node_id)] = node;

  node->set_mach(mach);
  return node;
}

Process* 
Grid::create_proc(long id, string name)       
{ 
  Process* proc;
  if ( procm.find(id) == procm.end() )
    {
      proc =  new Process(id, name);
      procm[id] = proc;
    }
  else
    {
      proc = this->get_proc(id);
      proc->set_name(name);
    }
  return proc;
}

Thread* 
Grid::create_thrd(long proc_id, long thrd_id, string name) 
{ 
  Thread* thrd; 
  pair<long, long> key(proc_id, thrd_id); 

  if ( thrdm.find(key) == thrdm.end() )
    {
      thrd =  new Thread(proc_id, thrd_id, name);
      thrdm[key] = thrd;
    }
  else
    {
      thrd = this->get_thrd(proc_id, thrd_id);
      thrd->set_name(name);
    }
  return thrd;
}

Location* 
Grid::create_loc(long id, long mach_id, long node_id, long proc_id, long thrd_id)     
{ 
  Machine* mach = this->get_mach(mach_id);
  Node*    node = this->get_node(mach_id, node_id);

  Process* proc; 
  try {
    proc = this->get_proc(proc_id);
  } catch ( RuntimeError ) {
    proc = this->create_proc(proc_id, "");
  }

  Thread* thrd; 
  try {
    thrd = this->get_thrd(proc_id, thrd_id);
  } catch ( RuntimeError ) {
    thrd = this->create_thrd(proc_id, thrd_id, "");
  }

  Location* loc = new Location(id, mach, node, proc, thrd);

  locm[id] = loc; 
  
  mach->add_node(node);
  node->add_proc(proc);
  proc->add_thrd(thrd);
  proc->set_node(node);
  thrd->set_proc(proc);
  thrd->set_loc(loc);

  return loc;
}

Communicator* 
Grid::create_com(long id, vector<long> proc_idv)     
{ 
  vector<Process*> procv;

  for ( size_t i = 0; i < proc_idv.size(); i++ )
    {
      Process* proc;
      long proc_id = proc_idv[i];

      try {
	proc = this->get_proc(proc_id);
      } catch ( RuntimeError ) {
	proc =  this->create_proc(proc_id, "");
      }

      procv.push_back(proc);
    }
  Communicator* com= new Communicator(id, procv); 
  
  comm[id] = com;
  return com;
}

MPIWindow* 
Grid::create_win(long wid, long cid)     
{ 
  MPIWindow* win;
  win = new earl::MPIWindow(wid, get_com(cid));
  winm[wid] = win;
  return win;
}

Cartesian*
Grid::create_cart(long id, 
		  long cid,
		  long ndims, 
		  std::vector<long> dimv, 
		  std::vector<bool> periodv)
{ 
  Communicator* com;

  if ( unsigned(cid) == ELG_NO_ID ) 
    com = NULL;
  else
    com = comm[cid];
    
  Cartesian* cart = new Cartesian(id, com, ndims, dimv, periodv); 
  
  cartm[id] = cart;
  
  return cart;
}
