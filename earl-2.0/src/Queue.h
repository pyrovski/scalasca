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

#ifndef EARL_QUEUE_H
#define EARL_QUEUE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Queue
 *
 * Queue container class for queue with limited length and random access
 *
 *---------------------------------------------------------------------------- 
 */

//#include <iostream>
#include <vector>

//using namespace std;

#include "Error.h"

namespace earl
{
  template<class T> class Queue
  {
  public:
      
    Queue(size_t max_len);
      
    size_t size();
    bool   empty();     
    bool   full();     
    T      front();
    T      back();
    T      operator[](size_t n);
    void   pop_front();
    void   push_back(T val);
    void   clear();
      
  private:
      
    // maps list index onto vector index
    int vec_idx(size_t list_idx);

    void print_status();
      
    std::vector<T> listv;
    int first;
    int last;
    int length;
  };
  
  
  template<class T>
  Queue<T>::Queue(size_t max_len) : listv(max_len)
  {
    first  = -1;
    last   = -1;
    length = 0;
  }
  
  template<class T> size_t 
  Queue<T>::size() 
  {
    return length;
  }
  
  template<class T> bool 
  Queue<T>::empty() 
  {
    return length == 0;
  }
  
  template<class T> bool 
  Queue<T>::full() 
  {
    return length == int(listv.size());
  }
  
  template<class T> T      
  Queue<T>::front() 
  {
    if ( this->size() == 0 )
      throw FatalError("Error in Queue<T>::front().");
  
    return listv[first];
  }

  template<class T> T      
  Queue<T>::back() 
  {
    if ( this->size() == 0 )
      throw FatalError("Error in Queue<T>::back().");
  
    return listv[last];
  }

  template<class T> T      
  Queue<T>::operator[](size_t n) 
  {
    if ( n > this->size() - 1 )
      throw FatalError("Error in Queue<T>::operator[](size_t).");

    return listv[vec_idx(n)];
  }

  template<class T> void   
  Queue<T>::pop_front() 
  {
    if ( this->empty() )
      throw FatalError("Error in Queue<T>::pop_front().");

    first = (first + 1) % listv.size();
    length--;
  }

  template<class T> void   
  Queue<T>::push_back(T val) 
  {
    if ( this->size() == listv.size() )
      throw FatalError("Error in Queue<T>::push_back(T).");
  
    if ( this->empty() )
      {
	first = 0;
	last = 0;
      }
    else
      {
	last = (last + 1) % listv.size();
      }
    listv[last] = val;
    length++;
  }

  template<class T> void   
  Queue<T>::clear() 
  {
    first  = 0;
    last   = 0;
    length = 0; 
  }

  template<class T> int 
  Queue<T>::vec_idx(size_t list_idx) 
  {
    size_t max_vdx =  listv.size() - 1;

    if ( list_idx > max_vdx )
      throw FatalError("Error in Queue<T>::vec_idx(size_t).");
    
    size_t vir_idx =  first + list_idx;

    return vir_idx % listv.size(); 
  }
}

#endif






