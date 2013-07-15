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

#ifndef EXP_SEVERITY_H
#define EXP_SEVERITY_H

/*
 *----------------------------------------------------------------------------
 *
 * class Severity
 *
 * Severity matrix used by class Pattern
 *
 *---------------------------------------------------------------------------- 
 */

#include <cstdlib>
#include <map>
#include <vector>

#include "CallTree.h"


template<class T> class Severity
{
 public:
  
  typedef typename std::map<long, T>::const_iterator MCI; 
  
  Severity(long nlocs) : matrix(nlocs) { }
  
  void set_val(long cnode_id, long loc_id, T value); 
  void add_val(long cnode_id, long loc_id, T value);  
  void sub_val(long cnode_id, long loc_id, T value);  
  
  T get(long cnode_id = -1, long loc_id = -1) const; 
  
  void exclusify(const CallTree& ctree, long cnode_id = -1);
  
 private:
  
  std::vector<std::map<long, T> > matrix;   
};



template<class T> void 
Severity<T>::set_val(long cnode_id, long loc_id, T value) 
{
  if ( loc_id < 0 || long(matrix.size()) < loc_id )
    {
      std::cerr << "EXPERT: Error in Severity<T>::set(long, long, T)." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  
  matrix[loc_id][cnode_id] = value; 
}

template<class T> void 
Severity<T>::add_val(long cnode_id, long loc_id, T value) 
{
  this->set_val(cnode_id, loc_id, this->get(cnode_id, loc_id) + value);
}

template<class T> void 
Severity<T>::sub_val(long cnode_id, long loc_id, T value) 
{
  this->set_val(cnode_id, loc_id, this->get(cnode_id, loc_id) - value);
}

template<class T> T 
Severity<T>::get(long cnode_id, long loc_id) const
{

  T result = 0;

  if ( cnode_id == -1 )
    {
      for ( size_t lid = 0 ; lid < matrix.size() ; lid++ )
	for (  MCI it = matrix[lid].begin(); it !=  matrix[lid].end() ; it++ )
	  result += this->get(it->first, lid);
    }
  else if ( loc_id == -1 )
    {
      for ( size_t lid = 0 ; lid < matrix.size() ; lid++ )
	result += this->get(cnode_id, lid);
    }
  else
    {
      if ( loc_id < 0 || long(matrix.size()) < loc_id )
	{
	  std::cerr << "EXPERT: Error in Severity<T>::get(long, long)." << std::endl;
	  std::exit(EXIT_FAILURE);
	}

      MCI it = matrix[loc_id].find(cnode_id);

      if ( it != matrix[loc_id].end() )
	result = it->second; 
      else
	result = 0;
    }
  return result;
}

template<class T> void 
Severity<T>::exclusify(const CallTree& ctree, long cnode_id) 
{
  std::vector<long> keyv;

  if ( cnode_id == -1 )
    {
      ctree.get_rootv(keyv);
      
      for ( size_t i = 0; i <  keyv.size(); i++ )
	this->exclusify(ctree, keyv[i]);
    }
  else
    {  
      ctree.get_childv(cnode_id, keyv);
            
      for ( size_t i = 0; i <  keyv.size(); i++ )
	{
	  long child_id = keyv[i];

	  for ( size_t loc_id = 0 ; loc_id < matrix.size() ; loc_id++ )
	    this->sub_val(cnode_id, loc_id, this->get(child_id, loc_id));
	  this->exclusify(ctree, child_id);
	}
    }
}

#endif






