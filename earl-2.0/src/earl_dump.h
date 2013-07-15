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

#ifndef EARL_DUMP_H
#define EARL_DUMP_H

//#include <cstdlib>
//#include <iostream>
//#include <iomanip>

#include "earl.h"

#include "MPILocEpoch.h"
#include "MPIRma_rep.h"

    
std::ostream& operator<<(std::ostream& out, const earl::Location& loc);
std::ostream& operator<<(std::ostream& out, const earl::Machine& mach);
std::ostream& operator<<(std::ostream& out, const earl::Node& node);
std::ostream& operator<<(std::ostream& out, const earl::Process& proc);
std::ostream& operator<<(std::ostream& out, const earl::Thread& thrd);
std::ostream& operator<<(std::ostream& out, const earl::Callsite& csite);
std::ostream& operator<<(std::ostream& out, const earl::Region& reg);
std::ostream& operator<<(std::ostream& out, const earl::Metric& met);
std::ostream& operator<<(std::ostream& out, const earl::Communicator& com);
std::ostream& operator<<(std::ostream& out, const earl::MPIWindow& win);
std::ostream& operator<<(std::ostream& out, const earl::Event& event);
std::ostream& operator<<(std::ostream& out, const std::vector<earl::Event>& eventv);
std::ostream& operator<<(std::ostream& out, const earl::MPIRma_rep& e );
std::ostream& operator<<(std::ostream& out, const earl::MPILocEpoch& epoch );
std::ostream& operator<<(std::ostream& out, const earl::MPIEpoch& epoch );

void print_enterv(const std::vector<earl::Event>& eventv);
void print_exitv(const std::vector<earl::Event>& eventv);
void print_sendv(const std::vector<earl::Event>& eventv);
void print_transferv(const std::vector<earl::Event>& eventv);

#endif
