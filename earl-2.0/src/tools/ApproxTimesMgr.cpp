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

#include "ApproxTimesMgr.h"

#include <algorithm>
#include <cstdio>
#include <iostream>

#include <elg_error.h>
#include <elg_rw.h>

using namespace std;

#define EPSILON 0.000000001

/*
 *---------------------------------------------------------------------------------
 * Read trace records
 *---------------------------------------------------------------------------------
 */

ElgRec* 
get_next_def_rec(ElgIn* inf)
{
  ElgRec* rec;
 
  while( (rec = ElgIn_read_record(inf)) != NULL )
    {
      if (!ElgRec_is_event(rec))
	return rec;
      ElgRec_free(rec);
    }
  return NULL;
}

ElgRec* 
get_next_event_rec(ElgIn* inf)
{
  ElgRec* rec;

  while( (rec = ElgIn_read_record(inf)) != NULL )
    {
      if (ElgRec_is_event(rec))
	  return rec;
      ElgRec_free(rec);
    }
  return NULL;
}
  

/*
 *---------------------------------------------------------------------------------
 * Copy definition records
 *---------------------------------------------------------------------------------
 */

void 
copy_definition_records(ElgIn* inf, ElgOut* outf)
{
  ElgRec* rec;

  cerr << "Copy definition records...    ";
  ElgIn_rewind(inf);
  while ( (rec = get_next_def_rec(inf)) != NULL )
    ElgOut_write_record(outf, rec);
  ElgIn_rewind(inf);
  cerr << "done" << endl;
}


/*
 *---------------------------------------------------------------------------------
 * Less-than operator for event records
 *---------------------------------------------------------------------------------
 */

class CompareRecords
{
public:
  int operator()(ElgRec* lhs, ElgRec* rhs) const
  {
    // check whether both are event records
    if (!ElgRec_is_event(lhs))
      elg_error_msg("bool operator<(const ElgRec*, const ElgRec*): no event record");
    if (!ElgRec_is_event(rhs))
      elg_error_msg("bool operator<(const ElgRec*, const ElgRec*): no event record");
    
    // get time stamps
    double lhs_time, rhs_time;
    ElgRec_seek(lhs, 4);
    ElgRec_seek(rhs, 4);
    lhs_time = ElgRec_read_d8(lhs);
    rhs_time = ElgRec_read_d8(rhs);
    
    return lhs_time < rhs_time;
  }
};


/*
 *---------------------------------------------------------------------------------
 * ApproxTimesMgr class methods
 *---------------------------------------------------------------------------------
 */

ApproxTimesMgr::ApproxTimesMgr(long nlocs, long nevents, char* infile_name, char* outfile_name) : 
  loc2posl(nlocs),
  inf(inf),
  outf(outf),
  nevents(nevents),
  rewrt_pos(0)
{
  // open input file (epilog)
  inf = ElgIn_open(infile_name);
  if (!inf)
    elg_error_msg("Cannot open file \"%s\"", infile_name);

  // open output file
  outf = ElgOut_open(outfile_name, ElgIn_get_byte_order(inf), ELG_UNCOMPRESSED);
  if (!outf) 
    elg_error_msg("Cannot open file \"%s\"", outfile_name);

  copy_definition_records(inf, outf);
} 


ApproxTimesMgr::~ApproxTimesMgr() 
{
  rewrite();

  // close infile
  ElgIn_close(inf);
  // close outfile
  ElgOut_close(outf);
} 

bool   
ApproxTimesMgr::has_approx_time(long pos) const
{
  if ( pos2atime.find(pos) != pos2atime.end() )
    return true;
  else
    return false;
}

void   
ApproxTimesMgr::set_approx_time(long pos,  long pred_pos, long loc_id, long ndis, double val)
{
  if ( this->has_approx_time(pos) )
    elg_error_msg("Approximated time for position %d has already been set before.", pos);

  // if pred_pos then enforce partial order
  if ( pred_pos > 0 )
    {
      if ( ! this->has_approx_time(pred_pos) )
	elg_error_msg("Approximated time for predecessor position %d not available.", pred_pos);
      double pred_a = this->get_approx_time(pred_pos); 
      if ( val <= pred_a )
	{
	  // restore partial order 
	  val = max(val, pred_a + EPSILON);
	}
    }

  // enter val in value map
  pos2atime[pos] = val;

  // remeber location
  pos2loc[pos] = loc_id;

  // remeber required number of dismissals
  pos2dep[pos] = ndis;

  // enter pos in list belonging to loc_id
  if ( loc2posl[loc_id].empty() )
    loc2posl[loc_id].push_back(pos);
  // insert at the back?
  else if ( pos < loc2posl[loc_id].back() )
    loc2posl[loc_id].push_back(pos);
  // insert at the front?
  else if ( pos > loc2posl[loc_id].front() )
    loc2posl[loc_id].push_front(pos);
  else 
    elg_error_msg("ApproxTimesMgr::set_approx_time(long, long, double): middle-insertion in per-process queue");
  
  if ( (this->get_max_pos_rewrt() - this->get_min_pos()) > 100 )
    this->rewrite();
}

void   
ApproxTimesMgr::dismiss(long pos)
{
  if ( ! this->has_approx_time(pos) )
    elg_error_msg("ApproxTimesMgr::dismiss(long): position %d unavailable.", pos);

  pos2dep[pos]--;
}

void   
ApproxTimesMgr::erase(long pos)
{
  map<long, double>::iterator itm;
  
  if ( (itm = pos2atime.find(pos)) == pos2atime.end() )
    elg_error_msg("ApproxTimesMgr::erase(long, long): position %d unavailable.", pos);

  pos2atime.erase(itm);
  pos2dep.erase(pos2dep.find(pos));
  loc2posl[pos2loc[pos]].remove(pos);
  pos2loc.erase(pos2loc.find(pos));
}

double 
ApproxTimesMgr::get_approx_time(long pos) const
{
  map<long, double>::const_iterator it;
  
  if ( (it = pos2atime.find(pos)) == pos2atime.end() )
    elg_error_msg("ApproxTimesMgr::get_approx_time(long): position %d unavailable.", pos);
  
  return it->second;
}

long   
ApproxTimesMgr::get_min_pos() const
{
  if ( ! pos2atime.empty() ) 
    return pos2atime.begin()->first;
  else
    return -1;
}         

long   
ApproxTimesMgr::get_max_pos() const
{
  if ( ! pos2atime.empty() ) 
    return (--pos2atime.end())->first;
  else
    return -1;
}  

long   
ApproxTimesMgr::get_max_pos_rewrt() const
{  
  if ( pos2atime.empty() )
    return -1;
  
  if ( pos2atime.size() == 1)
    return this->get_min_pos();
  
  long cur_pos = this->get_min_pos() - 1;

  // end of trace file already approximated?
  if ( this->get_max_pos() != nevents )
    {
      double max_at_rewrt = this->get_max_at_rewrt();
  
      map<long, double>::const_iterator it;
      double atime;
      long pos;
      for ( it = pos2atime.begin(); it != pos2atime.end(); it++ )
	{
	  pos = it->first;
	  atime = it->second;
	  if ( pos == cur_pos + 1 &&               // range must be contiguous 
	       atime <= max_at_rewrt &&            // must be <= allowed max time
	       pos2dep.find(pos)->second ==  0 )   // must already have been dismissed
	    cur_pos = pos;
	  else
	    break;
	}
    }
  else
    {
      map<long, double>::const_iterator it;
      for ( it = pos2atime.begin(); it != pos2atime.end(); it++ )
	{
	  long pos = it->first;
	  if ( pos == cur_pos + 1 &&               // range must be contiguous 
	       pos2dep.find(pos)->second ==  0 )   // must already have been dismissed
	    cur_pos = pos;
	  else
	    break;
	}
    }
  if ( cur_pos >=  this->get_min_pos() )
    return cur_pos;
  else 
    return 0;
}   

double 
ApproxTimesMgr::get_max_at_rewrt() const
{
  vector<double> atimev;

  if ( pos2atime.empty() )
    elg_error_msg("ApproxTimesMgr::get_max_at_rewrt(): no approximated times available.");
    
  // collect maximum approx times  
  for ( unsigned long i = 0; i < loc2posl.size(); i++ )
    if ( ! loc2posl[i].empty() )
      atimev.push_back(pos2atime.find(loc2posl[i].front())->second);

  // return minimum
  return *min_element(atimev.begin(), atimev.end());
}    

void 
ApproxTimesMgr::rewrite()
{
  long nrwevents = this->get_max_pos_rewrt() - this->get_min_pos() + 1;
  if ( nrwevents < 1 )
    return;

  // check that min_pos is last rewritten pos + 1
  if ( this->get_min_pos() != rewrt_pos + 1 )
    elg_error_msg("ApproxTimesMgr::rewrite(): wrong rewrite position");
    
  vector<ElgRec*> recordv; 

  // read n input events
  ElgRec* rec;
  for ( long i = 0; i < nrwevents; i++ )
    {
      if ( (rec = get_next_event_rec(inf)) == NULL )
	elg_error_msg("ApproxTimesMgr::rewrite(): IO error");
      recordv.push_back(rec);
    }
  
  // replace time stamps and erase approximated values
  for ( long i = 0; i < nrwevents; i++ )
    {
      ElgRec_write_d8(recordv[i], this->get_approx_time(++rewrt_pos), 4);
      this->erase(rewrt_pos);
    }

  // sort
  sort(recordv.begin(), recordv.end(), CompareRecords());

  // write new records to output file and free records
  for ( long i = 0; i < nrwevents; i++ )
    {
      // filter DUMP records
      //if ( ( ElgRec_get_type(recordv[i]) != ELG_ENTER_DUMP ) && ( ElgRec_get_type(recordv[i]) != ELG_EXIT_DUMP ) )
      if ( ElgOut_write_record(outf, recordv[i]) == EOF )
	elg_error();
      ElgRec_free(recordv[i]);
    }
}

