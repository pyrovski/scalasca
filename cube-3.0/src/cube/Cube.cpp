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
* \file Cube.cpp
* \brief Defines methods of teh class cube and IO-interface.
*
*/

#include <cstdlib>
#include <iostream>
#ifndef CUBE_COMPRESSED
#  include <fstream>
#else
#  include "zfstream.h"
#endif

#include "Cartesian.h"
#include "Cnode.h"
#include "Cube.h"
#include "cube_error.h"
#include "Machine.h"
#include "Matrix.h"
#include "Metric.h"
#include "Node.h"
#include "Process.h"
#include "Region.h"
#include "Row.h"
#include "Sysres.h"
#include "Thread.h"
#include "Driver.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;
Cube::Cube()
  : cur_cnode_id(0),
    cur_metric_id(0),
    cur_region_id(0),
    cur_machine_id(0),
    cur_node_id(0),
    cur_process_id(0),
    cur_thread_id(0)
{
}

Cube::Cube(Cube const& copy)
  : cur_cnode_id(0),
    cur_metric_id(0),
    cur_region_id(0),
    cur_machine_id(0),
    cur_node_id(0),
    cur_process_id(0),
    cur_thread_id(0)
{
	map<Metric*, Metric* > metric_map_copy2new;
	map<Region*, Region* > region_map_copy2new;
	map<Cnode*,  Cnode*  > cnode_map_copy2new;
	map<Machine*,Machine*> machine_map_copy2new;
	map<Node*,   Node*   > node_map_copy2new;
	map<Process*,Process*> process_map_copy2new;
	map<Thread*, Thread* > thread_map_copy2new;


	attrs=copy.attrs;
	mirror_urlv=copy.mirror_urlv;
	// Copy metrics vector.
  for (vector<Metric*>::const_iterator m = copy.metv.begin(); m != copy.metv.end(); ++m)
	{
		metric_map_copy2new[*m] =
			def_met((*m)->get_disp_name(), (*m)->get_uniq_name(), (*m)->get_dtype(),
				(*m)->get_uom(), (*m)->get_val(), (*m)->get_url(), (*m)->get_descr(),
				((*m)->get_parent() == NULL ? NULL : metric_map_copy2new[(*m)->get_parent()]),
				(*m)->get_id());
	}

	// Copy region vector.
  for (vector<Region*>::const_iterator r = copy.regv.begin(); r != copy.regv.end(); ++r)
	{
		region_map_copy2new[*r] =
			def_region((*r)->get_name(), (*r)->get_begn_ln(), (*r)->get_end_ln(),
				(*r)->get_url(), (*r)->get_descr(), (*r)->get_mod(), (*r)->get_id());
	}

	// Copy callnode vector
	for (vector<Cnode*>::const_iterator c = copy.cnodev.begin(); c != copy.cnodev.end(); ++c)
	{
		cnode_map_copy2new[*c] =
			def_cnode(region_map_copy2new[(*c)->get_callee()], (*c)->get_mod(),
				(*c)->get_line(),
				((*c)->get_parent() == NULL ? NULL : cnode_map_copy2new[(*c)->get_parent()]),
				(*c)->get_id());
	}

	// Copy machines vector
	for (vector<Machine*>::const_iterator m = copy.machv.begin(); m != copy.machv.end(); ++m)
	{
		machine_map_copy2new[*m] =
			def_mach((*m)->get_name(), (*m)->get_desc(), (*m)->get_id());
	}

	// Copy nodes vector
	for (vector<Node*>::const_iterator n = copy.nodev.begin(); n != copy.nodev.end(); ++n)
	{
		node_map_copy2new[*n] =
			def_node((*n)->get_name(), machine_map_copy2new[(*n)->get_parent()],
				(*n)->get_id());
	}

	// Copy process vector
	for (vector<Process*>::const_iterator p = copy.procv.begin(); p != copy.procv.end(); ++p)
	{
		process_map_copy2new[*p] =
			def_proc((*p)->get_name(), (*p)->get_rank(),
				node_map_copy2new[(*p)->get_parent()], (*p)->get_id());
	}

	// Copy thread vector
	for (vector<Thread*>::const_iterator t = copy.thrdv.begin(); t != copy.thrdv.end(); ++t)
	{
    thread_map_copy2new[*t] =
  		def_thrd((*t)->get_name(), (*t)->get_rank(),
	  		process_map_copy2new[(*t)->get_parent()], (*t)->get_id());
	}

	// Copy the cartesian vector.
	for (vector<Cartesian*>::const_iterator c = copy.cartv.begin(); c != copy.cartv.end(); ++c)
	{
		Cartesian * topo=def_cart((*c)->get_ndims(), (*c)->get_dimv(), (*c)->get_periodv());
		topo->set_name((*c)->get_name());
		topo->set_namedims((*c)->get_namedims());
		std::multimap<const Sysres*, std::vector<long> > sysv2coord=(*c)->get_cart_sys();
		std::multimap<const Sysres*, std::vector<long> >::iterator it;
		for(it=sysv2coord.begin();it != sysv2coord.end();++it) {
		  topo->def_coords((Sysres *)it->first,it->second);
		}
	}

 // Copy severity data
  for (vector<Metric*>::const_iterator m = copy.metv.begin(); m != copy.metv.end(); ++m)
  {
    for (vector<Cnode*>::const_iterator c = copy.cnodev.begin(); c != copy.cnodev.end(); ++c)
    {
      for (vector<Thread*>::const_iterator t = copy.thrdv.begin(); t != copy.thrdv.end(); ++t)
      {
        set_sev(metric_map_copy2new[*m], cnode_map_copy2new[*c], thread_map_copy2new[*t],
          copy.get_sev(*m,*c,*t));
      }
    }
  }
}

Cube::~Cube() {
  for (unsigned int i = 0; i < metv.size(); i++)
    delete metv[i];
  for (unsigned int i = 0; i < regv.size(); i++)
    delete regv[i];
  for (unsigned int i = 0; i < cnodev.size(); i++)
    delete cnodev[i];
  for (unsigned int i = 0; i < machv.size(); i++)
    delete machv[i];
  for (unsigned int i = 0; i < nodev.size(); i++)
    delete nodev[i];
  for (unsigned int i = 0; i < procv.size(); i++)
    delete procv[i];
  for (unsigned int i = 0; i < thrdv.size(); i++)
    delete thrdv[i];
  for (unsigned int i = 0; i < cartv.size(); i++)
    delete cartv[i];
}

void Cube::def_attr(const string& key, const string& value) {
  attrs[key] = value;
}

string Cube::get_attr(const std::string& key) const {
  map<std::string,std::string>::const_iterator it = attrs.find(key);
  if (it != attrs.end())
    return it->second;

  return string();
}

void Cube::def_mirror(const std::string& url) {
  mirror_urlv.push_back(url);
}

const vector<string>& Cube::get_mirrors() const {
  return mirror_urlv;
}

Metric* Cube::def_met(const string& disp_name,
                      const string& uniq_name,
                      const string& dtype,
                      const string& uom,
                      const string& val,
     		      const string& url,
                      const string& descr,
                      Metric* parent)
{
  return def_met(disp_name, uniq_name, dtype, uom, val, url,
                 descr, parent, cur_metric_id);
}

Metric* Cube::def_met(const string& disp_name,
                      const string& uniq_name,
                      const string& dtype,
                      const string& uom,
                      const string& val,
     		      const string& url,
                      const string& descr,
                      Metric* parent,
                      uint32_t id)
{
  Metric* met = new Metric(disp_name, uniq_name, dtype, uom, val, url,
                           descr, parent, id);
  if (!parent)
    root_metv.push_back(met);

  if(metv.size() <= id)
    metv.resize(id + 1);
  else if (metv[id])
    throw RuntimeError("Metric with this ID exists");

  metv[id]      = met;
  cur_metric_id = metv.size();

  return met;
}

Region* Cube::def_region(const string& name,
                         long begln,
                         long endln,
			 const string& url,
                         const string& descr,
                         const string& mod)
{
  return def_region(name, begln, endln, url, descr, mod, cur_region_id);
}

Region* Cube::def_region(const string& name,
                         long begln,
                         long endln,
			 const string& url,
                         const string& descr,
                         const string& mod,
                         uint32_t id)
{
  Region* reg = new Region(name, begln, endln, url, descr, mod, id);
  if (regv.size() <= id)
    regv.resize(id + 1);
  else if (regv[id])
    throw RuntimeError("Region with this ID exists");

  regv[id]      = reg;
  cur_region_id = regv.size();

  return reg;
}

Cnode* Cube::def_cnode(Region *callee,
                       const string& mod,
                       int line,
                       Cnode* parent)
{
  return def_cnode(callee, mod, line, parent, cur_cnode_id);
}

Cnode* Cube::def_cnode(Region* callee, Cnode* parent)
{
  return def_cnode(callee, "", -1, parent, cur_cnode_id);
}

Cnode* Cube::def_cnode(Region *callee,
                       const string& mod,
                       int line,
                       Cnode* parent,
                       uint32_t id)
{
  Cnode* cnode = new Cnode(callee, mod, line, parent, id);
  if (!parent)
    root_cnodev.push_back(cnode);

  callee->add_cnode(cnode);
  if (cnodev.size() <= id)
    cnodev.resize(id + 1);
  else if (cnodev[id])
    throw RuntimeError("Cnode with this ID exists");

  cnodev[id]   = cnode;
  cur_cnode_id = cnodev.size();

  return cnode;
}

Machine* Cube::def_mach(const string& name, const string& desc)
{
  return def_mach(name, desc, cur_machine_id);
}

Machine* Cube::def_mach(const string& name,
                        const string& desc,
                        uint32_t id)
{
  Machine* mach = new Machine(name, desc, id, sysv.size());

  if (machv.size() <= id)
    machv.resize(id + 1);
  else if (machv[id])
    throw RuntimeError("Machine with this ID exists");

  machv[id]      = mach;
  cur_machine_id = machv.size();
  sysv.push_back(mach);

  return mach;
}

Node* Cube::def_node(const string& name, Machine* mach)
{
  return def_node(name, mach, cur_node_id);
}

Node* Cube::def_node(const string& name,
                     Machine* mach,
                     uint32_t id)
{
  Node* node = new Node(name, mach, id, sysv.size());

  if (nodev.size() <= id)
    nodev.resize(id + 1);
  else if (nodev[id])
    throw RuntimeError("Node with this ID exists");

  nodev[id]   = node;
  cur_node_id = nodev.size();
  sysv.push_back(node);

  return node;
}

Process* Cube::def_proc(const string& name, int rank, Node* node)
{
  return def_proc(name, rank, node, cur_process_id);
}

Process* Cube::def_proc(const string& name,
                        int rank,
                        Node* node,
                        uint32_t id)
{
  Process* proc = new Process(name, rank, node, id, sysv.size());

  if (procv.size() <= id)
    procv.resize(id + 1);
  else if (procv[id])
    throw RuntimeError("Process with this ID exists");

  procv[id]      = proc;
  cur_process_id = procv.size();
  sysv.push_back(proc);

  return proc;
}

Thread* Cube::def_thrd(const string& name, int rank, Process* proc)
{
  return def_thrd(name, rank, proc, cur_thread_id);
}

Thread* Cube::def_thrd(const string& name,
                       int rank,
                       Process* proc,
                       uint32_t id)
{
  Thread* thrd = new Thread(name, rank, proc, id, sysv.size());

  if (thrdv.size() <= id)
    thrdv.resize(id + 1);
  else if (thrdv[id])
    throw RuntimeError("Thread with this ID already exists");

  thrdv[id]     = thrd;
  cur_thread_id = thrdv.size();
  sysv.push_back(thrd);

  return thrd;
}

Cartesian* Cube::def_cart(long ndims, const vector<long>& dimv,
				const vector<bool>& periodv) {
   /* if (ndims > 3) {
      std::cerr << "Topology with more than three dimensions, only the first three will be displayed" << std::endl;
    }

	//throw RuntimeError("Cube::def_cart(long, const vector<long>&, const vector<bool>&): Can't support a Cartesian topology with > 3 dimensions");*/
    Cartesian* newc = new Cartesian(ndims, dimv, periodv);
    cartv.push_back(newc);
    return newc;
}

const Cartesian* Cube::get_cart(int i) const {
  if (cartv.size() > 0)
      return cartv[i];
  else
     return NULL;
}

void Cube::def_coords(Cartesian* cart, Sysres* sys,
                      const vector<long>& coordv) {
  cart->def_coords(sys, coordv);
}

void Cube::def_coords(Cartesian* cart, Machine* sys,
                      const vector<long>& coordv) {
  cart->def_coords(sys, coordv);
}

void Cube::def_coords(Cartesian* cart, Node* sys,
                      const vector<long>& coordv) {
  cart->def_coords(sys, coordv);
}

void Cube::def_coords(Cartesian* cart, Process* sys,
                      const vector<long>& coordv) {
  cart->def_coords(sys, coordv);
}

void Cube::def_coords(Cartesian* cart, Thread* sys,
                      const vector<long>& coordv) {
  cart->def_coords(sys, coordv);
}

void Cube::set_sev(Metric* met, Cnode* cnode, Thread* thrd,
		   double value) {

   met->set_sev(cnode, thrd, value);

}

void Cube::set_sev(Metric* met, Region* region, Thread* thrd,
		   double value) {

  Cnode* v_cnode = NULL;

  for (unsigned int i = 0; i < cnodev.size(); i++) {
    const Region* tmp = cnodev[i]->get_callee();
    if (tmp == region) {
      v_cnode = cnodev[i];
      break;
    }
  }
  if (v_cnode == NULL) {
    v_cnode = def_cnode(region, region->get_mod(), -1, NULL);
  }

  set_sev(met, v_cnode, thrd, value);
}

void Cube::add_sev(Metric* met, Cnode*  cnode, Thread* thrd, double incr) {
  double val = get_sev(met, cnode, thrd);
  set_sev(met, cnode, thrd, incr+val);
}

void Cube::add_sev(Metric* met, Region* region, Thread* thrd, double incr) {
  Cnode* v_cnode = NULL;

  for (unsigned int i = 0; i < cnodev.size(); i++) {
    const Region* tmp = cnodev[i]->get_callee();
    if (tmp == region) {
      v_cnode = cnodev[i];
      break;
    }
  }
  if (v_cnode == NULL) {
    v_cnode = def_cnode(region, region->get_mod(), -1, NULL);
  }
  double val = get_sev(met, v_cnode, thrd);
  set_sev(met, v_cnode, thrd, incr+val);
}





double Cube::get_sev(Metric* met, Cnode*  cnode, Thread* thrd) const {

   return met->get_sev(cnode, thrd);
}

void Cube::write(const string& filename) {
#ifndef CUBE_COMPRESSED
  ofstream out;
#else
  gzofstream out;
#endif

  out.open(filename.c_str());
  if ( !out.good() ) {
    throw RuntimeError("Cube::write(string): file name doesn't exist");
  }
  out << *this;
  out.close();
}

int Cube::max_num_thrd() const {
  unsigned int ret = 0;
  for (unsigned int i = 0; i < procv.size(); i++) {
    const Process* p = procv[i];
    if (p->num_children() > ret) ret = p->num_children();
  }
  return ret;
}
/**
* \details Gives a metric with "uniq_name".
*/
Metric* Cube::get_met(const string& uniq_name) const {
  for (unsigned int i = 0; i < metv.size(); i++) {
    if (metv[i]->get_uniq_name() == uniq_name)
      return metv[i];
  }
  return NULL;
}
/**
* \details Gives a root metric.
*/
Metric* Cube::get_root_met(Metric* met) {
  if (met == NULL) return 0;
  while (met->get_parent() != NULL) {
    met = met->get_parent();
  }
  return met;
}
/**
* \details Gives not NULL only if cn exists in a cube..
*/
Cnode* Cube::get_cnode(Cnode& cn) const {
  const vector<Cnode*> &cnodev  = get_cnodev();
  for (unsigned int i = 0; i < cnodev.size(); i++) {
    if (*cnodev[i] == cn)
      return cnodev[i];
  }
  return NULL;
}
/**
* \details Gives not NULL only if mach exists in a cube..
*/
Machine* Cube::get_mach(Machine& mach) const {
  const vector<Machine*> &machv  = get_machv();
  for (unsigned int i = 0; i < machv.size(); i++) {
    if (*machv[i] == mach)
      return machv[i];
  }
  return NULL;
}

/**
* \details Gives not NULL only if node exists in a cube..
*/
Node* Cube::get_node(Node& node) const {
  const vector<Node*> &nodev  = get_nodev();
  for (unsigned int i = 0; i < nodev.size(); i++) {
    if (*nodev[i] == node)
      return nodev[i];
  }
  return NULL;
}


namespace cube {
/**
* \details Writes a xml .cube file. First general header, then structure of every dimension and then the data.
*/
  ostream& operator<< (ostream& out,  const Cube& cube) {

    const vector<Cnode*>& cnv        = cube.get_root_cnodev();
    const vector<Machine*>& mv       = cube.get_machv();
    const vector<Metric*>& rmetv     = cube.get_root_metv();
    const vector<Metric*>& metv      = cube.get_metv();
    const vector<Region*>& rv        = cube.get_regv();
    const vector<Thread*>& thrdv     = cube.get_thrdv();
    const map<string, string>& attrs = cube.get_attrs();
    const vector<string>& mirrors    = cube.get_mirrors();

    // xml header
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl << endl;
    out << "<cube " << "version=\"3.0\">" << endl;

    // metadata info
    map<string, string>::const_iterator ai;
    for(ai = attrs.begin(); ai != attrs.end(); ai++) {
      out << "  <attr " << "key=" << "\"" << escapeToXML(ai->first) << "\" "
	  << "value=" << "\"" << escapeToXML(ai->second) << "\"/>" << endl;
    }

    // mirrored URLs
    out << "  <doc>" <<endl;
    out << "    <mirrors>" << endl;
    for (unsigned int i = 0; i < mirrors.size(); i++)
      out << "      <murl>" << escapeToXML(mirrors[i]) << "</murl>" << endl;
    out << "    </mirrors>" << endl;
    out << "  </doc>" <<endl;

    // metrics
    out << "  <metrics>"   << endl;
    for (unsigned int i = 0; i < rmetv.size() ; i++)  // write metrics
      rmetv[i]->writeXML(out);
    out << "  </metrics>"  << endl;

    // program
    out << "  <program>"    << endl;

    for (unsigned int i = 0; i < rv.size(); i++)     // write regions
      rv[i]->writeXML(out);

    for (unsigned int i = 0; i < cnv.size(); i++)    // write cnodes
      cnv[i]->writeXML(out);

    out << "  </program>"   << endl;

    // system
    out << "  <system>"  << endl;
    for (unsigned int i = 0; i < mv.size(); i++)     // write system resources
       mv[i]->writeXML(out);

    // topologies
    out << "    <topologies>" << endl;
    for (unsigned int i = 0; i < cube.get_cartv().size(); i++) // write topologies
       cube.get_cart(i)->writeXML(out);
    out << "    </topologies>" << endl;

    out << "  </system>" << endl;

    // severity
    out << "  <severity>"   << endl;

    for (unsigned int i = 0; i < metv.size(); i++)
      metv[i]->writeXML_data(out, thrdv);

    out << "  </severity>"  << endl;
    out << "</cube>"      << endl;

    return out;
  }
/**
* \details Let "driver" to read the xml .cube file and creates a cube.
*/
  istream& operator>> (std::istream& in, Cube& cb) {
    try {
      cubeparser::Driver driver;
      driver.parse_stream(in,cb);
    }
    catch (RuntimeError e) {
      cerr << e;
      throw;
    }
    return in;
  }
}
