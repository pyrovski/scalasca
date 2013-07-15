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
* \file Row.h
* \brief Provides a row of a matrix of the severieties.
*/

#ifndef CUBE_ROW_H
#define CUBE_ROW_H   

/*
 *----------------------------------------------------------------------------
 *
 * class Row
 *
 *----------------------------------------------------------------------------
 */

#include <iosfwd>
#include <vector>


namespace cube {

  class Thread;

/**
* The Row is a vector of doubles with acces methods via "Thread*". The ThreadID is used as index in a row.
*/
  class Row {  

  public:
    
    Row() : elem_sum_valid(true), elem_sum(0.0) {}

    bool empty();
    double sum();

    void   set_elem(Thread* thrd, double value);///<The ThreadID is used as index in a row to get an element.
    double get_elem(Thread* thrd); ///< The ThreadID is used as index in a row to get an element.

    Row& unary_minus(Row& result);///< A little arithmetic with the Row.

    void writeXML(std::ostream& out, const std::vector<Thread*>& thrdv) const;  ///< Writes a xml-representation of a row in the .cube file.

    const Row& operator+=(const Row& rhs); ///< A little arithmetic with the Row.
    const Row& operator-=(const Row& rhs);///< A little arithmetic with the Row.
    Row operator-() const;///< A little arithmetic with the Row.

  private:

    std::vector<double> row; ///< The storage of the row.
    bool elem_sum_valid; ///< Is the sum still valid? As indicator of modifications.
    double elem_sum; ///< The sum of the elements. Chached if offen usage.

  };
}

#endif
