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
	\file Metric.cpp
	\brief Defines a methods of a Metric

********************************************/


#include <iostream>

#include "Metric.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;


void Metric::set_sev(Cnode* cnode, Thread* thrd, double value) {
  
  sev_mat.set_elem(cnode, thrd, value);

}

double Metric::get_sev(Cnode* cnode, Thread* thrd)  {
  
  return sev_mat.get_elem(cnode, thrd);
  
}

double Metric::sum(){
  return sev_mat.sum();
}

void Metric::clear(){
  sev_mat.clear();
}

void Metric::writeXML_data(ostream& out, const std::vector<Thread*>& thrdv) const {

  if (get_val() == "VOID" || sev_mat.empty())
    return;

  out << "    <matrix metricId=\"" << get_id() << "\">" << endl;
  sev_mat.writeXML(out, thrdv);
  out << "    </matrix>" << endl;
}

void Metric::writeXML(ostream& out) const {

  out << indent() << "    <metric id=\"" << get_id() << "\">" << endl;
  out << indent() << "      <disp_name>" << escapeToXML(get_disp_name()) << "</disp_name>" << endl;
  out << indent() << "      <uniq_name>" << escapeToXML(get_uniq_name()) << "</uniq_name>" << endl;
  out << indent() << "      <dtype>" << escapeToXML(get_dtype()) << "</dtype>" << endl;
  out << indent() << "      <uom>"  << escapeToXML(get_uom()) << "</uom>" << endl;
  if (!get_val().empty()) {
    out << indent() << "      <val>" << escapeToXML(get_val()) << "</val>\n";
  }
  out << indent() << "      <url>" << escapeToXML(get_url()) << "</url>" << endl;
  out << indent() << "      <descr>" << escapeToXML(get_descr()) << "</descr>" << endl;

  for( unsigned int i = 0; i < num_children(); i++ ) {
    const Metric* p = get_child(i);
    p->writeXML(out);
  }

  out << indent() << "    </metric>" << endl;

}
