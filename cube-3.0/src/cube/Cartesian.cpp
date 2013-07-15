/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file Cartesian.cpp
* \brief Defines a class and methods to handle a topology of a application.
*/
/************************************************
             Cartesian.cpp

 ************************************************/

#include <iostream>

#include "Cartesian.h"
#include "cube_error.h"
#include "Machine.h"
#include "Node.h"
#include "Process.h"
#include "Sysres.h"
#include "Thread.h"

using namespace std;
using namespace cube;
/**
* Sets a thread "sys" at  coordinates "coordv"
*/
void Cartesian::def_coords(Sysres* sys, const vector<long>& coordv) {
  sys2coordv.insert( pair<Sysres*, vector<long> > (sys, coordv ) );
}

/**
* Sets the name of the topology.
*/
void Cartesian::set_name(const std::string& _name)
{
     name = _name;
}



bool Cartesian::set_namedims(const std::vector<std::string> _namedims) {
  if(_namedims.size()==ndims) {
    namedims=_namedims;
    return true;
  }
  else return false;
}

/**
* Gets a coordinates of given resource.
*/

const vector<long>& Cartesian::get_coordv(Sysres* sys) const {
  multimap<const Sysres*, vector<long> >::const_iterator itr;
  itr = sys2coordv.find(sys);
  if (itr != sys2coordv.end())
    return itr->second;
  else
    throw RuntimeError("Cartesian::get_coordv(): coordinates for the given resource not found!");
}



/**
 * Gets all coordinates of given resource.
 */

const vector<vector<long> >
Cartesian::get_all_coordv( Sysres* sys ) const
{
//     multimap<const Sysres*, vector<long> >::const_iterator itr;

    vector< vector < long > >                                                                                            to_return;

    multimap<const Sysres*, vector<long> >::const_iterator                                                               it;
    pair<multimap<const Sysres*, vector<long> >::const_iterator, multimap<const Sysres*, vector<long> >::const_iterator> ret;


    ret = sys2coordv.equal_range( sys );

    if ( ret.first  != ret.second )
    {
        for ( it = ret.first; it != ret.second; ++it )
        {
            to_return.push_back( ( *it ).second );
        }
        return to_return;
    }
    else
    {
        throw RuntimeError( "Cartesian::get_all_coordv(): coordinates for the given resource not found!" );
    }
}




Cartesian* Cartesian::clone(const std::vector<Thread*>& thrdv)
{
    Cartesian * copy = new Cartesian(ndims, dimv, periodv);
    copy->name=name;
    copy->namedims=namedims;
    for (multimap<const Sysres*, vector<long> >::iterator itr=sys2coordv.begin(); itr != sys2coordv.end(); itr++)
    {
        pair<const Sysres*, vector<long> > tmp;
        const Sysres & systemp = *((*itr).first);


        Sysres * tmp_sys = NULL;
        vector<Thread*>::const_iterator itr2=thrdv.begin();
        while ( tmp_sys == NULL &&  itr2<thrdv.end())
        {
            if ((*itr2)->get_id() == systemp.get_id())
            {
                tmp_sys = (*itr2);
            }
            itr2++;
        }

        if (tmp_sys == NULL)
            throw RuntimeError("Cloning topology is impossible, target threads are incompatble.");

        tmp.first = tmp_sys; 
        tmp.second  = (*itr).second;
        copy->sys2coordv.insert(tmp);
    }
    return copy;
}


/**
* Prints a xml-format of a cartesian topology.
*/
void Cartesian::writeXML(ostream& out) const {
  int sys_res = 0;
  string indent = "    ";
  if(name=="") {
	  out << indent << "  <cart ndims=\"" << ndims << "\">" << endl;
  } else {
	  out << indent << "  <cart name=\"" << name << "\" ndims=\"" << ndims << "\">" << endl;
  }

  if (ndims != dimv.size() || ndims != periodv.size()) {
    throw RuntimeError("Cartesian::writeXML(): inconsistent dimensions defined!");
  }

  if(namedims.size()>0) {
    for (unsigned int i = 0; i < ndims; i++) {
      out << indent << "    <dim name=\"" << namedims[i] << "\" size=\"" << dimv[i] << "\" periodic=\""
	  << (periodv[i] ? "true" : "false") << "\"/>" << endl;
    }

  } else {
    for (unsigned int i = 0; i < ndims; i++) {
      out << indent << "    <dim size=\"" << dimv[i] << "\" periodic=\""
	  << (periodv[i] ? "true" : "false") << "\"/>" << endl;
    }
  }

  std::multimap<const Sysres*, std::vector<long> >::const_iterator s;

  map<int, const Sysres*> id_order;
  s = sys2coordv.begin();
  const Sysres* sys = s->first;


  Sysres * tmp_sys = (Sysres *)sys;
  do
  {
    sys_res++;
    tmp_sys= tmp_sys->get_parent();
  }
  while (tmp_sys != 0);

  // sort sys2coordv and store it in id_order
  for (s = sys2coordv.begin(); s != sys2coordv.end(); s++) {


    sys = s->first;


    if (sys_res== 1) {
      Machine* mach = (Machine*)sys;
      id_order[mach->get_id()] = mach;
    }
    else if (sys_res == 2) {
      Node* node = (Node*)sys;
      id_order[node->get_id()] = node;
    }
    else if (sys_res == 3) {
      Process* proc = (Process*)sys;
      id_order[proc->get_id()] = proc;
    }
    else if (sys_res == 4) {
      Thread* thrd = (Thread*)sys;
      id_order[thrd->get_id()] = thrd;
    }
    else
      throw RuntimeError("Cartesian::writeXML() [1]: unknown system resource defined");
  }
  // output each coordinate
  map<int, const Sysres*>::iterator i;
  for (i = id_order.begin(); i != id_order.end(); i++) {
    s = sys2coordv.find(i->second);
    sys = s->first;
    vector<long> pt = s->second;

    if (sys_res == 1) {
      Machine* mach = (Machine*)sys;
      out << indent << "    <coord machId=\"" << mach->get_id() << "\">";
    }
    else if (sys_res == 2) {
      Node* node = (Node*)sys;
      out << indent << "    <coord nodeId=\"" << node->get_id() << "\">";
    }
    else if (sys_res == 3) {
      Process* proc = (Process*)sys;;
      out << indent << "    <coord procId=\"" << proc->get_id() << "\">";
    }
    else if (sys_res == 4) {
      Thread* thrd = (Thread*)sys;
      out << indent << "    <coord thrdId=\"" << thrd->get_id() << "\">";
    }
    else
      throw RuntimeError("Cartesian::writeXML() [2]: unknown system resource defined");
    for (unsigned int j = 0; j < pt.size(); j++) {
      if (j < pt.size()-1) out << pt[j] << " "; // no space at the end
      else out << pt[j];
    }
    out << "</coord>" << endl;
  }
  out << indent << "  </cart>" << endl;
}



/**
 Compare topologies step by step.
   <ol>
    <li> Equal number of dimensions?
    <li> Equal number of elements and periodicity in every dimension?
    <li> For every element of the first topology look for the same element in second one and compare coordinates.
   </ol>
 */
 bool Cartesian::operator==(const Cartesian& b) {
    if (this->get_ndims() != b.get_ndims()) return false; // check dimensionality
    for (signed i=0; i<this->get_ndims(); i++) // check property of every dimension
    {
        if ((this->get_dimv())[i] != (b.get_dimv())[i]) return false; // number of points is equal?
        if ((this->get_periodv())[i] != (b.get_periodv())[i]) return false;  // periodicity is equal?
    }
    std::multimap<const Sysres*, std::vector<long> >::const_iterator iter_a;
    std::multimap<const Sysres*, std::vector<long> >::const_iterator iter_b;
    for (iter_a = (this->get_cart_sys()).begin();
                        iter_a != (this->get_cart_sys()).end(); ++iter_a)  // through first topology
    {
    
       pair<
            multimap<const Sysres*, vector<long> >::const_iterator,
            multimap<const Sysres*, vector<long> >:: const_iterator> ret;

        ret = ( b.get_cart_sys() ).equal_range( iter_a->first );   // fins same Thread in second topology.

        if ( ret.first == ret.second )
        {
            return false;                                     // found?
        }
        bool all_not_equal = true;
        for ( iter_b = ret.first; iter_b != ret.second; ++iter_b )
        {
//         if ((iter_a->first)->get_id() != (iter_b->first)->get_id()) return false;
            for ( signed i = 0; i < this->get_ndims(); i++ ) // check coordinates
            {
                if ( iter_a->second[ i ] != iter_b->second[ i ] )
                {
                    all_not_equal = all_not_equal && false;
                }
            }
        }
        if ( all_not_equal ) // we didnt find any threads in target topology wih the same coordinates
        {
            return false;
        }
 }
  return true; 
}

