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
* \file Metric.h
* \brief Defines a metric.
*/
#ifndef CUBE_METRIC_H
#define CUBE_METRIC_H   


#include <iosfwd>
#include <map>

#include "Matrix.h"
#include "Vertex.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Metric
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Cnode;
  class Thread;
  
 /**
* Metric has a treelike structure and is a kind of Vertex.
*/  
  class Metric : public Vertex { 

  public:
    
    Metric(const std::string& disp_name, 
           const std::string& uniq_name,
           const std::string& dtype, 
           const std::string& uom, 
           const std::string& val,
           const std::string& url, 
           const std::string& descr, 
           Metric* parent, uint32_t id=0)
      : Vertex(parent, id),
        disp_name(disp_name), 
        uniq_name(uniq_name),
        dtype(dtype), 
        uom(uom), 
        val(val), 
        url(url), 
        descr(descr)
        {}  
    
/**
* Returns the i-th "submetric"
*/    
    Metric* get_child(unsigned int i) const {
      return static_cast<Metric*>(Vertex::get_child(i));
    }
/**
* This metric is a submetric of ...?
*/
    Metric* get_parent()     const {
      return static_cast<Metric*>(Vertex::get_parent());
    }

/**
* Return metric value characteristics (such as "VOID") if optionally set,
  or value characteristics inherited from ancestors.
*/
    std::string get_val() const {
        Metric* parent = get_parent();
        if (val != "" || (parent == NULL)) return val;
        else return parent->get_val();
    }

    std::string get_disp_name() const { return disp_name; }             ///< Displayed name 
    std::string get_uniq_name() const { return uniq_name; }             ///< Unique name 
    std::string get_dtype()     const { return dtype; }                 ///< Type of the metric
    std::string get_uom()       const { return uom; }                   ///< Unit of measurement
    std::string get_descr()     const { return descr; }                 ///< Description of metric
    std::string get_url()       const { return url; }                   ///< URL for metric details

    void set_val(const std::string& value) { val = value; }             ///< set value type

    double get_sev(Cnode* cnode, Thread* thrd) ;
    void   set_sev(Cnode* cnode, Thread* thrd, double value);
    double sum();
    void clear();

    /// metric definition (writes child metrics recursively)
    void writeXML(std::ostream& out) const;
    /// severity
    void writeXML_data(std::ostream& out, const std::vector<Thread*>& thrdv) const;

  private:
    
    std::string disp_name;
    std::string uniq_name;
    std::string dtype;
    std::string uom;
    std::string val; // optional
    std::string url;
    std::string descr;
    
    Matrix sev_mat;
  };
  
  inline bool operator== (const Metric& a, const Metric& b) {
    return a.get_uniq_name() == b.get_uniq_name();
  }
}


#endif









































