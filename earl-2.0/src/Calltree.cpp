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

#include "Calltree.h"

#include <algorithm>
//#include <iostream>

#include "Error.h"

using namespace earl;
using namespace std;

Event
Calltree::get_cnodeptr(Event cedgeptr, Region* region, Callsite* csite) 
{
  if ( cedgeptr.null() )
    {
      for ( size_t i = 0; i < rootv.size(); i++ )
	if ( region == nodem[rootv[i]].get_reg() )
	  return nodem[rootv[i]];
    }
  else
    {
      for ( size_t i = 0; i < childvm[cedgeptr.get_pos()].size(); i++ )
	if ( region == nodem[childvm[cedgeptr.get_pos()][i]].get_reg() && 
	     csite  == nodem[childvm[cedgeptr.get_pos()][i]].get_csite() )
	    return nodem[childvm[cedgeptr.get_pos()][i]];
    }
  return NULL;
}

void
Calltree::add_node(Event enter) 
{
  long pos = enter.get_pos();
  
  // add new node 
  if ( enter.get_cnodeptr() == enter && last < pos)
    {    
      nodem[pos] = enter;
      
      if ( enter.get_cedgeptr().null() )
 	rootv.push_back(pos);
      else
	childvm[enter.get_cedgeptr().get_pos()].push_back(pos);      
      
      last = pos;
      nvisitsm[pos] = 1;
    }
  else
    {
      nvisitsm[enter.get_cnodeptr().get_pos()]++;
    }
}

void       
Calltree::calltree(vector<Event>& out) 
{
  map<long, Event>::iterator it = nodem.begin();

  while ( it != nodem.end() )
    {
      out.push_back(it->second);
      it++;
    }
}

void       
Calltree::ctroots(vector<Event>& out) 
{
  for ( size_t i = 0; i < rootv.size(); i++ )
    out.push_back(nodem[rootv[i]]);
}

void       
Calltree::callpath(vector<Event>& out, long cnode) 
{
  if ( !iscnode(cnode) )
    throw RuntimeError("Error in Calltree::callpath(vector<Event>&, long).");

  Event enter = nodem[cnode];

  while ( !enter.null() )
    {
      out.push_back(enter);
      enter = enter.get_cedgeptr();
    }
  reverse(out.begin(), out.end());
}

void       
Calltree::ctchildren(vector<Event>& out, long cnode) 
{
  if ( !iscnode(cnode) )
    throw RuntimeError("Error in Calltree::children(vector<Event, long).");     

  if ( childvm.find(cnode) ==  childvm.end() )
    return;

  for ( size_t i = 0; i < childvm[cnode].size(); i++ )
    out.push_back(nodem[childvm[cnode][i]]);
}

long       
Calltree::ctvisits(long cnode) 
{
  if ( !iscnode(cnode) )
    throw RuntimeError("Error in Calltree::nvisits(long)."); 

  return nvisitsm[cnode];
}
  

bool      
Calltree::iscnode(long cnode) 
{
  map<long, Event>::const_iterator it = nodem.find(cnode);
  
  if (it == nodem.end())
    return false;
  else
    return true;
}












