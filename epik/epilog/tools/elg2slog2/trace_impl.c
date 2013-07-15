/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "elg_error.h"
#include "elg_readcb.h"
#include "elg_conv.h"
#include "elg_conv_msgqueue.h"

#include "trace_API.h"
#include "slog2_mpitab.h"

/* defines whether debug information should be printed or not */
#undef DEBUG

#define MAX_LEGEND_LENGTH 128
#define MAX_LABEL_LENGTH 512

typedef struct {
    TRACE_Category_head_t *hdr;
    char                  *legend;
    char                  *label;
    int                    num_methods;
    int                   *methods;
} DRAW_Category;

#define  MAX_COLNAMES    10
#define  MAX_NAME_LEN    128

typedef struct {
    int               num_rows;
    int               num_columns;
    char              title_name[ MAX_NAME_LEN ];
    char              column_names[ MAX_COLNAMES ][ MAX_NAME_LEN ];
    int              *elems;
    int               num_methods;
    int              *methods;
} DRAW_YCoordMap;

typedef struct {
    double            starttime;
    double            endtime;
    int               type_idx;
    int               num_info;
    char             *info;
    int               num_tcoords;
    double           *tcoords;
    int               num_ycoords;
    int              *ycoords;
} DRAW_Primitive;

typedef struct {
    double            star;
    double            endt;
    int               type;
    int               num_info;
    char             *info;
    int               num_primes;
    DRAW_Primitive  **primes; 
    int               idx2prime;
} DRAW_Composite;

typedef struct _trace_file {
    ElgRCB           *fd;
} DRAW_File;

typedef struct _primitives_list{
  int size;
  DRAW_Primitive *prim;
  struct _primitives_list *next;
} PRIMITIVES_List;

PRIMITIVES_List *primitives, *last_primitive;

#define NUM_CATEGORIES 15
int categories_checked = 0;

DRAW_Category categories[NUM_CATEGORIES];

void init_categories() {
  int i;
  for(i=0; i< NUM_CATEGORIES; i++) {
    categories[i].hdr = (TRACE_Category_head_t*) malloc(sizeof(TRACE_Category_head_t));
    categories[i].legend = (char*) malloc(MAX_LEGEND_LENGTH*sizeof(char));
    categories[i].label = (char*) malloc(MAX_LABEL_LENGTH*sizeof(char));
  }
  categories[0].hdr->index = ST_MESSAGE;
  categories[0].hdr->shape =  TRACE_SHAPE_ARROW;
  categories[0].hdr->red = 255;
  categories[0].hdr->green = 255;
  categories[0].hdr->blue = 255;
  categories[0].hdr->alpha = 255; 
  categories[0].hdr->width = 1;
  categories[0].legend = "Message";
  categories[0].label = "Sender rank: %d\nDestination rank: %d\nTag: %d\nMessage size: %d";
  categories[0].num_methods = 0;
  categories[0].methods = 0;
  categories[1].hdr->index = ST_USER;
  categories[1].hdr->shape = TRACE_SHAPE_STATE;
  categories[1].hdr->red = 10;
  categories[1].hdr->green = 219;
  categories[1].hdr->blue = 45;
  categories[1].hdr->alpha = 255;
  categories[1].hdr->width = 1;
  categories[1].legend = "User";
  categories[1].label = "%s";
  categories[1].num_methods = 0;
  categories[1].methods = 0;
  categories[2].hdr->index = ST_EPIK;
  categories[2].hdr->shape = TRACE_SHAPE_STATE;
  categories[2].hdr->red = 249;
  categories[2].hdr->green = 66;
  categories[2].hdr->blue = 255;
  categories[2].hdr->alpha = 255;
  categories[2].hdr->width = 1;
  categories[2].legend = "Epik";
  categories[2].label = "%s";
  categories[2].num_methods = 0;
  categories[2].methods = 0;
  categories[3].hdr->index = ST_MPIRECV;
  categories[3].hdr->shape = TRACE_SHAPE_STATE;
  categories[3].hdr->red = 255;
  categories[3].hdr->green = 187;
  categories[3].hdr->blue = 0;
  categories[3].hdr->alpha = 255;
  categories[3].hdr->width = 1;
  categories[3].legend = "MPI_Recv";
  categories[3].label = "%s";
  categories[3].num_methods = 0;
  categories[3].methods = 0;
  categories[4].hdr->index = ST_MPISEND;
  categories[4].hdr->shape = TRACE_SHAPE_STATE;
  categories[4].hdr->red = 0;
  categories[4].hdr->green = 0;
  categories[4].hdr->blue = 255;
  categories[4].hdr->alpha = 255;
  categories[4].hdr->width = 1;
  categories[4].legend = "MPI_Send";
  categories[4].label = "%s";
  categories[4].num_methods = 0;
  categories[4].methods = 0;
  categories[5].hdr->index = ST_MPITEST;
  categories[5].hdr->shape = TRACE_SHAPE_STATE;
  categories[5].hdr->red = 0;
  categories[5].hdr->green = 0;
  categories[5].hdr->blue = 0;
  categories[5].hdr->alpha = 255;
  categories[5].hdr->width = 1;
  categories[5].legend = "MPI_Test";
  categories[5].label = "%s";
  categories[5].num_methods = 0;
  categories[5].methods = 0;
  categories[6].hdr->index = ST_MPIWAIT;
  categories[6].hdr->shape = TRACE_SHAPE_STATE;
  categories[6].hdr->red = 162;
  categories[6].hdr->green = 0;
  categories[6].hdr->blue =  0;
  categories[6].hdr->alpha = 255;
  categories[6].hdr->width = 1;
  categories[6].legend = "MPI_Wait";
  categories[6].label = "%s";
  categories[6].num_methods = 0;
  categories[6].methods = 0;
  categories[7].hdr->index = ST_MPIISEND;
  categories[7].hdr->shape = TRACE_SHAPE_STATE;
  categories[7].hdr->red = 0;
  categories[7].hdr->green = 0;
  categories[7].hdr->blue = 255;
  categories[7].hdr->alpha = 255;
  categories[7].hdr->width = 1;
  categories[7].legend = "MPI_ISend";
  categories[7].label = "%s";
  categories[7].num_methods = 0;
  categories[7].methods = 0;
  categories[8].hdr->index = ST_MPIIRECV;
  categories[8].hdr->shape = TRACE_SHAPE_STATE;
  categories[8].hdr->red = 255;
  categories[8].hdr->green = 187;
  categories[8].hdr->blue = 0;
  categories[8].hdr->alpha = 255;
  categories[8].hdr->width = 1;
  categories[8].legend = "MPI_IRecv";
  categories[8].label = "%s";
  categories[8].num_methods = 0;
  categories[8].methods = 0;
  categories[9].hdr->index = ST_MPICOLL;
  categories[9].hdr->shape = TRACE_SHAPE_STATE;
  categories[9].hdr->red = 255;
  categories[9].hdr->green = 94;
  categories[9].hdr->blue = 50;
  categories[9].hdr->alpha = 255;
  categories[9].hdr->width = 1;
  categories[9].legend = "MPI_Coll";
  categories[9].label = "%s";
  categories[9].num_methods = 0;
  categories[9].methods = 0;
  categories[10].hdr->index = ST_MPIOTHER;
  categories[10].hdr->shape = TRACE_SHAPE_STATE;
  categories[10].hdr->red = 209;
  categories[10].hdr->green = 212;
  categories[10].hdr->blue = 219;
  categories[10].hdr->alpha = 255;
  categories[10].hdr->width = 1;
  categories[10].legend = "MPI_Other";
  categories[10].label = "%s";
  categories[10].num_methods = 0;
  categories[10].methods = 0;
  categories[11].hdr->index = ST_MPISDRV;
  categories[11].hdr->shape = TRACE_SHAPE_STATE;
  categories[11].hdr->red = 0;
  categories[11].hdr->green = 0;
  categories[11].hdr->blue = 0;
  categories[11].hdr->alpha = 255;
  categories[11].hdr->width = 1;
  categories[11].legend = "MPI_SdRv";
  categories[11].label = "%s";
  categories[11].num_methods = 0;
  categories[11].methods = 0;
  categories[12].hdr->index = ST_MPIRMA;
  categories[12].hdr->shape = TRACE_SHAPE_STATE;
  categories[12].hdr->red = 0;
  categories[12].hdr->green = 0;
  categories[12].hdr->blue = 0;
  categories[12].hdr->alpha = 255;
  categories[12].hdr->width = 1;
  categories[12].legend = "MPI_RMA";
  categories[12].label = "%s";
  categories[12].num_methods = 0;
  categories[12].methods = 0;
  categories[13].hdr->index = ST_OMP;
  categories[13].hdr->shape = TRACE_SHAPE_STATE;
  categories[13].hdr->red = 248;
  categories[13].hdr->green = 255;
  categories[13].hdr->blue = 44;
  categories[13].hdr->alpha = 255;
  categories[13].hdr->width = 1;
  categories[13].legend = "OMP";
  categories[13].label = "%s";
  categories[13].num_methods = 0;
  categories[13].methods = 0;
  categories[14].hdr->index = ST_OMPCOLL;
  categories[14].hdr->shape = TRACE_SHAPE_STATE;
  categories[14].hdr->red = 255;
  categories[14].hdr->green = 209;
  categories[14].hdr->blue = 44;
  categories[14].hdr->alpha = 255;
  categories[14].hdr->width = 1;
  categories[14].legend = "OMP_Coll";
  categories[14].label = "%s";
  categories[14].num_methods = 0;
  categories[14].methods = 0;
}


static void elg_swap(void* buffer, size_t length) {
  /* length must be an even number */

  char   tmp;
  size_t i;

  if (length > 1)
    for (i = 0; i < length / 2; i++)
      {
	tmp = ((char*) buffer)[i];
	((char*) buffer)[i] = ((char*) buffer)[length - 1 - i];
	((char*) buffer)[length - 1 - i] = tmp;
      }  
}

/* -- Attributes -- */

void elg_readcb_ATTR_UI1(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui1 val,       /* value                       */
                      void* userdata)
{ 
}

void elg_readcb_ATTR_UI4(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui4 val,       /* value                       */
                      void* userdata)
{ 
}

/* -- Region -- */

void elg_readcb_ENTER(elg_ui4 lid, elg_d8 time, elg_ui4 rid,
                      elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  stk_push(lid, rid, time);
#ifdef DEBUG
  printf("Enter on rank %d, Region: %d, ID: %d, Group: %d, Groupname: %s, Name: %s\n",lid, rid, statetab[rid].id, statetab[rid].act, stringtab[statetab[rid].act], statetab[rid].name);
#endif
}

void elg_readcb_ENTER_CS(elg_ui4 lid, elg_d8 time, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  stk_push(lid, csitetab[csid].erid, time);
}

void elg_readcb_EXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;
  int i;
  struct {
    int16_t length;
    char *string;
  } val;

  lid = locmap[lid];
  n = stk_pop(lid);

#ifdef DEBUG
  printf("Exit on %d, %d, %s\n",lid, n->reg, statetab[n->reg].name);
#endif

  if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }

  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = n->tim;
  last_primitive->prim->endtime = time;
  if (!strcmp(stringtab[statetab[n->reg].act],"EPIK")) { 
    last_primitive->prim->type_idx = ST_EPIK;
  } else if (!strcmp(stringtab[statetab[n->reg].act],"USR")) {
    last_primitive->prim->type_idx = ST_USER;
  } else if (!strcmp(stringtab[statetab[n->reg].act],"OMP")) {
    if (strstr(statetab[n->reg].name,"sblock") != NULL ||
      (strstr(statetab[n->reg].name,"section") != NULL &&
      strstr(statetab[n->reg].name,"sections") == NULL)) {
      last_primitive->prim->type_idx = ST_USER;
    } else {
      last_primitive->prim->type_idx = ST_OMP;
    }
  } else if (!strcmp(stringtab[statetab[n->reg].act],"MPI")) {
    for(i=0; i < MPI_FUNCTIONS; i++) {
      if(!strcmp(statetab[n->reg].name,slog2_mpi_tab[i].name)) {
        last_primitive->prim->type_idx = slog2_mpi_tab[i].state;
        break;
      }
    }
  } else {
    fprintf(stderr, "!!!Error1 - Function: %s in Group %s not implemented!!!!\n", statetab[n->reg].name, stringtab[statetab[n->reg].act]);
    exit(1);
  }
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = n->tim;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = lid;
  last_primitive->prim->ycoords[1] = lid;
  last_primitive->prim->num_info = sizeof(int16_t)+strlen(statetab[n->reg].name)*sizeof(char);
  last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));

  val.string = (char *) malloc(strlen(statetab[n->reg].name)*sizeof(char));
  strcpy(val.string, statetab[n->reg].name);
  val.length = strlen(statetab[n->reg].name);

  if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
    elg_swap(&val.length, sizeof(int16_t));
  }

  memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
  memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, strlen(statetab[n->reg].name)*sizeof(char));

}

/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 cid,
       elg_ui4 tag, elg_ui4 sent, void* userdata) {
#ifdef DEBUG
 printf("New Message from %d to %d with tag %d at time %f \n",lid, dlid, tag, time);
#endif
  AddMessage(lid, dlid, tag, cid, sent, 0.0, time);
}

void elg_readcb_MPI_RECV(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 cid,
       elg_ui4 tag, void* userdata) {

  double tcoords[2];
  int ycoords[2];
  int32_t vals[4];
    
  Message *tmp = FindMessage(slid, lid, tag, cid);

#ifdef DEBUG
 printf("Message recieved at %d from %d with tag %d at time %f \n",lid, slid, tag, time);
#endif
    
  if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }

  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = tmp->stime;
  last_primitive->prim->endtime = time;
  last_primitive->prim->type_idx = ST_MESSAGE;
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = tmp->stime;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = slid;
  last_primitive->prim->ycoords[1] = lid;
  last_primitive->prim->num_info = 4*sizeof(int32_t);
  last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));

  vals[0] = slid;
  vals[1] = lid;
  vals[2] = tag;
  vals[3] = tmp->len;
  if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
    elg_swap(&vals[0], sizeof(int32_t));
    elg_swap(&vals[1], sizeof(int32_t));
    elg_swap(&vals[2], sizeof(int32_t));
    elg_swap(&vals[3], sizeof(int32_t));
  }
  memcpy(last_primitive->prim->info, vals, last_primitive->prim->num_info);

}

void elg_readcb_MPI_COLLEXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;
  int i;
  struct {
    int16_t length;
    char *string;
  } val;


  lid = locmap[lid];
  if ( rlid != ELG_NO_ID ) rlid = locmap[rlid];
  n = stk_pop(lid);

  if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }

  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = n->tim;
  last_primitive->prim->endtime = time;
  //last_primitive->prim->type_idx = n->reg;
  if(!strcmp(stringtab[statetab[n->reg].act],"MPI")) {
    for(i=0; i < MPI_FUNCTIONS; i++) {
      if(!strcmp(statetab[n->reg].name,slog2_mpi_tab[i].name)) {
        last_primitive->prim->type_idx = slog2_mpi_tab[i].state;//n->reg;
        break;
      }
    }
  } else {
    fprintf(stderr, "!!!Error - Function: %s not implemented!!!!", statetab[n->reg].name);
    exit(1);
  }
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = n->tim;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = lid;
  last_primitive->prim->ycoords[1] = lid;
  last_primitive->prim->num_info = sizeof(int16_t)+strlen(statetab[n->reg].name)*sizeof(char);

  last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
  val.string = (char *) malloc(strlen(statetab[n->reg].name)*sizeof(char));
  strcpy(val.string, statetab[n->reg].name);
  val.length = strlen(statetab[n->reg].name);
  
  if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
    elg_swap(&val.length, sizeof(int16_t));
  }
  
  memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
  memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, strlen(statetab[n->reg].name)*sizeof(char));
  
}

/* -- MPI-1 Non-blocking -- */

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

/* -- MPI-2 One-sided Operations -- */

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
}

void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
}

void elg_readcb_MPI_GET_1TO(elg_ui4 lid, elg_d8 time, elg_ui4 rmaid,
       void* userdata) {
}

void elg_readcb_MPI_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
}

void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
       void* userdata) {
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, void* userdata) {
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, elg_ui1 ltype, void* userdata) {
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, void* userdata) {
}

/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
}

/* -- OpenMP -- */

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void* userdata) {
#ifdef DEBUG
  printf("OMP fork on %d!\n",lid);
#endif
  lid = locmap[lid];
  stk_push(lid, lid, time);
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;
  int i;
  struct {
    int16_t length;
    char *string;
  } val;

  lid = locmap[lid];
  n = stk_pop(lid);

#ifdef DEBUG
  printf("OMP Join on %d, %d, %s\n",lid, n->reg, statetab[n->reg].name);
#endif

  if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }

  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = n->tim;
  last_primitive->prim->endtime = time;
  last_primitive->prim->type_idx = ST_OMP;
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = n->tim;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = lid;
  last_primitive->prim->ycoords[1] = lid;
  last_primitive->prim->num_info = sizeof(int16_t)+6*sizeof(char);

  last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
  val.string = (char *) malloc(6*sizeof(char));
  strcpy(val.string, "OpenMP");
  val.length = strlen("OpenMP");
  
  if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
    elg_swap(&val.length, sizeof(int16_t));
  }
  
  memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
  memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, 6*sizeof(char));
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {

  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;
  int i;
  struct {
    int16_t length;
    char *string;
  } val;

  lid = locmap[lid];
  n = stk_pop(lid);

#ifdef DEBUG
  printf("OMP CollExit on %d, %d, %s\n",lid, n->reg, statetab[n->reg].name);
#endif

  if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }

  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = n->tim;
  last_primitive->prim->endtime = time;
  if( !strncmp(statetab[n->reg].name,"!$omp barrier",13) ||
      ! strncmp(statetab[n->reg].name,"!$omp ibarrier",14) ) {
     last_primitive->prim->type_idx = ST_OMPCOLL;
  } else if (!strncmp(statetab[n->reg].name,"!$omp parallel",14)) {
     last_primitive->prim->type_idx = ST_USER;
  } else {
    last_primitive->prim->type_idx = ST_OMP;
  }
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = n->tim;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = lid;
  last_primitive->prim->ycoords[1] = lid;
  last_primitive->prim->num_info = sizeof(int16_t)+strlen(statetab[n->reg].name)*sizeof(char);

  last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
  val.string = (char *) malloc(strlen(statetab[n->reg].name)*sizeof(char));
  strcpy(val.string, statetab[n->reg].name);
  val.length = strlen(statetab[n->reg].name);
  
  if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
    elg_swap(&val.length, sizeof(int16_t));
  }
  
  memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
  memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, strlen(statetab[n->reg].name)*sizeof(char));

//   if( !strncmp(statetab[n->reg].name,"!$omp barrier",13) ||
//      ! strncmp(statetab[n->reg].name,"!$omp ibarrier",14) ) {
//     printf("OMP Collective!\n");
//   }
}

/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
}

void elg_readcb_LOG_ON(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  
    lid = locmap[lid];
    stk_push(lid, 0, time);
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {

  MYDATA* md = (MYDATA*) userdata;
  StackNode *n, *p;
  int i;
  struct {
    int16_t length;
    char *string;
  } val;


  lid = locmap[lid];
  //if ( rlid != ELG_NO_ID ) rlid = locmap[rlid];
  n = stk_pop(lid);

    if(primitives == NULL) {
    primitives = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = primitives;
    primitives->size = 0;
  } else {
    last_primitive->next = (PRIMITIVES_List*) malloc(sizeof(struct _primitives_list));
    last_primitive = last_primitive->next;
  }
  /* New Event */
  primitives->size++;

  last_primitive->next = NULL;
  last_primitive->prim = (DRAW_Primitive *) malloc(sizeof(DRAW_Primitive));
  last_primitive->prim->starttime = n->tim;
  last_primitive->prim->endtime = time;
  last_primitive->prim->type_idx = ST_EPIK;
  last_primitive->prim->num_tcoords = 2;
  last_primitive->prim->tcoords = (double*) malloc(2*sizeof(double));
  last_primitive->prim->tcoords[0] = n->tim;
  last_primitive->prim->tcoords[1] = time;
  last_primitive->prim->num_ycoords = 2;
  last_primitive->prim->ycoords = (int*) malloc(2*sizeof(int));
  last_primitive->prim->ycoords[0] = lid;
  last_primitive->prim->ycoords[1] = lid;
  p = stk_top(lid);
  if(!strcmp(statetab[p->reg].name,"MPI_Init")) {
    last_primitive->prim->num_info = sizeof(int16_t)+21*sizeof(char);
    last_primitive->prim->info = (char*) malloc(21*sizeof(char));
    strcpy(last_primitive->prim->info, "Epik - Initialization");
  
    last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
    val.string = (char *) malloc(21*sizeof(char));
    strcpy(val.string, "Epik - Initialization");
    val.length = 21;
    
    if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
      elg_swap(&val.length, sizeof(int16_t));
    }
    
    memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
    memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, 21*sizeof(char));
  } else if(!strcmp(statetab[p->reg].name,"MPI_Finalize")){
    last_primitive->prim->num_info = sizeof(int16_t)+19*sizeof(char);
    last_primitive->prim->info = (char*) malloc(19*sizeof(char));
    strcpy(last_primitive->prim->info, "Epik - Finalization");
  
    last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
    val.string = (char *) malloc(19*sizeof(char));
    strcpy(val.string, "Epik - Finalization");
    val.length = 19;
    
    if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
      elg_swap(&val.length, sizeof(int16_t));
    }
    
    memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
    memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, 19*sizeof(char));
  } else {
    last_primitive->prim->num_info = sizeof(int16_t)+12*sizeof(char);
    last_primitive->prim->info = (char*) malloc(12*sizeof(char));
    strcpy(last_primitive->prim->info, "Epik - Flush");
  
    last_primitive->prim->info = (char*) malloc(last_primitive->prim->num_info*sizeof(char));
    val.string = (char *) malloc(12*sizeof(char));
    strcpy(val.string, "Epik - Flush");
    val.length = 12;
    
    if(ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN) {
      elg_swap(&val.length, sizeof(int16_t));
    }
    
    memcpy(last_primitive->prim->info, &val.length, sizeof(int16_t));
    memcpy(last_primitive->prim->info+sizeof(int16_t), val.string, 12*sizeof(char));
  }
}

TRACE_EXPORT
int TRACE_Open(const char filespec[], TRACE_file *fp) {
  TRACE_file tr;
  MYDATA md = {0, 0, 0, 0};

  init_categories();
  tr = (TRACE_file) malloc(sizeof(struct _trace_file));
  /* Open EPILOG trace stored in `file' */
  tr->fd = elg_read_open(filespec);
  if (tr->fd == NULL) return 1;

  /* Process every event record of the trace */
  /* Calls read callback functions above     */
  while ( elg_read_next(tr->fd, (void*) &md) ) {
  }
  *fp = tr;
  return 0;
}

TRACE_EXPORT
int TRACE_Close(TRACE_file *fp) {
  /* Close trace file and cleanup */
  elg_read_close((*fp)->fd);

  return 0;
}

TRACE_EXPORT
int TRACE_Peek_next_kind(const TRACE_file fp, TRACE_Rec_Kind_t *next_kind) {
  if(categories_checked < NUM_CATEGORIES) {
    *next_kind = TRACE_CATEGORY;
  } else if(primitives != NULL) {
    *next_kind = TRACE_PRIMITIVE_DRAWABLE;
  } else {
    *next_kind = TRACE_EOF;
  }
  return 0;
}

TRACE_EXPORT
int TRACE_Get_next_method(const TRACE_file fp,
                          char method_name[], char method_extra[],
                          int *methodID) {
  return 0;
}

TRACE_EXPORT
int TRACE_Peek_next_category(const TRACE_file fp,
                             int *n_legend, int *n_label,
                             int *n_methodIDs) {
#ifdef DEBUG
  printf("Category %d: Legend: %s (%d), Label: %s, Num: %d\n", categories_checked,
         categories[categories_checked].legend,
         strlen(categories[categories_checked].legend),
         categories[categories_checked].label,
         categories[categories_checked].num_methods);
#endif
  *n_legend = strlen(categories[categories_checked].legend);
  *n_label = strlen(categories[categories_checked].label);
  *n_methodIDs = categories[categories_checked].num_methods;
  return 0;
}

TRACE_EXPORT
int TRACE_Get_next_category(const TRACE_file fp,
                            TRACE_Category_head_t *head,
                            int *n_legend, char legend_base[],
                            int *legend_pos, const int legend_max,
                            int *n_label, char label_base[],
                            int *label_pos, const int label_max,
                            int *n_methodIDs, int methodID_base[],
                            int *methodID_pos, const int methodID_max) {
  int legend_len, label_len;

  head->index = categories[categories_checked].hdr->index;
  head->shape = categories[categories_checked].hdr->shape;
  head->red = categories[categories_checked].hdr->red;
  head->green = categories[categories_checked].hdr->green;
  head->blue = categories[categories_checked].hdr->blue;
  head->alpha = categories[categories_checked].hdr->alpha;
  head->width = categories[categories_checked].hdr->width;
  //*head = *(categories[categories_checked].hdr);

  legend_len = strlen(categories[categories_checked].legend);
  strcpy(legend_base, categories[categories_checked].legend);
  *n_legend = legend_len;
  *legend_pos += *n_legend;
  label_len = strlen(categories[categories_checked].label);
  strcpy(label_base, categories[categories_checked].label);
  *n_label = label_len;
  *label_pos += *n_label;
  n_methodIDs = 0;
  methodID_base = (int*) malloc(sizeof(int));
  methodID_base[0] = 0;
  categories_checked++;

  return 0;
}

TRACE_EXPORT
int TRACE_Peek_next_ycoordmap( TRACE_file fp,
                               int *n_rows, int *n_columns,
                               int *max_column_name,
                               int *max_title_name,
                               int *n_methodIDs ) {

  return 0;
}

TRACE_EXPORT
int TRACE_Get_next_ycoordmap( TRACE_file fp,
                              char *title_name,
                              char **column_names,
                              int *coordmap_sz, int coordmap_base[],
                              int *coordmap_pos, const int coordmap_max,
                              int *n_methodIDs, int methodID_base[],
                              int *methodID_pos, const int methodID_max ) {

  return 0;
}

TRACE_EXPORT
int TRACE_Peek_next_primitive( const TRACE_file fp,
                               double *starttime, double *endtime,
                               int *n_tcoords, int *n_ycoords, int *n_bytes ) {

  *starttime = primitives->prim->starttime;
  *endtime = primitives->prim->endtime;
  *n_tcoords = primitives->prim->num_tcoords;
  *n_ycoords = primitives->prim->num_ycoords;
  *n_bytes = primitives->prim->num_info;

  return 0;
}

TRACE_EXPORT
int TRACE_Get_next_primitive( const TRACE_file fp, 
                              int *category_index, 
                              int *n_tcoords, double tcoord_base[],
                              int *tcoord_pos, const int tcoord_max, 
                              int *n_ycoords, int ycoord_base[], 
                              int *ycoord_pos, const int ycoord_max,
                              int *n_bytes, char byte_base[],
                              int *byte_pos, const int byte_max ) {
  int i;

  *category_index = primitives->prim->type_idx;
  for(i=0; i<*n_tcoords; i++)
    tcoord_base[*tcoord_pos+i] = primitives->prim->tcoords[i];
  *tcoord_pos += *n_tcoords;
  for(i=0; i<*n_ycoords; i++)
    ycoord_base[*ycoord_pos+i] = primitives->prim->ycoords[i];
  *ycoord_pos += *n_ycoords;
  
  if(primitives->prim->info != NULL) {
    memcpy(&(byte_base[*byte_pos]), primitives->prim->info, sizeof(char) * primitives->prim->num_info);
    *n_bytes = primitives->prim->num_info;
    *byte_pos += *n_bytes;
  } else {
    *byte_pos = 0;
  }
  primitives = primitives->next;
  return 0;
}

TRACE_EXPORT
int TRACE_Peek_next_composite( const TRACE_file fp,
                               double *starttime, double *endtime,
                               int *n_primitives, int *n_bytes ) {
  return 0;
}

TRACE_EXPORT
int TRACE_Get_next_composite( const TRACE_file fp,
                              int *category_index,
                              int *n_bytes, char byte_base[],
                              int *byte_pos, const int byte_max ) {
  return 0;
}

TRACE_EXPORT
int TRACE_Get_position( TRACE_file fp, TRACE_int64_t *offset ) {
  return 0;
}

TRACE_EXPORT
int TRACE_Set_position( TRACE_file fp, TRACE_int64_t offset ) {
  return 0;
}

TRACE_EXPORT
char *TRACE_Get_err_string( int ierr ) {
  switch(ierr) {
    case 0: return "Usage: elgTOslog2 epilog-file!";
            break;
    case 1: return "Error opening file!";
            break;
    case 2: return "No such next kind!";
            break;
    default: printf("Unknown error code: %d", ierr);
             return "Unknown or invalid error code!";
  }
}

void conv_setup() {
}

void conv_write_definition_records() {
}
