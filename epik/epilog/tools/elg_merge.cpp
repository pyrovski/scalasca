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

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "elg_error.h"
#include "epk_mpireg.h"
#include "elg_rw.h"
#include "epk_archive.h"
#include "epk_conf.h"
#include "epk_idmap.h"

using namespace std;


/* definitions for queue handling */
#define ELG_RMA_DEQUEUE_NONE     0
#define ELG_RMA_DEQUEUE_LOCAL    1
#define ELG_RMA_DEQUEUE_REMOTE   2
#define ELG_RMA_DEQUEUE_BOTH     3


/*
 *---------------------------------------------------------------------------------
 * Declaration of types
 *---------------------------------------------------------------------------------
 */

/* MPI group */
struct Group
{
  elg_ui4  id;      /* identifier */
  elg_ui1  mode;    /* mode flags */
  elg_ui4  size;    /* number of ranks in group */
  elg_ui4* ranks;   /* local |-> global rank mapping */
  elg_ui4  cid;     /* associated communicator identifier (group |-> comm mapping)*/
};

/* MPI communicator */
struct Comm
{
  elg_ui4  id;     /* identifier */
  elg_ui1  mode;   /* mode flags */
  elg_ui4  grpc;   /* size of bitstring */
  elg_ui1* grpv;   /* bitstring defining communicator */
  elg_ui4  gid;    /* associated group identifier (comm |-> group mapping) */
};

/* MPI RMA window */
struct Window
{
  elg_ui4 id;    /* identifier */
  elg_ui4 cid;   /* associated MPI communicator */
};

/* input trace file */
struct Infile
{
  /* constructor & destructor */
  Infile(const char* file_name, unsigned mpi_rank);
  ~Infile();

  /* member functions */
  void    rewind();
  ElgRec* next_rec();
  double  corrected_time(double time) const;

  elg_ui4 llid2glid(elg_ui4 lid) const;
  elg_ui4 lrid2grid(elg_ui4 rid) const;
  elg_ui4 lcsid2gcsid(elg_ui4 csid) const;
  elg_ui4 lgid2gcid(elg_ui4 gid) const;
  elg_ui4 lcid2gcid(elg_ui4 cid) const;
  elg_ui4 lwid2gwid(elg_ui4 wid) const;

  /* attributes */
  ElgIn* inf;     /* unmerged event trace file */

  elg_ui4 rank;      /* global rank of this file */
  elg_ui4 glid;      /* location id in case of locationless event records */

  int    offcnt;        /* number of offset records */
  double offset[2];     /* clock offset */
  double ltime[2];      /* local time */

  EpkIdMap* locations;     /* local location id     |-> global location id */
  EpkIdMap* regions;       /* local region id       |-> global region id */
  EpkIdMap* callsites;     /* local call site id    |-> global call site id */
  EpkIdMap* groups;        /* local group id        |-> global group id */
  EpkIdMap* comms;         /* local communicator id |-> global communicator id */
  EpkIdMap* windows;       /* local window id       |-> global window id */
};


/*
 *---------------------------------------------------------------------------------
 * Function prototypes
 *---------------------------------------------------------------------------------
 */

int main(int argc, char** argv);

void do_merge(const char* fprefix);
void process_defs(const char* fprefix);
void process_maps(const char* fprefix);
void process_traces_pass1();
void finalize_defs();
void process_traces_pass2();

static int check_locationless(ElgIn* in);
static int rma_sync_type(elg_ui4 rid);
static void create_comm(Group* group, elg_ui4 cid, elg_ui1 mode);


/*
 *---------------------------------------------------------------------------------
 * Global variables 
 *---------------------------------------------------------------------------------
 */

ElgOut* of;                  /* merged event trace */
vector<Infile*> infilev;     /* input event trace files */

/* if no identifier mapping file exists, we assume an already
   globalized trace */
bool is_globalized = false;

elg_ui4 elg_version;     /* EPILOG file format version */

/* process id |-> global location id of (-,-,p,0) */
map<elg_ui4, elg_ui4> pid2glid;
/* global communicator id |-> list of locations */
map<elg_ui4, vector<elg_ui4> > gcid2lid;

/* NOTE:
     During the merge, SELF-encoded communicators will be expanded (i.e., one
     communicator per rank created). Therefore, a mapping from the global
     communicator id to the "expanded ids" is required. Since RMA windows can
     refer to such communicators, a similar mapping is required for windows
     as well. */
/* global communicator id |-> list of "expanded global" communicator ids */
map<elg_ui4, vector<elg_ui4> > gcid2egcid;
/* global window id |-> list of "expanded global" window ids */
map<elg_ui4, vector<elg_ui4> > gwid2egwid;

vector<Group*>  group_q;    /* MPI group queue */
vector<Comm*>   comm_q;     /* MPI communicator queue */
vector<Window*> win_q;      /* MPI RMA window queue */
vector<ElgRec*> cart_q;     /* Cartesian topology record queue */

/* vampirtrace event placement ? */
int elg_vt_mode = epk_str2int(epk_get(ELG_VT_MODE));

/* number of performance metrics */
elg_ui1 metc = 0; 

/* number of communicators */
elg_ui4 comc = 0;

/* current offset between access epochs */
/* NOTE: With weak synchronization a put can belong to a later
         access epoch, than a post on the corresponding window 
         on the target location. As the complete-event returns
         on the putting process redardless of the status of the
         corresponding post-event. */
map<elg_ui4, map<pair<elg_ui4, elg_ui4>,elg_ui4> > access_epoch;
map<elg_ui4, map<pair<elg_ui4, elg_ui4>,elg_ui4> > exposure_epoch;

/* map of double ended queues to hold all remote events to be shifted */
/* evt_put_q[(window_location,window_id)][(source_location,access_epoch)] -> ElgRec* */
map<pair<elg_ui4,elg_ui4>,map<pair<elg_ui4,elg_ui4>, deque<ElgRec*> > > evt_put_q;
map<pair<elg_ui4,elg_ui4>,map<pair<elg_ui4,elg_ui4>, deque<ElgRec*> > > evt_get_q;

/* region IDs of MPI-2 RMA synchronization functions */
elg_ui4 rid_mpi_win_complete = ELG_NO_ID;
elg_ui4 rid_mpi_win_fence    = ELG_NO_ID;
elg_ui4 rid_mpi_win_test     = ELG_NO_ID;
elg_ui4 rid_mpi_win_unlock   = ELG_NO_ID;
elg_ui4 rid_mpi_win_wait     = ELG_NO_ID;


/*
 *---------------------------------------------------------------------------------
 * Definition of local functions
 *---------------------------------------------------------------------------------
 */

// Usage: elg_merge <archive>

int main(int argc, char** argv)
{
  /* check # of arguments */
  if (argc != 2)
    elg_error_msg("Wrong # of arguments\n"
                  "Usage: elg_merge <archive>");

  /* merge */
  do_merge(argv[1]);

  return 0;
}


void do_merge(const char* fprefix)
{
  /* check whether mapping file exists */
  char* dir_name = epk_archive_filename(EPK_TYPE_DIR, fprefix);
  char* map_name = (char*)calloc(strlen(dir_name) + 10, sizeof(char));
  sprintf(map_name, "%s/epik.map", dir_name);
  struct stat sbuf;
  if (stat(map_name, &sbuf) != 0) {
    elg_cntl_msg("No mapping file found -- Assuming already globalized trace");
    is_globalized = true;
  }
  free(dir_name);
  free(map_name);

  /* multi-pass merge */
  process_defs(fprefix);
  if (!is_globalized)
    process_maps(fprefix);
  process_traces_pass1();
  finalize_defs();
  process_traces_pass2();

  /* close outfile */
  ElgOut_close(of);
}


void process_defs(const char* fprefix)
{
  /* name IDs of MPI-2 RMA synchronization functions */
  elg_ui4 nid_mpi_win_complete = ELG_NO_ID;
  elg_ui4 nid_mpi_win_fence    = ELG_NO_ID;
  elg_ui4 nid_mpi_win_test     = ELG_NO_ID;
  elg_ui4 nid_mpi_win_unlock   = ELG_NO_ID;
  elg_ui4 nid_mpi_win_wait     = ELG_NO_ID;

  elg_cntl_msg("Processing definition file...");

  /* open definition file */
  char* esd_name = epk_archive_filename(EPK_TYPE_ESD, fprefix);
  ElgIn* defs = ElgIn_open(esd_name);
  if (!defs)
    elg_error_msg("Cannot open %s file \"%s\"",
                  epk_archive_filetype(esd_name), esd_name);
  free(esd_name);

  /* Determine EPILOG file format version */
  elg_version = ElgIn_get_version(defs);

  /* open outfile */
  char* elg_name = epk_archive_filename(EPK_TYPE_ELG, fprefix);
  of = ElgOut_open(elg_name, ElgIn_get_byte_order(defs), ELG_UNCOMPRESSED);
  if (!of)
    elg_error_msg("Cannot create %s file \"%s\"", 
                  epk_archive_filetype(elg_name), elg_name);
  free(elg_name);

  /* process definitions */
  ElgRec* rec;
  while ((rec = ElgIn_read_record(defs)) != NULL) {
    switch (ElgRec_get_type(rec)) {
      /* Determine name IDs of MPI-2 RMA synchronization functions */
      case ELG_STRING:
        {
          elg_ui4 strid = ElgRec_read_ui4(rec);
          elg_ui1 cntc  = ElgRec_read_ui1(rec);

          /* All function names we are looking for fit into a single record */
          if (cntc != 0)
            break;

          /* Compare strings */
          char* name = ElgRec_read_string(rec);
          if (0 == strcmp(name, "MPI_Win_complete"))
            nid_mpi_win_complete = strid;
          else if (0 == strcmp(name, "MPI_Win_fence"))
            nid_mpi_win_fence = strid;
          else if (0 == strcmp(name, "MPI_Win_test"))
            nid_mpi_win_test = strid;
          else if (0 == strcmp(name, "MPI_Win_unlock"))
            nid_mpi_win_unlock = strid;
          else if (0 == strcmp(name, "MPI_Win_wait"))
            nid_mpi_win_wait = strid;
        }
        break;

      /* Determine region IDs of MPI-2 RMA synchronization functions */
      case ELG_REGION:
        {
          elg_ui4 rid = ElgRec_read_ui4(rec);
          elg_ui4 nid = ElgRec_read_ui4(rec);

          /* Compare name IDs & store region IDs */
          if (nid == nid_mpi_win_complete)
            rid_mpi_win_complete = rid;
          else if (nid == nid_mpi_win_fence)
            rid_mpi_win_fence = rid;
          else if (nid == nid_mpi_win_test)
            rid_mpi_win_test = rid;
          else if (nid == nid_mpi_win_unlock)
            rid_mpi_win_test = rid;
          else if (nid == nid_mpi_win_wait)
            rid_mpi_win_wait = rid;
        }
        break;

      /* count number of defined HWC metrics */
      case ELG_METRIC:
        {
          metc++;
        }
        break;

      /* groups have to be stored for the communicator record translation */
      case ELG_MPI_GROUP:
        {
          /* Create group data structure */
          Group* group = new Group;
          group->id    = ElgRec_read_ui4(rec);
          group->mode  = ElgRec_read_ui1(rec);
          group->size  = ElgRec_read_ui4(rec);
          group->ranks = NULL;
          group->cid   = ELG_NO_ID;

          if ((group->mode & ELG_GROUP_SELF) != ELG_GROUP_SELF) {
            group->ranks = new elg_ui4[group->size];

            /* Special case: GROUP_WORLD */
            if (group->mode & ELG_GROUP_WORLD) {
              for (elg_ui4 i = 0; i < group->size; ++i)
                group->ranks[i] = i;
            } else {
              /* Read rank mapping */
              elg_ui4 pos   = 0;
              elg_ui4 block = min(group->size,
                                  static_cast<elg_ui4>((255 - 2 * sizeof(elg_ui4) - sizeof(elg_ui1))
                                                       / sizeof(elg_ui4)));
              for (elg_ui4 i = 0; i < block; ++i)
                group->ranks[pos++] = ElgRec_read_ui4(rec);

              /* process continuation records */
              while (pos < group->size) {
                ElgRec* contRec = ElgIn_read_record(defs);

                if (ElgRec_get_type(contRec) != ELG_MPI_GROUP_CNT)
                  elg_error_msg("MPI_GROUP_CNT record expected!");

                block = min(static_cast<elg_ui4>(255 / sizeof(elg_ui4)), group->size - pos);
                for (elg_ui4 i = 0; i < block; ++i)
                  group->ranks[pos++] = ElgRec_read_ui4(contRec);

                ElgRec_free(contRec);
              }
            }
          }

          /* enqueue group */
          group_q.push_back(group);
        }
        ElgRec_free(rec);
        continue;

      /* MPI communicator records introduced with EPILOG 1.8 */
      case ELG_MPI_COMM_REF:
        {
          elg_ui4 cid = ElgRec_read_ui4(rec);
          elg_ui4 gid = ElgRec_read_ui4(rec);

          create_comm(group_q[gid], cid, 0);
          comc++;
        }
        ElgRec_free(rec);
        continue;

      /* communicators have to be stored since the ELG_GROUP_FLAG might */
      /* need an update... */
      case ELG_MPI_COMM:
        {
          elg_ui4 cid  = ElgRec_read_ui4(rec);
          elg_ui1 mode = ElgRec_read_ui1(rec);
          elg_ui4 grpc = ElgRec_read_ui4(rec);

          /* expand SELF-encoded communicators */
          if (mode & ELG_GROUP_SELF) {
            size_t numRanks = pid2glid.size();
            for (elg_ui4 rank = 0; rank < numRanks; ++rank) {
              /* create & initialize communicator data structure */
              Comm* comm = new Comm;
              comm->id   = comm_q.size();
              comm->mode = mode & ELG_GROUP_FLAG;
              comm->grpc = (numRanks + 7) / 8;
              comm->grpv = (elg_ui1*)calloc(comm->grpc, sizeof(elg_ui1));
              comm->gid  = ELG_NO_ID;

              /* set rank bit */
              comm->grpv[rank / 8] |= 1 << (rank % 8);

              /* build global communicator |-> location mapping */
              gcid2lid[comm->id].push_back(rank);

              /* store global id |-> "expanded global" id */
              gcid2egcid[cid].push_back(comm->id);

              /* enqueue communicator */
              comm_q.push_back(comm);
            }
          }

          /* handle all other communicators */
          else {
            /* create & initialize communicator data structure */
            Comm* comm = new Comm;
            comm->id   = comm_q.size();
            comm->mode = mode & ELG_GROUP_FLAG;
            comm->grpc = grpc;
            comm->grpv = (elg_ui1*)malloc(comm->grpc * sizeof(elg_ui1));
            comm->gid  = ELG_NO_ID;

            /* read bitstring */
            elg_ui4 pos   = 0;
            elg_ui4 block = min(comm->grpc, static_cast<elg_ui4>(255 - 2 * sizeof(elg_ui4) - sizeof(elg_ui1)));
            for (elg_ui4 i = 0; i < block; ++i)
              comm->grpv[pos++] = ElgRec_read_ui1(rec);

            /* process continuation records */
            while (pos < comm->grpc) {
              ElgRec* contRec = ElgIn_read_record(defs);

              if (ElgRec_get_type(contRec) != ELG_MPI_COMM_CNT)
                elg_error_msg("MPI_COMM_CNT record expected!");

              block = min(255u, comm->grpc - pos);
              for (elg_ui4 i = 0; i < block; ++i)
                comm->grpv[pos++] = ElgRec_read_ui1(contRec);

              ElgRec_free(contRec);
            }

            /* build global communicator |-> location mapping */
            for (elg_ui4 i = 0; i < comm->grpc*8; ++i)
              if (((1 << (i % 8)) & comm->grpv[i/8]))
                gcid2lid[comm->id].push_back(i);

            /* store global id |-> "expanded global" id */
            gcid2egcid[cid].push_back(comm->id);

            /* enqueue communicator */
            comm_q.push_back(comm);
          }
        }
        ElgRec_free(rec);
        continue;

      /* Cartesian topologies need to be stored since they reference */
      /* communicators (see above) and must therefore be defined afterwards */
      case ELG_CART_TOPOLOGY:
        {
          ElgRec_seek(rec, sizeof(elg_ui4));
          elg_ui4 gcid = ElgRec_read_ui4(rec);

          /* adjust communicator ID */
          if (gcid != ELG_NO_ID) {
            elg_ui4 egcid = gcid2egcid[gcid][0];
            ElgRec_write_ui4(rec, egcid, sizeof(elg_ui4));
          }

          /* enqueue Cartesian topology record */
          cart_q.push_back(rec);          
        }
        continue;

      /* Cartesian topology coordinates need to be stored since they */
      /* reference Cartesian topology definitions (see above) and must */
      /* thereforte be defined afterwards */
      case ELG_CART_COORDS:
        {
          /* enqueue Cartesian topology coordinate record */
          cart_q.push_back(rec);          
        }
        continue;

      /* RMA windows need to be stored since they reference communicators */
      /* (see above) and must therefore be defined afterwards */
      case ELG_MPI_WIN:
        {
          elg_ui4 wid = ElgRec_read_ui4(rec);
          elg_ui4 cid = ElgRec_read_ui4(rec);

          /* handle windows using communicators with 'SELF' encoding */
          if (gcid2egcid[cid].size() != 1) {
            size_t numRanks = pid2glid.size();
            for (elg_ui4 rank = 0; rank < numRanks; ++rank) {
              /* create & initialize RMA window data structure */
              Window* win = new Window;
              win->id  = win_q.size();
              win->cid = gcid2egcid[cid][rank];

              /* store global id |-> "expanded global" id */
              gwid2egwid[wid].push_back(win->id);

              /* enqueue RMA window */
              win_q.push_back(win);
            }
          }

          /* handle all other windows */
          else {
            /* create & initialize RMA window data structure */
            Window* win = new Window;
            win->id  = win_q.size();
            win->cid = gcid2egcid[cid][0];

            /* store global id |-> "expanded global" id */
            gwid2egwid[wid].push_back(win->id);

            /* enqueue RMA window */
            win_q.push_back(win);
          }
        }
        ElgRec_free(rec);
        continue;

      /* build up rank |-> location mapping & open input file */
      case ELG_LOCATION:
        {
          elg_ui4 lid = ElgRec_read_ui4(rec);
          ElgRec_seek(rec, 3 * sizeof(elg_ui4));
          elg_ui4 pid = ElgRec_read_ui4(rec);
          elg_ui4 tid = ElgRec_read_ui4(rec);

          if (tid == 0) {
            pid2glid[is_globalized ? lid : pid] = lid;
            infilev.push_back(new Infile(fprefix, pid));
          }
        }
        break;

      /* map offsets are useless for merged trace files => ignored */
      case ELG_MAP_OFFSET:
        {
          ElgRec_free(rec);
        }
        continue;

      default:
        break;
    }

    ElgOut_write_record(of, rec);
    ElgRec_free(rec);
  }

  /* close definition file */
  ElgIn_close(defs);
}


void process_maps(const char* fprefix)
{
  elg_cntl_msg("Processing mapping file...");

  char* dir_name = epk_archive_filename(EPK_TYPE_DIR, fprefix);
  char* map_name = (char*)calloc(strlen(dir_name) + 10, sizeof(char));
  sprintf(map_name, "%s/epik.map", dir_name);

  /* open mapping file */
  ElgIn* maps = ElgIn_open(map_name);
  if (!maps) {
    elg_error_msg("Cannot open mapping file \"%s\"", map_name);
    free(dir_name);
    free(map_name);
    return;
  }

  /* process mappings */
  elg_ui4 rank = 0;
  ElgRec* rec;
  while ((rec = ElgIn_read_record(maps)) != NULL) {
    switch (ElgRec_get_type(rec)) {
      /* beginning of a new mapping file section */
      case ELG_MAP_SECTION:
        {
          rank = ElgRec_read_ui4(rec);
          if (rank >= infilev.size())
            elg_error_msg("Contents of mapping file does not match specified number of input files");
        }
        break;

      /* clock synchronization offsets */
      case ELG_OFFSET:
        {
          Infile* in = infilev[rank];

          if (in->offcnt >= 2)
            elg_error_msg("Found more than two offset records!");

          in->rank = rank;
          in->ltime[in->offcnt]  = ElgRec_read_d8(rec);
          in->offset[in->offcnt] = ElgRec_read_d8(rec);
          in->offcnt++;
        }
        break;

      /* identifier mapping table */
      case ELG_IDMAP:
        {
          Infile* in = infilev[rank];

          /* read table header */
          elg_ui1 type  = ElgRec_read_ui1(rec);
          elg_ui1 mode  = ElgRec_read_ui1(rec);
          elg_ui4 count = ElgRec_read_ui4(rec);

          /* select map */
          EpkIdMap** map    = NULL;
          bool       ignore = false;
          switch (type) {
            case ELG_LOCATION:
              map = &in->locations;
              break;
            case ELG_REGION:
              map = &in->regions;
              break;
            case ELG_CALL_SITE:
              map = &in->callsites;
              break;
            case ELG_MPI_COMM:
              map = &in->comms;
              break;
            case ELG_MPI_GROUP:
              map = &in->groups;
              break;
            case ELG_MPI_WIN:
              map = &in->windows;
              break;
            case ELG_CALL_PATH:
              // Ignore call-path mapping for backwards compatibility
              ignore = true;
              break;
            default:
              elg_error_msg("Unknown mapping table type!");
              break;
          }

          /* Something to ignore? */
          if (ignore) {
            /* Process continuation records */
            count -= min(count, 62u);
            while (count > 0) {
              ElgRec* contRec = ElgIn_read_record(maps);
              if (ElgRec_get_type(contRec) != ELG_IDMAP_CNT)
                elg_error_msg("IDMAP_CNT record expected!");
              ElgRec_free(contRec);
              count -= min(count, 63u);
            }
            break;
          }

          /* create map */
          if (*map)
            elg_error_msg("Mapping table already defined!");
          if (mode == ELG_MAP_DENSE)
            *map = epk_idmap_create(EPK_IDMAP_DENSE, count);
          else
            *map = epk_idmap_create(EPK_IDMAP_SPARSE, count/2);

          /* buffer data */
          vector<elg_ui4> buffer;
          buffer.resize(count);

          elg_ui4 pos   = 0;
          elg_ui4 block = min(count, 62u);
          for (elg_ui4 i = 0; i < block; ++i)
            buffer[pos++] = ElgRec_read_ui4(rec);

          /* read continuation records */
          while (pos < count) {
            ElgRec* contRec = ElgIn_read_record(maps);

            if (ElgRec_get_type(contRec) != ELG_IDMAP_CNT)
              elg_error_msg("IDMAP_CNT record expected!");

            block = min(count - pos, 63u);
            for (elg_ui4 i = 0; i < block; ++i)
              buffer[pos++] = ElgRec_read_ui4(contRec);

            ElgRec_free(contRec);
          }

          /* store mapping */
          if (mode == ELG_MAP_DENSE)
            for (size_t i = 0; i < buffer.size(); ++i)
              epk_idmap_add(*map, i, buffer[i]);
          else
            for (size_t i = 0; i < buffer.size(); i += 2)
              epk_idmap_add(*map, buffer[i], buffer[i + 1]);
        }
        break;

      default:
        break;
    }

    ElgRec_free(rec);
  }

  /* release resources */
  free(dir_name);
  free(map_name);
}


void process_traces_pass1()
{
  /* no windows => no "used as group" action needed */
  if (win_q.size() == 0)
    return;

  /* adjust "used as group" flag of communicators */
  size_t count = infilev.size();
  for (size_t i = 0; i < count; ++i) {
    ElgRec* rec;
    Infile* in = infilev[i];
    while ((rec = ElgIn_localize_record(in->inf, 0)) != NULL) {
      if (ElgRec_get_type(rec) == ELG_MPI_WINEXIT) {
        ElgRec_seek(rec, sizeof(elg_ui4) + sizeof(elg_d8) +
                         metc * sizeof(elg_ui8) + sizeof(elg_ui4));
        elg_ui4 cid = ElgRec_read_ui4(rec);
        comm_q[in->lgid2gcid(cid)]->mode |= ELG_GROUP_FLAG;
      }
      ElgRec_free(rec);
    }
    in->rewind();
  }
}


void finalize_defs()
{
  /* write communicator records */
  size_t count = comm_q.size();
  for (size_t i = 0; i < count; ++i) {
    Comm* comm = comm_q[i];
    ElgOut_write_MPI_COMM(of, comm->id, comm->mode, comm->grpc, comm->grpv);
  }

  /* write topology records */
  count = cart_q.size();
  for (size_t i = 0; i < count; ++i) {
    ElgOut_write_record(of, cart_q[i]);
    ElgRec_free(cart_q[i]);
  }
  cart_q.clear();

  /* write RMA window records */
  count = win_q.size();
  for (size_t i = 0; i < count; ++i) {
    Window* win = win_q[i];
    ElgOut_write_MPI_WIN(of, win->id, win->cid);
  }

  /* finalize definition part */
  ElgOut_write_LAST_DEF(of);
}


void process_traces_pass2()
{
  elg_cntl_msg("Merging trace files...");

  vector<        ElgRec*  > recv (infilev.size());
  vector< vector<ElgRec*> > attrv(infilev.size());

  double start_time = 0.0;
  double timestamp;
  bool is_first=true;
  
  int qed  = 0;
  int dqed = 0;
  
  map<pair<elg_ui4,elg_ui4>, bool> lock_list;
 
  /* save last entered region on each location */
  map<elg_ui4,elg_ui4> entered_region;
  
  /* rewind */
  for_each(infilev.begin(), infilev.end(), 
           mem_fun(&Infile::rewind)); 
 
  /* init */
  transform(infilev.begin(), infilev.end(),
            recv.begin(),
            mem_fun(&Infile::next_rec));

  while(1)
    {
      ElgRec* min_rec    = NULL;
      int     min_pos    = 0;
      elg_ui4 real_loc   = 0;
      elg_ui4 real_dest  = 0;
      elg_ui4 shift_loc  = 0;
      elg_ui4 shift_wid  = 0;
      elg_ui4 cur_wid    = 0;
      elg_ui4 cur_cid    = 0;
      elg_ui4 remote_loc = 0;
      bool shift_event   = false;
      bool is_remote     = false;
      bool force_dequeue = false;
      bool is_rma_sync_exit = false;
      
      for (size_t i = 0; i < infilev.size() ; i++)
	{
	  if (recv[i] == NULL)
	      continue;

          /* store attributes */
          while (ElgRec_is_attribute(recv[i])) {
            attrv[i].push_back(recv[i]);
            recv[i] = infilev[i]->next_rec();
          }

	  /* select first available record */
	  if (min_rec == NULL)
	    {
	      min_rec = recv[i];	      
	      min_pos = i;
	    }
	  /* compare timestamps: looking for oldest (minimum time stamp) record */     
	  else
	    {
	      double first  = ElgRec_read_time(min_rec);
	      double second = ElgRec_read_time(recv[i]);
	      
	      if (first > second)
		{
		  min_rec = recv[i];
		  min_pos = i;
		}
	    }
	}
      /* finish if all records are processed */
      if (min_rec == NULL)
	{
	  if (!elg_vt_mode) 
            {
	      elg_cntl_msg("Queued: %i Dequeued: %i", qed, dqed); 

              if ((qed != dqed))
                elg_error_msg("There are still transfer events left in the queue. Merged trace is broken!");
            }

          /* close infiles */
          for (size_t i = 0; i < infilev.size(); ++i)
            delete infilev[i];

          return;
	}  

      /* replace oldest record by new record */
      recv[min_pos] = infilev[min_pos]->next_rec();
      
      /* adjust time stamps in a way that the first event has time stamp 0.0 */
      timestamp = ElgRec_read_time(min_rec);
      
      if (is_first)
	{
	  start_time = timestamp;
	  is_first = false;
	}
      ElgRec_write_time(min_rec, timestamp - start_time);

      /* get event location */
      ElgRec_seek( min_rec, 0 );
      elg_ui4 cur_lid = ElgRec_read_ui4( min_rec );
      ElgRec_seek( min_rec, sizeof(elg_ui4)+sizeof(elg_d8) );

      /* check if record needs to be manipulated */
      switch ( ElgRec_get_type(min_rec) )
      {
        case ELG_MPI_GET_1TS_REMOTE:
        {
          /* get real location and destination */
          real_dest = cur_lid;
          real_loc  = ElgRec_read_ui4( min_rec );
          
            /* create and insert ELG_GET_1TO-record into event stream */
            ElgRec *newrec = ElgRec_create_empty( sizeof(elg_ui4)+  /* lid */
                                                  sizeof(elg_d8) +  /* time */
                                                  sizeof(elg_ui4),  /* rmaid */
                                                  ELG_MPI_GET_1TO);
            /* save lid to new record */
            ElgRec_write_ui4(newrec, cur_lid, 0);
            /* save time stamp to new record */
            ElgRec_write_time(newrec, timestamp - start_time);
            /* get rmaid an save it to new record */
            ElgRec_seek(min_rec, sizeof(elg_ui4)+sizeof(elg_d8)
                                +sizeof(elg_ui4)+sizeof(elg_ui4));
            elg_ui4 cur_rmaid = ElgRec_read_ui4(min_rec);
            ElgRec_write_ui4(newrec, cur_rmaid, sizeof(elg_ui4)+sizeof(elg_d8));
            
            /* save to event stream */
            ElgOut_write_record(of, newrec);
            /* free record */
            ElgRec_free(newrec);
            /* Done with ELG_GET_1TO-record */
          
          /* change type and correct location and destination */
          ElgRec_set_type(min_rec, ELG_MPI_GET_1TS);
          ElgRec_write_ui4(min_rec, real_loc, 0);
          ElgRec_write_ui4(min_rec, real_dest, sizeof(elg_ui4)+sizeof(elg_d8));
          break;
        }
        case ELG_GET_1TS_REMOTE:
        {
          /* get real location and destination */
          real_dest = cur_lid;
          real_loc  = ElgRec_read_ui4( min_rec );

          /* change type and correct location and destination */
          ElgRec_set_type(min_rec, ELG_GET_1TS);
          ElgRec_write_ui4(min_rec, real_loc, 0);
          ElgRec_write_ui4(min_rec, real_dest, sizeof(elg_ui4)+sizeof(elg_d8));
          break;
        }
        case ELG_MPI_GET_1TE:
        {
          /* declare this event to be shifted, if desired */
          shift_event = true;
          is_remote   = false;
          
          /* shift on the location the GET was issued */
          shift_loc = cur_lid;
          ElgRec_seek( min_rec, sizeof(elg_ui4) + sizeof(elg_d8) + sizeof(elg_ui4) );
          shift_wid = ElgRec_read_ui4( min_rec );
          break;
        }
        case ELG_MPI_PUT_1TE_REMOTE:
        {
          /* declare this event to be shifted, if desired */
          shift_event = true;
          is_remote   = true;
          
          /* get real location and destination */
          real_dest = cur_lid;
          real_loc  = ElgRec_read_ui4( min_rec );
          
          /* shift on the real location the GET will be set to */
          shift_loc = real_loc;
          shift_wid = ElgRec_read_ui4( min_rec );
          
          /* change type and correct location and destiantion */
          ElgRec_set_type( min_rec, ELG_MPI_PUT_1TE );
          ElgRec_write_ui4( min_rec, real_loc, 0 );
          ElgRec_write_ui4( min_rec, real_dest, sizeof(elg_ui4) + sizeof(elg_d8) );
          break;
        }
        case ELG_PUT_1TE_REMOTE:
        {
          /* get real location and destination */
          real_dest = cur_lid;
          real_loc  = ElgRec_read_ui4( min_rec );
          
          /* change type and correct location and destiantion */
          ElgRec_set_type( min_rec, ELG_PUT_1TE );
          ElgRec_write_ui4( min_rec, real_loc, 0 );
          ElgRec_write_ui4( min_rec, real_dest, sizeof(elg_ui4) + sizeof(elg_d8) );
          break;
        }
        case ELG_ENTER :
        case ELG_ENTER_CS :
        {
          /* save last region that was entered */
          entered_region[cur_lid] = ElgRec_read_ui4(min_rec);
          break;
        }
        case ELG_MPI_WIN_LOCK:
        {
          ElgRec_seek(min_rec,sizeof(elg_ui4)
                             +sizeof(elg_d8)
                             +sizeof(elg_ui4));
          cur_wid          = ElgRec_read_ui4(min_rec);
          lock_list[make_pair(cur_lid,cur_wid)] = true;
          break;
        }
        case ELG_MPI_WIN_UNLOCK :
        {
          is_rma_sync_exit = true;
          remote_loc       = ElgRec_read_ui4(min_rec);
          cur_wid          = ElgRec_read_ui4(min_rec);
          force_dequeue    = true;
          lock_list[make_pair(cur_lid,cur_wid)] = false;
          break;
        }
        case ELG_MPI_WINEXIT :
        {
          /* jump to wid */
          elg_ui4 newpos = sizeof(elg_ui4) + sizeof(elg_d8)
                         + metc * sizeof(elg_ui8);
          ElgRec_seek(min_rec, newpos);
         
          cur_wid       = ElgRec_read_ui4(min_rec);
          cur_cid       = ElgRec_read_ui4(min_rec);
          elg_ui1 synex = ElgRec_read_ui1(min_rec);
         
          if (synex)
            is_rma_sync_exit = true;

          /* track access/exposure epoch skews */
          switch (entered_region[cur_lid])
            {
              case EPK__MPI_WIN_START:
                {
                  for( vector<elg_ui4>::iterator it=gcid2lid[cur_cid].begin(); 
                       it!=gcid2lid[cur_cid].end(); ++it )
                    {
                      access_epoch[cur_lid][make_pair(*it,cur_wid)]++;
                      if (!elg_vt_mode)
                        elg_cntl_msg("Access epoch for location %i on window (%i,%i) incremented to %i",
                                     cur_lid, *it, cur_wid, 
                                     access_epoch[cur_lid]
                                                 [make_pair(*it,cur_wid)]);
                    }
                  break;
                } 
              case EPK__MPI_WIN_POST:
                {
                  for( vector<elg_ui4>::iterator it=gcid2lid[cur_cid].begin(); 
                       it!=gcid2lid[cur_cid].end(); ++it )
                    {
                      exposure_epoch[*it][make_pair(cur_lid,cur_wid)]++;
                      if (!elg_vt_mode)
                        elg_cntl_msg("Exposure epoch for location %i on window (%i,%i) incremented to %i",
                                     *it, cur_lid, cur_wid, 
                                     exposure_epoch[*it]
                                                   [make_pair(cur_lid,cur_wid)]); 
                    }
                  break;
                }
          }
          break;
        }
        case ELG_MPI_WINCOLLEXIT :
        {
          /* jump to wid */
          elg_ui4 newpos = sizeof(elg_ui4) + sizeof(elg_d8)
                         + metc * sizeof(elg_ui8);
          ElgRec_seek( min_rec, newpos );
          
          is_rma_sync_exit = true;
          cur_wid = ElgRec_read_ui4(min_rec);
          break;
        }  
      } 
      
      /* is vt_mode turned on and is it an event to be queued? */
      if (!elg_vt_mode && shift_event)
        {
          /* create a new copy to avoid losing the record 
             on a buffer refill */
          ElgRec* newrec = ElgRec_copy( min_rec );
          
          /* create winloc for improved readability */
          pair<elg_ui4,elg_ui4> winloc;
          winloc = make_pair(shift_loc,shift_wid);
          
          /* queue event */
          if (is_remote)
          {
            evt_put_q[winloc]
                     [make_pair(cur_lid,access_epoch[cur_lid][winloc])]
                     .push_back(newrec);
          }
          else
          {
            evt_get_q[winloc]
                     [make_pair(cur_lid,access_epoch[cur_lid][winloc])]
                     .push_back(newrec);
          }

          elg_cntl_msg("Queued message from location %i for window (%i,%i)",
                      cur_lid, shift_loc, shift_wid);
          qed++; /* counting how many entries have been queued */
          ElgRec_free(min_rec);
        }  
      else
        {
          int res = 0;
          if (!elg_vt_mode && 
              is_rma_sync_exit &&
              (res = rma_sync_type( entered_region[cur_lid]) ) )
            {
              pair<elg_ui4,elg_ui4> winloc;
              pair<elg_ui4,elg_ui4> loc_queue;
              switch (res)
              {
                case ELG_RMA_DEQUEUE_BOTH :
                case ELG_RMA_DEQUEUE_LOCAL :
                  {
                    /* 
                     * this will flush remote events on remote location
                     * without special event on that location.
                     * -- this is needed for a Put/Unlock situation 
                     */
                    if (force_dequeue == true)
                      {
                        /* 
                         * if an unlock has not been set for this process
                         * yet, the record has to be skipped and dequeued
                         * with a later unlock 
                         */
                        winloc = make_pair(remote_loc, cur_wid);
                        loc_queue = make_pair(cur_lid,access_epoch[cur_lid][winloc]);
                        deque<ElgRec*> evt_skipped_q;
                        while (evt_put_q[winloc][loc_queue].size() )
                          {
                            elg_cntl_msg("Dequeued message from %i for window (%i,%i)",
                                         cur_lid, remote_loc, cur_wid);

                            /* get element from queue */ 
                            ElgRec* rec = evt_put_q[winloc][loc_queue].at(0);
                            evt_put_q[winloc][loc_queue].pop_front();
                            
                            /* get event location */
                            ElgRec_seek(rec, sizeof(elg_ui4)+sizeof(elg_d8));
                            elg_ui4 origin = ElgRec_read_ui4(rec);
                            elg_ui4 window = ElgRec_read_ui4(rec);
                            if (lock_list[make_pair(origin,window)] == true)
                            {
                              /* 
                               * record is not yet unlocked, thus it
                               * has to be skipped and requeued.
                               */
                               evt_skipped_q.push_back(rec);
                            }
                            else
                            {
                              /* correct time stamp */
                              ElgRec_write_time(rec, timestamp - start_time);
                              /* write rma op record */
                              ElgOut_write_record(of, rec);
                              ElgRec_free(rec);
                              dqed++; /* counting how many entries have been dequeued */
                            }
                          }
                        if (evt_skipped_q.size())
                        {
                          /* requeue all skipped records to normal queue */
                          evt_put_q[winloc][loc_queue] = evt_skipped_q;
                        }
                        else
                        {
                          /* clear empty map entry */
                          evt_put_q[winloc].erase(loc_queue);
                        }
                      } 
                    /* 
                     * dequeue collected events for 
                     * current wid on this location 
                     */
                    winloc = make_pair(cur_lid, cur_wid);
                    for( vector<elg_ui4>::iterator it=gcid2lid[cur_cid].begin(); 
                         it!=gcid2lid[cur_cid].end(); ++it )
                      {
                        loc_queue = make_pair(*it,exposure_epoch[*it][winloc]);
                        while (evt_get_q[winloc][loc_queue].size()) 
                          {
                            elg_cntl_msg("Dequeued message from %i for window (%i,%i)",
                                         cur_lid, cur_lid, cur_wid);

                            dqed++; /* counting how many entries have been dequeued */
                            
                            /* get element from queue */ 
                            ElgRec* rec = evt_get_q[winloc][loc_queue].at(0);
                            evt_get_q[winloc][loc_queue].pop_front();
                            
                            /* correct time stamp */
                            ElgRec_write_time(rec, timestamp - start_time);
                            /* write rma op record */
                            ElgOut_write_record(of, rec);
                            ElgRec_free(rec);
                          }
                        /* clear empty map entry */
                        evt_get_q[winloc].erase(loc_queue);
                      }  
                    if (res != ELG_RMA_DEQUEUE_BOTH) break; 
                  }
                case ELG_RMA_DEQUEUE_REMOTE :
                  {
                    winloc = make_pair(cur_lid, cur_wid);
                    /* dequeue collected events for current wid on min_pos */
                    for( vector<elg_ui4>::iterator it=gcid2lid[cur_cid].begin(); 
                         it!=gcid2lid[cur_cid].end(); ++it )
                      {
                        loc_queue = make_pair(*it,exposure_epoch[*it][winloc]);
                        while (evt_put_q[winloc][loc_queue].size()) 
                        {
                          elg_cntl_msg("Dequeued message from %i for window (%i,%i)",
                                       cur_lid, cur_lid, cur_wid);

                          dqed++; /* counting how many entries have been dequeued */

                          /* get element from queue */ 
                          ElgRec* rec = evt_put_q[winloc][loc_queue].at(0);
                          evt_put_q[winloc][loc_queue].pop_front();
                          
                          /* correct time stamp */
                          ElgRec_write_time(rec, timestamp - start_time);
                          /* write rma op record */
                          ElgOut_write_record(of, rec);
                          ElgRec_free(rec);
                        }
                        /* clear empty map entry */
                        evt_put_q[winloc].erase(loc_queue);
                      }
                    break;
                  }
                }
                
                /* write exit record */
                ElgOut_write_record(of, min_rec);
                ElgRec_free(min_rec);
            }
          else
            { 
              /* write w/o any queue handling 
                 -- this is used on ELG_VT_MODE=yes */

              /* write attributes, if any */
              for (vector<ElgRec*>::iterator it = attrv[min_pos].begin(); it != attrv[min_pos].end(); ++it) {
                ElgOut_write_record(of, *it);
                ElgRec_free(*it);
              }

              attrv[min_pos].clear();

              ElgOut_write_record(of, min_rec);
              ElgRec_free(min_rec);
            }  
        }
    }
}


static int check_locationless(ElgIn* in)
{
  ElgRec* rec = ElgIn_read_record(in);
  elg_ui1 type = ElgRec_get_type(rec);
  elg_ui1 size = ElgRec_get_length(rec);
  const elg_ui1 Enter_size = 12 + (metc * sizeof(elg_ui8));
  int locationless = ((type == ELG_ENTER) && (size == Enter_size));
  ElgIn_rewind(in);
  ElgRec_free(rec);
  return locationless;
}


static int rma_sync_type(elg_ui4 rid)
{
  if (rid == rid_mpi_win_fence)
  {
    return ELG_RMA_DEQUEUE_BOTH; 	/* remote _and_ local events */
  }
  else if (rid == rid_mpi_win_test ||
           rid == rid_mpi_win_wait)
  {
    return ELG_RMA_DEQUEUE_REMOTE; 	/* remote only */
  }
  else if (rid == rid_mpi_win_complete ||
           rid == rid_mpi_win_unlock)
  {
    return ELG_RMA_DEQUEUE_LOCAL; 	/* local only */
  }

  return ELG_RMA_DEQUEUE_NONE; 	/* no sync */
}


static void create_comm(Group* group, elg_ui4 cid, elg_ui1 mode)
{
  size_t numRanks = pid2glid.size();

  /* Adjust mode flag */
  mode |= group->mode;

  if (mode & ELG_GROUP_SELF) {
    /* Expand SELF-like communicators */
    for (elg_ui4 rank = 0; rank < numRanks; ++rank) {
      /* create & initialize communicator data structure */
      Comm* comm = new Comm;
      comm->id   = comm_q.size();
      comm->mode = mode & ELG_GROUP_FLAG;
      comm->grpc = (numRanks + 7) / 8;
      comm->grpv = (elg_ui1*)calloc(comm->grpc, sizeof(elg_ui1));
      comm->gid  = ELG_NO_ID;

      /* set rank bit */
      comm->grpv[rank / 8] |= 1 << (rank % 8);

      /* build global communicator |-> location mapping */
      gcid2lid[comm->id].push_back(rank);

      /* store global id |-> "expanded global" id */
      gcid2egcid[cid].push_back(comm->id);

      /* enqueue communicator */
      comm_q.push_back(comm);
    }
  } else {
    /* Create "normal" communicator entry */
    Comm* comm = new Comm;
    comm->id   = comm_q.size();
    comm->mode = mode & ELG_GROUP_FLAG;
    comm->grpc = (numRanks + 7) / 8;
    comm->grpv = (elg_ui1*)calloc(comm->grpc, sizeof(elg_ui1));
    comm->gid  = group->id;

    for (elg_ui4 rank = 0; rank < group->size; ++rank)
      comm->grpv[group->ranks[rank] / 8] |= 1 << (group->ranks[rank] % 8);

    /* build global communicator |-> location mapping */
    for (elg_ui4 i = 0; i < comm->grpc*8; ++i)
      if (((1 << (i % 8)) & comm->grpv[i/8]))
        gcid2lid[comm->id].push_back(i);

    /* store global id |-> "expanded global" id */
    gcid2egcid[cid].push_back(comm->id);

    /* enqueue communicator */
    comm_q.push_back(comm);
  }
}


/*
 *---------------------------------------------------------------------------------
 * struct Infile
 *---------------------------------------------------------------------------------
 */

Infile::Infile(const char* file_name, unsigned mpi_rank)
  : glid(mpi_rank),
    offcnt(0),
    locations(NULL),
    regions(NULL),
    callsites(NULL),
    groups(NULL),
    comms(NULL),
    windows(NULL)
{
  char* elg_name = epk_archive_rankname(EPK_TYPE_ELG, file_name, mpi_rank);

  /* open trace file */
  inf = ElgIn_open(elg_name);
  if (!inf)
    elg_error_msg("Cannot open %s file \"%s\"",
                  epk_archive_filetype(elg_name), elg_name);

  /* default values for post-mortem clock synchronization */
  offset[0] = 0.0;
  offset[1] = 0.0;
  ltime[0]  = 0.0;
  ltime[1]  = 1.0;

  /* free resources */
  free(elg_name);
}


Infile::~Infile()
{
  /* close trace file */
  ElgIn_close(inf);

  /* release resources */
  if (locations)
    epk_idmap_free(locations);
  if (regions)
    epk_idmap_free(regions);
  if (callsites)
    epk_idmap_free(callsites);
  if (groups)
    epk_idmap_free(groups);
  if (comms)
    epk_idmap_free(comms);
  if (windows)
    epk_idmap_free(windows);
}


inline void Infile::rewind()
{
  ElgIn_rewind(inf);
}


ElgRec* 
Infile::next_rec()
{
  static int locationless = check_locationless(inf);
  ElgRec* rec;

  while( (rec = ElgIn_localize_record(inf, locationless ? glid : ELG_NO_ID)) ) {
      elg_ui1 rec_type = ElgRec_get_type(rec);

      /* Discard attribute and non-blocking extension records */
      if (ElgRec_is_attribute(rec)             ||
          (rec_type == ELG_MPI_SEND_COMPLETE)  ||
          (rec_type == ELG_MPI_RECV_REQUEST)   ||
          (rec_type == ELG_MPI_REQUEST_TESTED) ||
          (rec_type == ELG_MPI_CANCELLED))
      {
        ElgRec_free(rec);
        continue;
      }

      if (ElgRec_is_event(rec))
	{
	  int pos = 0;
	  
          if (locationless) {
            /* already globalized location */
            ElgRec_seek(rec, sizeof(elg_ui4));
          } else {
            ElgRec_seek(rec, 0);
            /* correct location id */
            elg_ui4 llid, clid;
            llid = ElgRec_read_ui4(rec);
            clid = llid2glid(llid);
            ElgRec_write_ui4(rec, clid, 0);
          }
          pos += sizeof(elg_ui4);
	  
	  /* correct time stamp */
	  elg_d8  oldtime, newtime; 

	  oldtime = ElgRec_read_d8(rec);
	  newtime = corrected_time(oldtime); 	  
	  ElgRec_write_d8(rec, newtime, pos);
	  pos += sizeof(newtime);
	  
	  switch(rec_type)
	    {
	    case ELG_ENTER:  
	      {
		elg_ui4 lrid, grid;

		/* correct region id */
		lrid = ElgRec_read_ui4(rec);
		grid = lrid2grid(lrid);
		ElgRec_write_ui4(rec, grid, pos);
		break;
	      }
	    case ELG_ENTER_CS:  
	      {
		elg_ui4 lcsid, gcsid;

		/* correct call-site id */
		lcsid = ElgRec_read_ui4(rec);
		gcsid = lcsid2gcsid(lcsid);
		ElgRec_write_ui4(rec, gcsid, pos);
		break;
	      }
	    case ELG_MPI_SEND:
	    case ELG_MPI_RECV:
	      {
                elg_ui4 spid = ElgRec_read_ui4(rec);
                elg_ui4 lcid = ElgRec_read_ui4(rec);

                /* Correct communicator ID */
                elg_ui4 gcid = lcid2gcid(lcid);

                /* Correct src/dest location id */
                if (elg_version >= 1008 && spid != ELG_NO_ID ) {
                  Comm* comm = comm_q[gcid];
                  if (comm->gid == ELG_NO_ID)
                    spid = glid;
                  else
                    spid = group_q[comm->gid]->ranks[spid];
                }
                elg_ui4 gslid = ( spid == ELG_NO_ID ) ? ELG_NO_ID : pid2glid[spid];

		/* Update trace record */
		ElgRec_write_ui4(rec, gslid, pos);
		pos += sizeof(gslid);
		ElgRec_write_ui4(rec, gcid, pos);

		break;
	      }
	    case ELG_COLLEXIT:
	    case ELG_MPI_COLLEXIT:
	      {
		/* Jump over performance counter */
		pos += metc * sizeof(elg_ui8);
		ElgRec_seek(rec, pos);

                elg_ui4 rpid = ElgRec_read_ui4(rec);
                elg_ui4 lcid = ElgRec_read_ui4(rec);

                /* Correct communicator ID */
                elg_ui4 gcid = lcid2gcid(lcid);

                /* Correct root location id */
                if (elg_version >= 1008 && rpid != ELG_NO_ID) {
                  Comm* comm = comm_q[gcid];
                  if (comm->gid == ELG_NO_ID)
                    rpid = glid;
                  else
                    rpid = group_q[comm->gid]->ranks[rpid];
                }
                elg_ui4 grlid = ( rpid == ELG_NO_ID ) ? ELG_NO_ID : pid2glid[rpid];

		/* Update trace record */
		ElgRec_write_ui4(rec, grlid, pos);
		pos += sizeof(grlid);
		ElgRec_write_ui4(rec, gcid, pos);

		break;
	      }
	    case ELG_MPI_PUT_1TS :
	    case ELG_MPI_PUT_1TE :
	    case ELG_MPI_PUT_1TE_REMOTE :
	    case ELG_MPI_GET_1TS :
	    case ELG_MPI_GET_1TE :
	    case ELG_MPI_GET_1TS_REMOTE :
	      {
                elg_ui4 ltid = ElgRec_read_ui4(rec);
                elg_ui4 lwid = ElgRec_read_ui4(rec);

                /* Correct window id */
                elg_ui4 gwid = lwid2gwid(lwid);

                /* Correct origin/target location id */
                if (elg_version >= 1008) {
                  Comm* comm = comm_q[win_q[gwid]->cid];
                  if (comm->gid == ELG_NO_ID)
                    ltid = glid;
                  else
                    ltid = group_q[comm->gid]->ranks[ltid];
                }
                elg_ui4 gtid = pid2glid[ltid];

		/* Update trace record */
		ElgRec_write_ui4(rec, gtid, pos);
		pos += sizeof(gtid);
		ElgRec_write_ui4(rec, gwid, pos);

		break;
	      }
	    case ELG_MPI_WIN_LOCK : 
	    case ELG_MPI_WIN_UNLOCK : 
	      {
                elg_ui4 llid = ElgRec_read_ui4(rec);
                elg_ui4 lwid = ElgRec_read_ui4(rec);

                /* Correct window id */
                elg_ui4 gwid = lwid2gwid(lwid);

                /* Correct lock location id */
                if (elg_version >= 1008) {
                  Comm* comm = comm_q[win_q[gwid]->cid];
                  if (comm->gid == ELG_NO_ID)
                    llid = glid;
                  else
                    llid = group_q[comm->gid]->ranks[llid];
                }
                elg_ui4 gllid = pid2glid[llid];

		/* Update trace record */
		ElgRec_write_ui4(rec, gllid, pos);
		pos += sizeof(glid);
		ElgRec_write_ui4(rec, gwid, pos);

		break;
	      }
	    case ELG_MPI_WINEXIT : 
	      {
		elg_ui4 lwid,lcid,gwid,gcid;

		/* Jump over performance counter */
		pos += metc * sizeof(elg_ui8);
		ElgRec_seek(rec, pos);

		/* Correct window id */
		lwid = ElgRec_read_ui4( rec );
		gwid = lwid2gwid(lwid);
		ElgRec_write_ui4( rec, gwid, pos );
		pos += sizeof(gwid);

		/* Correct Communicator ID */
		lcid = ElgRec_read_ui4( rec );
		gcid = lgid2gcid(lcid);
		ElgRec_write_ui4( rec, gcid, pos );
		break;
	      }
	    case ELG_MPI_WINCOLLEXIT :
	      {
		elg_ui4 lwid,gwid;

		/* Jump over performance counter */
		pos += metc * sizeof(elg_ui8);
		ElgRec_seek(rec, pos);

		/* Correct window id */
		lwid = ElgRec_read_ui4( rec );
		gwid = lwid2gwid(lwid);
		ElgRec_write_ui4( rec, gwid, pos );
		
		break;
	      }
	    case ELG_PUT_1TS :
	    case ELG_PUT_1TE :
	    case ELG_PUT_1TE_REMOTE :
	    case ELG_GET_1TS :
	    case ELG_GET_1TE :
	    case ELG_GET_1TS_REMOTE :
	      {
		elg_ui4 ltid, gtid;
		
		/* Correct origin/target location id  */
		ltid = ElgRec_read_ui4(rec);
		gtid = pid2glid[ltid];
		ElgRec_write_ui4(rec, gtid, pos );
		break;
	      }

	    }
	  return rec;
	}
      ElgRec_free(rec);
    }
  return NULL;
}


inline double Infile::corrected_time(double time) const
{
  return time + (((offset[1] - offset[0]) / (ltime[1] - ltime[0])) * 
		 (time - ltime[0])) + offset[0];
}


inline elg_ui4 Infile::llid2glid(elg_ui4 lid) const
{
  if (locations)
    return epk_idmap_get(locations, lid);
  return lid;
}


inline elg_ui4 Infile::lrid2grid(elg_ui4 rid) const
{
  if (regions)
    return epk_idmap_get(regions, rid);
  return rid;
}


inline elg_ui4 Infile::lcsid2gcsid(elg_ui4 csid) const
{
  if (callsites)
    return epk_idmap_get(callsites, csid);
  return csid;
}


inline elg_ui4 Infile::lgid2gcid(elg_ui4 gid) const
{
  elg_ui4 gcid = gid;

  /* group records have changed with EPILOG v1.8 */
  if (elg_version >= 1008) {
    elg_ui4 ggid = gid;
    if (groups)
      ggid = epk_idmap_get(groups, gid);

    Group* group = group_q[ggid];
    if (group->cid == ELG_NO_ID) {
      group->cid = comc++;
      create_comm(group, group->cid, ELG_GROUP_FLAG);
    }
    gcid = group->cid;
  } else {
    if (comms)
      gcid = epk_idmap_get(comms, gid);
  }

  if (gcid2egcid[gcid].size() == 1)
    return gcid2egcid[gcid][0];

  return gcid2egcid[gcid][rank];
}


inline elg_ui4 Infile::lcid2gcid(elg_ui4 cid) const
{
  elg_ui4 gcid = cid;
  if (comms)
    gcid = epk_idmap_get(comms, cid);

  if (gcid2egcid[gcid].size() == 1)
    return gcid2egcid[gcid][0];

  return gcid2egcid[gcid][rank];
}


inline elg_ui4 Infile::lwid2gwid(elg_ui4 wid) const
{
  elg_ui4 gwid = wid;
  if (windows)
    gwid = epk_idmap_get(windows, wid);

  if (gwid2egwid[gwid].size() == 1)
    return gwid2egwid[gwid][0];

  return gwid2egwid[gwid][rank];
}
