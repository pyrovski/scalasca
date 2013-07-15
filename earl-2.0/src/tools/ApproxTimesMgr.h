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

#ifndef PERTRB_APPROXTIMESMGR_H
#define PERTRB_APPROXTIMESMGR_H

#include <list>
#include <map>
#include <set>
#include <vector>

#include <elg_rw.h>


class ApproxTimesMgr
{
public:

  ApproxTimesMgr(long nlocs, long nevents, char* infile_name, char* outfile_name); 
  ~ApproxTimesMgr(); 
  
  bool   has_approx_time(long pos) const;
  void   set_approx_time(long pos, long pred_pos, long loc_id, long ndis, double val);
  double get_approx_time(long pos) const;
  void   dismiss(long pos);           // dismiss pos an event depends on after it has been approximated
  
private:

  void   erase(long pos);             // erase approximated pos
  void   rewrite();                   // rewrite the maximum possible number of events 

  long   get_min_pos() const;         // smallest available position
  long   get_max_pos() const;         // largest available position
  long   get_max_pos_rewrt() const;   // largest pos that can be rewritten
  double get_max_at_rewrt() const;    // largest approximated time that can be rewritten


  std::vector<std::list<long> > loc2posl;  // location |-> list with approximated positions
  std::map<long, double>        pos2atime; // pos |-> approximated time
  std::map<long, long>          pos2dep;   // pos |-> number of unapproximated dependant events
  std::map<long, long>          pos2loc;   // pos |-> location (i.e., the process)

  ElgIn*  inf;
  ElgOut* outf;

  long nevents;
  long rewrt_pos;
};

#endif
