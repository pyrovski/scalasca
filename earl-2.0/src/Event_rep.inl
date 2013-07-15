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

// inline definitions

inline long 
Event_rep::add_ref()
{ 
  if ( ++refc == 1 && buffer != NULL ) 
    buffer->insert(this);
  return refc; 
}

inline long 
Event_rep::rem_ref() 
{ 
  if ( --refc == 0 && buffer != NULL ) 
    buffer->remove(this->get_pos()); 
  return refc; 
}




