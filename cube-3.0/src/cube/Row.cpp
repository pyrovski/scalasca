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
 
	\file Row.cpp
	\brief Defines methods of a row of a matrix. 

 ************************************************/

#include <iostream>
#include <iomanip>

#include "Row.h"
#include "Thread.h"

using namespace std;
using namespace cube;


bool Row::empty() {
  return row.empty();
}

double Row::sum() {

  if (elem_sum_valid)
     return elem_sum;

  elem_sum = 0.0;
  
  if (!empty()) {
    for (unsigned int i = 0; i<row.size(); i++) {
      elem_sum += row[i]; 
    }
    elem_sum_valid = true;
    return elem_sum;
  }
  return elem_sum;
}

void Row::set_elem(Thread* thrd, double value) {
    elem_sum_valid = false;
    uint32_t id = thrd->get_id();
    if (row.size() > id) row[id] = value;
    else if (value!=0.0){
      row.resize(id+1, 0);
      row[id] = value;
    }
}

double Row::get_elem(Thread* thrd) {

  uint32_t id = thrd->get_id();
  
  if(id < row.size()){
  	return row[id];}
  else
  	return 0;
}

const Row& Row::operator-=(const Row& rhs){

  if(row.size()<rhs.row.size())
  	row.resize(rhs.row.size());
  for(unsigned int i=0; i<rhs.row.size(); i++) {
  	row[i]-=rhs.row[i];
  }
  elem_sum_valid = false;
  return *this;
}


const Row& Row::operator+=(const Row& rhs){
  if(row.size()<rhs.row.size())
  	row.resize(rhs.row.size());
  for(unsigned int i=0; i<rhs.row.size(); i++) {
  	row[i]+=rhs.row[i];
  }
  elem_sum_valid = false;
  return *this;
}


Row Row::operator-() const {
  Row result;

  result.row.resize(row.size());
  for (unsigned int i = 0; i < row.size(); i++) {
    result.row[i] = -row[i];
  }
  result.elem_sum_valid = false;
  return result; 
}

Row& Row::unary_minus(Row& result) {
  for (unsigned int i = 0; i < result.row.size(); i++) {
     result.row[i] = -result.row[i];
  }
  elem_sum_valid = false;
  return result;
}


void Row::writeXML(ostream& out, const std::vector<Thread*>& thrdv) const {
  for (unsigned int i = 0; i < row.size(); i++)
    out << setprecision(12) << row[i] << "\n";
  for (unsigned int i = row.size(); i < thrdv.size(); i++)
    out << "0\n";
}
