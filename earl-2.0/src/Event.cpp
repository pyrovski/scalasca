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

#include "Event.h"

#include "Error.h"
#include "Event_rep.h"

using namespace earl;
using namespace std;

Event::Event() : event(NULL)           
{}

Event::Event(Event_rep* event) : event(event)           
{ 
  if ( event != NULL ) event->add_ref(); 
}

Event::Event(const Event& rhs) : event(rhs.event)  
{ 
  if ( event != NULL )  
    event->add_ref(); 
}
 
Event::~Event()
{
  if ( event != NULL && event->rem_ref() == 0 )
    delete event;
}

Event& 
Event::operator=(const Event& rhs)
{  
  if (this == &rhs) 
    return *this;
  
  if ( event != NULL && event->rem_ref() == 0 )
    delete event;
  event = rhs.event;
  if ( event != NULL ) 
    event->add_ref();
  return *this;
}

bool           
Event::null() const 
{ 
  return ( event == NULL ); 
}

Event_rep*         
Event::operator->()
{
  if (!event)
    throw RuntimeError("Null pointer exception");
  return event; 
}

const Event_rep*         
Event::operator->() const 
{ 
  if (!event)
    throw RuntimeError("Null pointer exception (const)");
  return event; 
}

Event_rep*         
Event::operator*() const 
{ 
  return event; 
}

bool           
Event::is_type(etype type) const
{
  if ( event == NULL )
    throw RuntimeError("Error in Event::is_type().");

  return event->is_type(type); 
}

long           
Event::get_pos() const 
{
  //if ( event == NULL )
  //  throw RuntimeError("Error in Event::get_pos().");

  return event->get_pos(); 
}

Location*      
Event::get_loc() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_loc().");

  return event->get_loc(); 
}

double         
Event::get_time() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_time().");

  return event->get_time(); 
}

Event         
Event::get_enterptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_enterptr().");

  return event->get_enterptr(); 
}

etype    
Event::get_type() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_type().");

  return event->get_type(); 
}

string
Event::get_typestr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_typestr().");

  return event->get_typestr(); 
}

Event         
Event::get_cnodeptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_cnodeptr().");

  return event->get_cnodeptr(); 
}

Event         
Event::get_cedgeptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_cedgeptr().");

  return event->get_cedgeptr(); 
}

Region*        
Event::get_reg() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_reg().");

  return event->get_reg(); 
}

Callsite*      
Event::get_csite() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_csite().");

  return event->get_csite(); 
}

Event         
Event::get_sendptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_sendptr().");

  return event->get_sendptr(); 
}

Event         
Event::get_lockptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_lockptr().");

  return event->get_lockptr(); 
}

Event         
Event::get_forkptr() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_forkptr().");

  return event->get_forkptr(); 
}

Location*      
Event::get_src() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_src().");

  return event->get_src(); 
}

Location*      
Event::get_dest() const 
{
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_dest().");

 return event->get_dest(); 
}

Communicator*  
Event::get_com() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_com().");

  return event->get_com(); 
}

MPIWindow*  
Event::get_win() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_win().");

  return event->get_win(); 
}

long           
Event::get_tag() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_tag().");

  return event->get_tag(); 
}

long           
Event::get_length() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_length().");

  return event->get_length(); 
}

Location*      
Event::get_root() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_root().");

  return event->get_root(); 
}

long           
Event::get_sent() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_sent().");

  return event->get_sent(); 
}

long           
Event::get_recvd() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_recvd().");

  return event->get_recvd(); 
}

long           
Event::get_lock_id() const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_lock_id().");

  return event->get_lock_id(); 
}

long           
Event::get_nmets() const 
{  
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_nmets().");

  return event->get_nmets(); 
}

long long      
Event::get_metval(long i) const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_metval(long).");

  return event->get_metval(i); 
}

std::string    
Event::get_metname(long i) const 
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::get_metname(long).");

  return event->get_metname(i); 
}

void 
Event::trans(State& state)
{ 
  if ( event == NULL )
    throw RuntimeError("Error in Event::trans(State&).");

  event->trans(state); 
}

bool earl::operator==(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator==(const Event&, const Event&).");

  return lhs.get_pos() == rhs.get_pos();
}

bool earl::operator!=(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator!=(const Event&, const Event&).");

  return lhs.get_pos() != rhs.get_pos();
}

bool earl::operator<(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator<(const Event&, const Event&).");
  
  return lhs.get_pos() < rhs.get_pos();
}

bool earl::operator>(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator>(const Event&, const Event&).");
  
  return lhs.get_pos() > rhs.get_pos();
}

bool earl::operator<=(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator<=(const Event&, const Event&).");
  
  return lhs.get_pos() <= rhs.get_pos();
}

bool earl::operator>=(const Event& lhs, const Event& rhs)
{
  if ( lhs.null() || rhs.null() )
    throw RuntimeError("Error in operator>=(const Event&, const Event&).");
  
  return lhs.get_pos() >= rhs.get_pos();
}

