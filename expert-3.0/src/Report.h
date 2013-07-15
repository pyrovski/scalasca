/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_REPORT_H
#define EXP_REPORT_H

/*
 *----------------------------------------------------------------------------
 *
 * class Report
 *
 * Analysis report
 *
 *---------------------------------------------------------------------------- 
 */

#include <string>

#include <earl.h>
#include <Cube.h>

class PatternTree;
class CallTree;

class Report
{
 public:
  Report(const PatternTree& ptree, 
	 const CallTree&    ctree,
	 const earl::EventTrace& trace);

  void write(std::string path);

 private:
  cube::Cube cb;
};

#endif

