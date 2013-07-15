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
* \file Cube.h
* \brief Provides a class "cube". 
*/
#ifndef CUBE_CUBE_H
#define CUBE_CUBE_H   


#include <inttypes.h>

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace cubeparser{
  class Cube2Parser;
  class Cube3Parser;
}

// CUBE library version number
#define CUBE_VERSION_MAJOR  3
#define CUBE_VERSION_MINOR  4


/*
 *----------------------------------------------------------------------------
 *
 * class Cube
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Cartesian;
  class Cnode;
  class Machine;
  class Matrix;
  class Metric;
  class Node;
  class Process;
  class Region;
  class Sysres;
  class Thread;

/**
* Class containing whole information about performance measurement. 
*
* It has a three "dimensions" : "metrics", "call tree" und "threads". Every dimension has its own treelike structure. 
*/
  class Cube {

  public:
    Cube();
    Cube(Cube const& copy);
    virtual ~Cube();

    /**************************************************************
     * CUBE write API
     *************************************************************/ 
    
    // The order of children in any hierarchical structure is defined by
    // the order of child creation. For example, the first performance metric
    // created as child of a given metric is the first child.

    /// set attributes
    void def_attr(const std::string& key, const std::string& value);
    /// set mirror URLs
    void def_mirror(const std::string& url);

    // build metric dimension

    // url is a URL or a file path, or a string of "@mirror@...". 
    // descr is simply a short string.

/**  
* Defines a metric and its parent in the dimension "metrix" of the cube.
*
* url is a URL or a file path, or a string of "@mirror@...", descr is simply a short string.
*/
    Metric*   def_met(const std::string& disp_name,
                      const std::string& uniq_name,
                      const std::string& dtype, 
		      const std::string& uom,
                      const std::string& val,
                      const std::string& url,
                      const std::string& descr,
                      Metric* parent);
    
    // build program dimension
/**
* Define a region in a program.
*/
    Region*   def_region(const std::string& name,
                         long begln,
                         long endln, 
			 const std::string& url,
                         const std::string& descr,
                         const std::string& mod);

    // if source-code info available
/**
* Define a call node with a known source code.
*/
    Cnode*    def_cnode(Region* callee,
                        const std::string& mod,
                        int line,
                        Cnode* parent);
    // if source-code info not available
/**
* Define a call node without a known source code.
*/
    Cnode*    def_cnode(Region* callee, Cnode* parent);
       
    // build system dimension
/**
* Define machine, on which the measurement was done.
*/
    Machine*  def_mach(const std::string& name, const std::string& desc);
/**
* Define node of the machine.
*/
    Node*     def_node(const std::string& name, Machine* mach);
/**
* Define process of a programm.
*/
    Process*  def_proc(const std::string& name, int rank, Node* node);
/**
* Define thread.
*/
    Thread*   def_thrd(const std::string& name, int rank, Process* proc);

    // define topology
/**
* Set to every thread coordinates in the topology.
*/
    Cartesian* def_cart (long ndims, const std::vector<long>& dimv, const std::vector<bool>& periodv);
    void  def_coords(Cartesian* cart, Sysres*  sys, const std::vector<long>& coordv);    
    void  def_coords(Cartesian* cart, Machine* sys, const std::vector<long>& coordv);    
    void  def_coords(Cartesian* cart, Node*    sys, const std::vector<long>& coordv);    
    void  def_coords(Cartesian* cart, Process* sys, const std::vector<long>& coordv);    
    void  def_coords(Cartesian* cart, Thread*  sys, const std::vector<long>& coordv);    

    // set severity data
/**
* Set a severity "value" for triplet (metric, cnode, thread)
*/
    void set_sev(Metric* met, Cnode*  cnode,  Thread* thrd, double value); 
/**
* Add a value "inc" to severiti for triplet (metric, cnode, thread)
*/
    void add_sev(Metric* met, Cnode*  cnode,  Thread* thrd, double incr);

    // flat-tree only
/**
* Set a severity "value" for triplet (metric, region, thread)
*/
    void set_sev(Metric* met, Region* region, Thread* thrd, double value);
/**
* Add a value "inc" to severiti for triplet (metric, region, thread)
*/
    void add_sev(Metric* met, Region* region, Thread* thrd, double incr);
                 
/**
* Set a vector of topologies
*/
    void add_cart(std::vector<Cartesian*>& cart) { cartv = cart; };



    // write to a file
/**
* Write a XML file.
*/
    void write(const std::string& filename);

    /**************************************************************
     * CUBE read API
     *************************************************************/ 

    // get vector of entities
    const std::vector<Metric*>&    get_metv()      const { return metv; }			///<  Get a vector of metrics.
    const std::vector<Region*>&    get_regv()      const { return regv; }			///<  Get a vector of regions.
    const std::vector<Cnode*>&     get_cnodev()    const { return cnodev; }			///<  Get a vector of call nodes.
    const std::vector<Sysres*>&    get_sysv()      const { return sysv; };			///<  Get a vector of system resources.
    const std::vector<Machine*>&   get_machv()     const { return machv; };			///<  Get a vector of machines.
    const std::vector<Node*>&      get_nodev()     const { return nodev; };			///<  Get a vector of nodes.
    const std::vector<Process*>&   get_procv()     const { return procv; }			///<  Get a vector of processes.
    const std::vector<Thread*>&    get_thrdv()     const { return thrdv; }			///<  Get a vector of threads.
    const std::vector<Cartesian*>& get_cartv()     const { return cartv; };			///<  Get a vector of cartesian topologies.

    const Cartesian*               get_cart(int i)  const;

    /// get the roots of metric and program dimension
    const std::vector<Metric*>&    get_root_metv()   const { return root_metv; };
    /// get the roots of program dimension
    const std::vector<Cnode*>&     get_root_cnodev() const { return root_cnodev; };

    /// get severity
    double get_sev(Metric* met, Cnode*  cnode, Thread* thrd) const; 

    /// get attribute
    std::string get_attr(const std::string& key) const;
    /// get attributes
    const std::map<std::string, std::string>&  get_attrs() const { return attrs; }

    /// get mirrors
    const std::vector<std::string>& get_mirrors() const;

    /// get mirrors
    std::vector<std::string>& get_mirror() { return mirror_urlv; }

    /// returns the number of threads
    int max_num_thrd() const;

    /// search for metrics
    Metric* get_met(const std::string& uniq_name) const;
    /// get a root of metrics
    Metric* get_root_met(Metric* met);

    /// search for cnodes    
    Cnode* get_cnode(Cnode& cn) const;

    /// search for sysres (machines, nodes)    
    Machine* get_mach(Machine& mach) const;
    /// get a machine node
    Node* get_node(Node& node) const;

  private:

    // metric dimension
    std::vector<Metric*>    metv;			///<  Vector of metrics. Plain storage. Tree hierarchy is saved inside of metric.
    // program dimension
    std::vector<Region*>    regv;     			///<  Vector of region. Plain storage. Tree hierarchy is saved inside of region.
    std::vector<Cnode*>     cnodev;   			///<  Vector of call threes. Plain storage. Tree hierarchy is saved inside of cnode.
    // system dimension
    std::vector<Sysres*>    sysv;   			///<  Vector of system resources. 
    std::vector<Machine*>   machv;        		///<  Vector of machines. 
    std::vector<Node*>      nodev;    			///<  Vector of nodes.   
    std::vector<Process*>   procv;   
    std::vector<Thread*>    thrdv;       
    std::vector<Cartesian*> cartv;
    /// attributes <key, value>
    std::map<std::string, std::string> attrs; 
    /// mirror urls
    std::vector<std::string> mirror_urlv;

    ///severity
    std::map<Metric*, Matrix*> sev_matm;
    Matrix* get_sev_matrix(Metric* metric);

    /// roots of metric dimension
    std::vector<Metric*>  root_metv;
    /// roots of call tree 
    std::vector<Cnode*>   root_cnodev;

    uint32_t cur_cnode_id;
    uint32_t cur_metric_id;
    uint32_t cur_region_id;
    uint32_t cur_machine_id;
    uint32_t cur_node_id;
    uint32_t cur_process_id;
    uint32_t cur_thread_id;
/**
* Actual method to create a metric.
*/
    Metric*   def_met(const std::string& disp_name,
                      const std::string& uniq_name,
                      const std::string& dtype, 
		      const std::string& uom,
                      const std::string& val,
                      const std::string& url,
                      const std::string& descr,
                      Metric* parent,
                      uint32_t id);
/**
* Actual method to create a region.
*/
    Region*   def_region(const std::string& name,
                         long begln,
                         long endln, 
			 const std::string& url,
                         const std::string& descr,
                         const std::string& mod,
                         uint32_t id);
/**
* Actual method to create a cnode.
*/
    Cnode*    def_cnode(Region* callee,
                        const std::string& mod,
                        int line,
                        Cnode* parent,
                        uint32_t id);
/**
* Actual method to create a machine.
*/
    Machine*  def_mach(const std::string& name,
                       const std::string& desc,
                       uint32_t id);
/**
* Actual method to create a node.
*/
    Node*     def_node(const std::string& name,
                       Machine* mach,
                       uint32_t id);
/**
* Actual method to create a process.
*/
    Process*  def_proc(const std::string& name,
                       int rank,
                       Node* node,
                       uint32_t id);
/**
* Actual method to create a thread.
*/
    Thread*   def_thrd(const std::string& name,
                       int rank,
                       Process* proc,
                       uint32_t id);

    friend class MdAggrCube; ///< An extension that also extends Cnode and Region.
    friend class Parser; ///< Parser (not defined anywhere)
    friend class cubeparser::Cube2Parser; ///< Parser of saved .cube v2.0
    friend class cubeparser::Cube3Parser; ///< Parser of saved .cube v3.0
  };

  // I/O operators
/// Read a cube
  std::istream& operator>> (std::istream& in, Cube& cb);
/// Write a cube.
  std::ostream& operator<< (std::ostream& out, const Cube& cb);
}

#endif
