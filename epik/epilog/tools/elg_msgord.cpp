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

#include "elg_error.h"
#include "elg_rw.h"
#include "epk_archive.h"

#include <unistd.h>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <iterator>

using namespace std;

struct TimeCtr 
{
  int cnt;
  queue<elg_d8> tqueue;
};

map<vector<elg_ui4>, int> m_queue;   /* message queue */
map<vector<elg_ui4>, TimeCtr> em_queue;  /* expected message queue */
map<elg_ui4, elg_ui4> lid2nid;
map<elg_ui4, elg_ui4> lid2pid;

/* options */
static elg_ui1 verbose;
static elg_ui1 missing_send;

/* error counters */
static elg_ui4 recv_before_send=0;
static elg_ui4 send_after_recv=0;

void elg_verify_send(long numrec, elg_ui4 slid, elg_ui4 spid, elg_ui4 dlid,
                     elg_ui4 dpid, elg_ui4 cid, elg_ui4 tag, elg_d8 time)
{
  vector<elg_ui4> key(4);
  key[0] = spid; key[1] = dpid; key[2] = cid; key[3] = tag;

  map<vector<elg_ui4>, int>::iterator it;
  map<vector<elg_ui4>, TimeCtr>::iterator itExp;
  
  /* check for MPI_Send after MPI_Recv */
  itExp = em_queue.find(key);
  if (itExp != em_queue.end() && itExp->second.cnt > 0 )
    {
      itExp->second.cnt--;
      elg_d8 rtime = itExp->second.tqueue.front();
      itExp->second.tqueue.pop();
      send_after_recv++;
      fprintf(stderr, "\n[%ld] Expected MPI_Send arrived after MPI_Recv :\n"
                      "  spid: %d[%d], dpid: %d[%d], cid: %d, tag: %d, "
                      "time: %10.6f, delay: %10.6f\n",
                      numrec, spid, lid2nid[slid], dpid, lid2nid[dlid], cid,
                      tag, time, (time-rtime));
      return;
    }

  /* usual send */
  it = m_queue.find(key); 
  if (it != m_queue.end())
    it->second++;
  else
    m_queue[key] = 1;
}

void elg_verify_recv(long numrec, elg_ui4 slid, elg_ui4 spid, elg_ui4 dlid,
                     elg_ui4 dpid, elg_ui4 cid, elg_ui4 tag, elg_d8 time)
{
  vector<elg_ui4> key(4);
  key[0] = spid; key[1] = dpid; key[2] = cid; key[3] = tag;

  map<vector<elg_ui4>, int>::iterator it = m_queue.find(key);
  map<vector<elg_ui4>, TimeCtr>::iterator itExp;

  /* check for MPI_Recv before MPI_Send */
  if (it == m_queue.end() || it->second<=0 )
    {
      fprintf(stderr, "\n[%ld] MPI_Recv before MPI_Send:\n"
                      "  spid: %d[%d], dpid: %d[%d], cid: %d, tag: %d, time: %10.6f\n",
                      numrec, spid, lid2nid[slid], dpid, lid2nid[dlid], cid, tag, time);
      
      /* assume coresponding MPI_Send is lost */
      if ( missing_send )
        return;
        
      /* expect MPI_Send later */
      recv_before_send++;
      itExp = em_queue.find(key);
      if (itExp != em_queue.end())
        {
          itExp->second.cnt++;     
          itExp->second.tqueue.push(time);
        }
      else 
        {
          TimeCtr tc;
          tc.cnt = 1;
          tc.tqueue.push(time);
          em_queue[key] = tc;
        }
      return;
    }
  
  /* usual MPI_Recv */ 
  it->second--;
}

void print_usage()
{
    fprintf(stderr,
            "Usage:    elg_msgord [OPTIONS]...  "
            "(<infile.elg> | <experiment_archive>)\n"
            "Try 'elg_msgord -h' for more information\n"
           );
    fflush(stderr);
}

void print_help()
{
    fprintf(stderr,
         "Usage:    elg_msgord [OPTIONS]...  "
         "(<infile.elg> | <experiment_archive>)\n\n"
         "[OPTIONS] are:\n"
         "-h    - print this help\n"        
         "-o    - assume message order may be wrong, but no messages"
         "        are lost. (Default)\n"
         "-s    - assume MPI_Send may be lost, but message order is correct\n"
         "-v    - verbose output.\n\n"
         "options '-o' and '-s' are mutual exlusive\n"
           );
    fflush(stderr);
}

int parse_opt(int argc, char* argv[])
{
    int ch;
    missing_send = 0;
    verbose      = 0;

    while ((ch = getopt(argc, argv, "osh?v")) != EOF)
    {
        switch (ch)
        {
          case 'h':
          case '?':
              print_help();
              exit(EXIT_SUCCESS);
            break;
          case 'o':
            missing_send = 0;
            break;
          case 's':
            missing_send = 1;
            break;
          case 'v':
              verbose++;
            break;
        }
    }
    return optind;
}

struct value_not_null
{
    bool operator() ( const pair<const vector<elg_ui4>,int>& x ) const
      {
        return x.second != 0;
      }
};

struct time_value_not_null
{
    bool operator() ( const pair<const vector<elg_ui4>,TimeCtr>& x ) const
      {
        return x.second.cnt != 0;
      }
};

int main(int argc, char **argv) 
{
  ElgIn* in;
  ElgRec* rec;
  int opt_ind;
  int exitcode;
  size_t count;
  long numrec = 0;

  opt_ind = parse_opt(argc, argv);
  if ((argc - opt_ind) != 1)
    {
      elg_warning("invalid parameters");
      print_usage();
      exit(EXIT_FAILURE); 
    }
    
  if ( (in = ElgIn_open(epk_get_elgfilename(argv[opt_ind]))) == NULL )
    {
      elg_error_msg("Can't open file %s\n", argv[opt_ind]);
      exit(EXIT_FAILURE);
    }

  while( (rec = ElgIn_read_record(in)) )
    {
      elg_ui1 type = ElgRec_get_type(rec);
      numrec++;

      switch(type)
	{
	case ELG_LAST_DEF :
	  {
	    numrec = 0;
	    break;
	  }
	case ELG_LOCATION :
	  {
	    elg_ui4 lid = ElgRec_read_ui4(rec);
	    /* elg_ui4 mid = */ ElgRec_read_ui4(rec);
	    elg_ui4 nid = ElgRec_read_ui4(rec);
	    elg_ui4 pid = ElgRec_read_ui4(rec);
	    lid2nid[lid] = nid;
	    lid2pid[lid] = pid;
	    break;
	  }
	case ELG_MPI_SEND :
	  {
	    elg_ui4 lid  = ElgRec_read_ui4(rec);
	    elg_ui4 pid  = lid2pid[lid];
	    elg_d8  time = ElgRec_read_d8(rec); 
	    elg_ui4 dlid = ElgRec_read_ui4(rec);
	    elg_ui4 dpid = lid2pid[dlid];
	    elg_ui4 cid  = ElgRec_read_ui4(rec);
	    elg_ui4 tag  = ElgRec_read_ui4(rec);
	    elg_ui4 sent = ElgRec_read_ui4(rec);

            if ( verbose > 0 )
              {
	        printf("<SEND> ");
	        printf(" lid: %4u", lid);	  
    	        printf(" pid: %4u", pid);	  
	        printf(" time: %10.6f", time);	  
	        printf(" dlid: %4u", dlid);	  
	        printf(" dpid: %4u", dpid);	  
	        printf(" cid: %4u", cid);	  
	        printf(" tag: %4u", tag);	  
	        printf(" sent: %u", sent);	  
	        printf("\n");
              }
	    elg_verify_send(numrec, lid, pid, dlid, dpid, cid, tag, time);
	    
	    break;
	  }
	case ELG_MPI_RECV :
	  {
	    elg_ui4 lid  = ElgRec_read_ui4(rec);
	    elg_ui4 pid  = lid2pid[lid];
	    elg_d8  time = ElgRec_read_d8(rec); 
	    elg_ui4 slid = ElgRec_read_ui4(rec);
	    elg_ui4 spid = lid2pid[slid];
	    elg_ui4 cid  = ElgRec_read_ui4(rec);
	    elg_ui4 tag  = ElgRec_read_ui4(rec);
	   
            if ( verbose > 0 )
              {
	        printf("<RECV> ");
	        printf(" lid: %4u", lid);	  
	        printf(" pid: %4u", pid);	  
	        printf(" time: %10.6f", time);	  
	        printf(" slid: %4u", slid);	  
	        printf(" spid: %4u", spid);	  
	        printf(" cid: %4u", cid);	  
	        printf(" tag: %4u", tag);	  
	        printf("\n");
              }
	    elg_verify_recv(numrec, slid, spid, lid, pid, cid, tag, time);
	    
	    break;
	  }
	}
      ElgRec_free(rec);
    }
  ElgIn_close(in);

  exitcode = EXIT_SUCCESS;
  
  fprintf(stderr, "\nError summary:\n");
  if ( recv_before_send )
    {
      fprintf(stderr,"  MPI_Recv before MPI_Send: %d\n",recv_before_send);
      exitcode = EXIT_FAILURE;
    }
    
  if ( send_after_recv )
    {
      fprintf(stderr,"  MPI_Send after MPI_Recv: %d\n",send_after_recv);
      exitcode = EXIT_FAILURE;
    }
 
  #ifdef __SUNPRO_CC
  count = 0;
  for ( map<vector<elg_ui4>, int>::iterator it = m_queue.begin();
        it != m_queue.end(); ++it)
    {
      if ( value_not_null()(*it) ) ++count;
    }
  #else
    // SUNpro CC is even too stupid to compile this simple line
    count = count_if(m_queue.begin(),m_queue.end(), value_not_null() ); 
  #endif
  if ( count != 0 )
    {
      fprintf(stderr,"  MPI_Send without coresponding MPI_Recv: %lld\n", (long long) count );
      exitcode = EXIT_FAILURE;
    }
    
  #ifdef __SUNPRO_CC
  count = 0;
  for ( map<vector<elg_ui4>, TimeCtr>::iterator it = em_queue.begin();
        it != em_queue.end(); ++it)
    {
      if ( time_value_not_null()(*it) ) ++count;
    }
  #else
    count = count_if(em_queue.begin(),em_queue.end(), time_value_not_null() ); 
  #endif
  if ( count > 0 )
    {
      fprintf(stderr,"  MPI_Recv without coresponding MPI_Send: %lld\n", (long long) count );
      exitcode = EXIT_FAILURE;
    }
    
  if (exitcode == EXIT_SUCCESS)
    fprintf(stderr, "  Message order o.k.!\n\n");
  
  return exitcode;
}
