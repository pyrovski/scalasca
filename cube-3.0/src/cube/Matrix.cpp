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
 
 \file  Matrix.cpp
 \brief Profides fuctionality of a matrix of severities.

************************************************/

#include <iostream>

#include "Matrix.h"
#include "Row.h"
#include "Cnode.h"
#include "Thread.h"

using namespace std;
using namespace cube;

bool cnode_cmp::operator()(const Cnode *a, const Cnode *b) const 
{
  if (a==NULL) return false;
  else if (b==NULL) return true;
  else return (a->get_id() < b->get_id());
}


Matrix::~Matrix() {     
  if (this->empty() == false) {
    cmap::const_iterator iter;
    for (iter = rowm.begin(); iter != rowm.end(); iter++) {
      delete iter->second;
    }
  }
}


bool Matrix::empty() const {
  return rowm.empty();
}

double Matrix::sum() {

  if (elem_sum_valid == true)
    return elem_sum;

  elem_sum = 0.0; 
    
  if (this->empty() == false) {
    cmap::const_iterator iter;
    for (iter = rowm.begin(); iter != rowm.end(); iter++) {
       Row* mr = iter->second;
       elem_sum += mr->sum();
    }
  }
  return elem_sum;
}

void Matrix::clear(){
  if (this->empty() == false) {
    cmap::const_iterator iter;
    for (iter = rowm.begin(); iter != rowm.end(); iter++) {
      delete iter->second;
    }
    rowm.clear();
  }
}

Row* Matrix::get_row(Cnode* cnode) {
  Row* mrow = new Row();
  rowm[cnode] = mrow;
  return mrow;
}

void Matrix::set_elem(Cnode* cnode, Thread* thrd, double val) {

  cmap::const_iterator iter;
  iter = rowm.find(cnode);

  Row* mrow;
  if (iter == rowm.end())
     mrow = get_row(cnode);
  else
     mrow = iter->second;

  mrow->set_elem(thrd, val);
  elem_sum_valid = false;
}

double Matrix::get_elem(Cnode* cnode, Thread* thrd) {

  cmap::const_iterator iter;
  iter = rowm.find(cnode);
  if (iter != rowm.end()) { 
    return iter->second->get_elem(thrd);
  }
  else 
    return 0;
}

void Matrix::writeXML(ostream& out, const std::vector<Thread*>& thrdv) const {

  cmap::const_iterator j; 

  for (j = rowm.begin(); j != rowm.end(); j++) {
     Row* mrow = j->second;
     if (!mrow->empty())
     {
        out << "      <row cnodeId=\"" << j->first->get_id() << "\">" << endl;
        mrow->writeXML(out, thrdv);
        out << "      </row>" << endl;
     }
  }
}
