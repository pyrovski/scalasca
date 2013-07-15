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
* \file Matrix.h
* Provides a for given Metric a matrix (Cnode, Thread) -> value
*/ 
#ifndef CUBE_MATRIX_H
#define CUBE_MATRIX_H   


#include <iosfwd>

#include <map>
#include <vector>


/*
 *----------------------------------------------------------------------------
 *
 * class Matrix 
 *
 * (Cnode, Thread) |-> value
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Cnode;
  class Thread;
  class Row;

  class cnode_cmp 
  {
  public:
    bool operator()(const Cnode *a, const Cnode *b) const;
  };

/** class Matrix (Cnode, Thread) -> value
*/
  class Matrix {  

  public:
    

    Matrix() : elem_sum_valid(true), elem_sum(0.0) {}
    ~Matrix();

    bool empty() const; ///< Is a matrix empty?
    double sum(); ///< Sum of the elements.
    void clear(); ///< Empty this matrix.

    void set_elem(Cnode*, Thread*, double val); ///< Save an element at the position.
    double get_elem(Cnode*, Thread*); ///< Get an element in the position.
    
    void writeXML(std::ostream& out, const std::vector<Thread*>& thrdv) const; ///< Write a xml-representation af the matrix for saving in .cube file.

  private:

    typedef std::map<Cnode*, Row*, cnode_cmp> cmap;

    Row* get_row(Cnode* cnode);  ///< Returns a row.
    cmap rowm;  ///< Matrix as a set of rows to every cnode.
    bool elem_sum_valid; ///< Is the summ stil valid or new elements are there?
    double elem_sum; ///< Sum of the elements.
  };
}

#endif
