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

#ifndef PERTRB_COMPENSATOR_H
#define PERTRB_COMPENSATOR_H

#include <earl.h>
#include <vector> 

#include "Overheads.h"

class ApproxTimesMgr;

typedef enum { UPPER_BOUND, LOWER_BOUND } approx_mode;

class Compensator
{
public:

  Compensator(char* infile_name);
  ~Compensator();

  void compensate_trace(char* outfile_name, approx_mode mode); 

private: 

  void dismiss_last_events();
  void check_missing_exits();

  // the predecessor event at a given location is not provided by EARL
  bool compensate_event(earl::Event event, long pred_pos);
  bool compensate_default(earl::Event event, long pred_pos); 
  bool compensate_recv(earl::Event recv);
  bool compensate_n2n(earl::Event mpicexit);
  bool compensate_12n(earl::Event mpicexit);
  bool compensate_n21(earl::Event mpicexit);
  bool compensate_exit_tracing(earl::Event exit_tracing);

  double approx_recv_cm(double send_m, double recv_m, double send_a, double enter_recv_a, size_t msg_size);
  double approx_recv_ca(double send_m, double recv_m, double send_a, double enter_recv_a, size_t msg_size);

  // management of predecessor events
  void set_pred_pos(earl::Location* loc, long pred_pos) { predv[loc->get_id()] = pred_pos; }
  long get_pred_pos(earl::Location* loc)                { return predv[loc->get_id()]; }


  earl::EventTrace* trace;

  ApproxTimesMgr* atime;
  Overheads ov;
  std::vector<long> predv; 

  double mbandw;
  approx_mode amode;  
  char* infile_name;
};

#endif
