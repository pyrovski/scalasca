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
* \file Region.h
* \brief Provides a description of a region of the source code.
*/
#ifndef CUBE_REGION_H
#define CUBE_REGION_H


#include <functional>
#include <iosfwd>

#include "Vertex.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Region
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Cnode;
  class Canonizer;

/**
* Functions contain Loops, Loops contains blocks and so on. Heterogen treelike structure. 
*/ 
  class Region : public Vertex { 
  
  public:
/**
* Regions is defined by name, startline and endline ogf the source code, url as online help
*/
    Region(const std::string& name, int begln, int endln, const std::string& url, 
           const std::string& descr, const std::string& mod, uint32_t id=0)
      : Vertex(id), name(name), begln(begln), endln(endln),
        url(url), descr(descr), mod(mod) {}

    void set_url(std::string newUrl) { url = newUrl; }

    std::string get_name()  const { return name; }
    std::string get_url()   const { return url; }
    std::string get_descr() const { return descr; }
    std::string get_mod()   const { return mod; }
    int get_begn_ln()       const { return begln; }
    int get_end_ln()        const { return endln; }
    int  num_children()     const { return cnodev.size(); }
    void add_cnode(Cnode* cnode);

    const std::vector<Cnode*>& get_cnodev() const { return cnodev; }
    const std::vector<Cnode*>& get_excl_cnodev() const { return excl_cnodev; }
    const std::vector<Cnode*>& get_incl_cnodev() const { return incl_cnodev; }

    void writeXML(std::ostream& out) const;
  
  private:

    friend class Cnode; ///< Cnodes are friends, They may call private members. 
    friend class Canonizer; ///< Canonizer is a friend, it may call private members.
    
    void add_incl_cnode(Cnode* cnode) { incl_cnodev.push_back(cnode); }
    void add_excl_cnode(Cnode* cnode) { excl_cnodev.push_back(cnode); }

    std::string name;
    int begln;
    int endln;
    std::string url;
    std::string descr;
    std::string mod;
    std::vector<Cnode*> cnodev;

    std::vector<Cnode*> excl_cnodev; ///< call paths calling this region    
    std::vector<Cnode*> incl_cnodev; ///< call paths calling this region without recursive calls
  };

  bool operator== (const Region& a, const Region& b);
  inline bool operator<  (const Region& a, const Region& b) {
    return a.get_name() < b.get_name();
  }

  inline bool operator== (const Region& a, const std::string b) {
    return (a.get_name() == b ? true : false);
  }

  template<class T>
    class GenLess : public std::binary_function<T, T, bool> {
      public :
        bool operator() (const T a, const T b) const {
          return (*a < *b);
        }
    };
}

#endif
