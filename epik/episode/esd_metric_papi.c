/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "epk_conf.h"
#include "elg_defs.h"
#include "elg_error.h"
#include "epk_metric.h"
#include "papi.h"

/* maximum number of supported papi-c components
 * set only if not specified as environment variable
 */
#ifndef ELG_PAPIC_MAX_COMP
#define ELG_PAPIC_MAX_COMP     (5)
#endif

struct metric
{
  char* name;
  char  descr[PAPI_HUGE_STR_LEN];
  int   papi_code;
};

/* KF: updated to support more than one component */
struct esd_metv
{
  /* we have one eventset per component */
  int EventSet[ELG_PAPIC_MAX_COMP];

  /* number of events added for each component */
  int NumEvents[ELG_PAPIC_MAX_COMP];

  /* maps the component-relative index to its
     gloabal metric index */
  int Comp2Metr[ELG_PAPIC_MAX_COMP][ELG_METRIC_MAXNUM];
};



/* global variables */
static struct metric* metricv[ELG_METRIC_MAXNUM];
static int    nmetrics = 0;

void esd_metrics_status()
{
    const char* env_metspec = epk_get(EPK_METRICS_SPEC);
    const char* env_metrics = epk_get(EPK_METRICS);
    if (env_metspec) elg_warning("EPK_METRICS_SPEC=\"%s\"", env_metspec);
    elg_warning("EPK_METRICS=\"%s\" defined %d metrics", env_metrics, nmetrics);
}

static void metricv_add(char* name, int code)
{
    if (nmetrics >= ELG_METRIC_MAXNUM)
        elg_error_msg("Number of counters exceeds ELG allowed maximum of %d\n",
                      ELG_METRIC_MAXNUM);
    else {
        metricv[nmetrics] = malloc(sizeof(struct metric));
        metricv[nmetrics]->name = strdup(name);
        metricv[nmetrics]->descr[0] = '\0';
        metricv[nmetrics]->papi_code = code;
        nmetrics++;
    }
}

/* PAPI-specific error message */

void esd_metric_error(int errcode, char *note)
{
  char *errstring;

  errstring = PAPI_strerror(errcode);
  if (errcode == PAPI_ESYS) {
      errstring = strcat(errstring,": ");
      errstring = strcat(errstring,strerror(errno));
  }
  elg_error_msg("%s: %s (fatal)", note?note:"PAPI", errstring);
}

/* PAPI-specific warning message */

void esd_metric_warning(int errcode, char *note)
{
  char *errstring;

  errstring = PAPI_strerror(errcode);
  if (errcode == PAPI_ESYS) {
      errstring = strcat(errstring,": ");
      errstring = strcat(errstring,strerror(errno));
  }
  elg_warning("%s: %s (ignored)", note?note:"PAPI", errstring);
}


/* get metric descriptions */

void esd_metric_descriptions()
{
  int i, j, k, retval;
  PAPI_event_info_t info;

  for (i=0; i < nmetrics; i++) {
    memset(&info, 0, sizeof(PAPI_event_info_t));
    retval = PAPI_get_event_info(metricv[i]->papi_code, &info);
    if (retval != PAPI_OK)
      esd_metric_error(retval, "PAPI_get_event_info");

    if (strcmp(info.long_descr, metricv[i]->name) != 0) {
      strncpy(metricv[i]->descr, info.long_descr, sizeof(metricv[i]->descr));
              
      /* tidy description if necessary */
      j=strlen(metricv[i]->descr)-1;
      if (metricv[i]->descr[j] == '\n') metricv[i]->descr[j]='\0';
      j=strlen(metricv[i]->descr)-1;
      if (metricv[i]->descr[j] != '.')
        strncat(metricv[i]->descr, ".", sizeof(metricv[i]->descr));
    }

    if (metricv[i]->papi_code & PAPI_PRESET_MASK) { /* PAPI preset */
      char *postfix_chp = info.postfix;
      char derive_ch = strcmp(info.derived,"DERIVED_SUB")?'+':'-';
      strncat(metricv[i]->descr, " [ ", sizeof(metricv[i]->descr));
      strncat(metricv[i]->descr, info.name[0], sizeof(metricv[i]->descr));
      for (k=1; k<info.count; k++) {
        char op[4];
        postfix_chp = postfix_chp?strpbrk(++postfix_chp, "+-*/"):NULL;
        sprintf(op, " %c ", (postfix_chp?*postfix_chp:derive_ch));
        strncat(metricv[i]->descr, op, sizeof(metricv[i]->descr));
        strncat(metricv[i]->descr, info.name[k], sizeof(metricv[i]->descr));
      }
      strncat(metricv[i]->descr, " ]", sizeof(metricv[i]->descr));
      if (strcmp(info.symbol, metricv[i]->name) != 0) { /* add preset name */
        strncat(metricv[i]->descr, " = ", sizeof(metricv[i]->descr));
        strncat(metricv[i]->descr, info.symbol, sizeof(metricv[i]->descr));
      }
    }

    /*printf("Metric %d: <%s>\n<<%s>>\n", i, metricv[i]->name, metricv[i]->descr);*/
  }
}

/* test whether requested event combination valid */

void esd_metric_test()
{
  int i, j, comp;
  int retval;
  int EventSet[ELG_PAPIC_MAX_COMP]; /* KF */
  
  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    EventSet[i] = PAPI_NULL;

    /* create event set */
    retval = PAPI_create_eventset(&(EventSet[i]));
    if ( retval != PAPI_OK)
      esd_metric_error(retval, "PAPI_create_eventset");
  }
      
  for ( j = 0; j < nmetrics; j++ ) {
    /* add event to event set */
    /*fprintf(stderr,"Checking Metric %s, Component indes: %d,\n\n",metricv[j]->name,PAPI_COMPONENT_INDEX( metricv[j]->papi_code ));*/
    comp = PAPI_COMPONENT_INDEX( metricv[j]->papi_code );
    retval = PAPI_add_event(EventSet[comp], metricv[j]->papi_code);
    
      
    if ( retval != PAPI_OK ) {
      char errstring[PAPI_MAX_STR_LEN];
      sprintf(errstring, "PAPI_add_event(%d:\"%s\")", j, metricv[j]->name);
      esd_metric_error(retval, errstring);
    }
    elg_cntl_msg("Event %s added to event set", metricv[j]->name);
  }
  
  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    retval = PAPI_cleanup_eventset(EventSet[i]);
    if ( retval != PAPI_OK )
      esd_metric_error(retval, "PAPI_cleanup_eventset");
      
    retval = PAPI_destroy_eventset(&(EventSet[i]));
    if ( retval != PAPI_OK )
      esd_metric_error(retval, "PAPI_destroy_eventset");
  }
  elg_cntl_msg("Event set tested OK");
}

/* define and set PAPI metrics based on EPK_METRICS specification */

int esd_metric_open()
{
  int retval;
  const char* env;
  char* var;
  char* token;
  PAPI_event_info_t info;
  epk_metricmap_t* mapv = NULL;

  /* read configuration variable "EPK_METRICS". Return if unset. */
  env = epk_get(EPK_METRICS);
  if ( env == NULL || strlen(env)==0)
    return nmetrics;

  mapv = epk_metricmap_init(EPK_METMAP_MEASURE|EPK_METMAP_AGGROUP);
  /*epk_metricmap_dump(mapv);*/

  /* initialize PAPI */
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if ( retval != PAPI_VER_CURRENT )
    esd_metric_error(retval, "PAPI_library_init");

  var = calloc(strlen(env) + 1, sizeof(char));
  strcpy(var, env);
  elg_cntl_msg("EPK_METRICS=%s", var);
        
  /* read metrics from specification string */
  token = strtok(var, ":");
  while ( token && (nmetrics < ELG_METRIC_MAXNUM) ) {
    /* search metricmap for a suitable definition */
    epk_metricmap_t* map = mapv;
    /*printf("Token%d: <%s>\n", nmetrics, token);*/
    while (map != NULL) {
      if ( strcmp(map->event_name, token) == 0 ) {
        /*printf("Definition %s = <%s>\n", token, map->alias_name);*/
        /* expand definition and set components */
        char* c_token = map->alias_name;
        int len = strcspn(c_token, " \t"); /* first token */
        int got_valid_match = 1; /* to be verified */
        int k = 0;
        do { /* verify each component of definition is available */
          char component[64];
          int code = -1;
          strncpy(component, c_token, len);
          component[len] = '\0';
          /*printf("Comp[%d] <%s>\n", k, component);*/
          c_token += len + strspn(c_token+len, " \t");
          len = strcspn(c_token, " \t"); /* next token */

          retval = PAPI_event_name_to_code(component, &code);
          if (retval == PAPI_OK) {
            memset(&info, 0, sizeof(PAPI_event_info_t));
            retval = PAPI_get_event_info(code, &info);
          }
          if (retval != PAPI_OK) {
              elg_warning("Event %s *N/A*", component);
              got_valid_match = 0;
          } else if ((k==0) && (len==0)) { /* use provided event name */
              metricv_add(token, code);
          } else { /* use alias component name */
              metricv_add(component, code);
          }
          elg_cntl_msg("v[%d] %s [0x%X] %d", k, component, code, info.count);
          k++;
        } while (got_valid_match && (len > 0));
        if (got_valid_match) {
            /*printf("Definition %s = <%s> OK\n", map->event_name, map->alias_name);*/
            break; /* accept this event definition */
        }
      }
      map = map->next;
    }

    if (map == NULL) { /* no map match, so try given name */
      int code = -1;
      char* component = token;
      /*elg_warning("v[%d] <%s>", nmetrics, component);*/
      retval = PAPI_event_name_to_code(component, &code);
	    
	    /* KF: the retval of PAPI_event_name_to_code seems to indicate
	       failure even when things went alright with non-cpu components, 
	       workaround: call PAPI_query_event() afterwords, that seems to 
	       correctly detect failure/success */
	    retval = PAPI_query_event( code );

      if ((retval != PAPI_OK) || (code == -1))
          elg_error_msg("Metric <%s> not supported", component);

      memset(&info, 0, sizeof(PAPI_event_info_t));
      retval = PAPI_get_event_info(code, &info);
      if (retval != PAPI_OK)
          esd_metric_error(retval, "PAPI_get_event_info");
      /*elg_cntl_msg("Metric <%s> info has %d fields", component, info.count);*/

      elg_cntl_msg("v[%d] %s [0x%X] %d", nmetrics, component, code, info.count);

      metricv_add(component, code);
    }

    token = strtok(NULL, ":");
  }

  /*printf("nmetrics=%d\n", nmetrics);*/

  /* clean up */
  epk_metricmap_free(mapv);
  free(var);

  /* Check whether event combination is valid. This is done here to
     avoid errors when creating the event set for each thread, which
     would multiply the error messages. */
  esd_metric_test();

  esd_metric_descriptions();

  return nmetrics;
}

void esd_metric_close()
{
  int i;
  
  for ( i = 0; i < nmetrics; i++ ) {
    free (metricv[i]->name);
    free(metricv[i]);
  }
}

void esd_metric_thread_init(int (*id_fn)(void))
{
  int retval;

  if ( nmetrics == 0 )
    return;

  retval = PAPI_thread_init((unsigned long (*)(void))(id_fn));
  if ( retval != PAPI_OK)
    esd_metric_error(retval, "PAPI_thread_init");
  elg_cntl_msg("PAPI thread support initialized");
}


int esd_metric_num()
{
  return nmetrics;
}

double esd_metric_clckrt()
{
  const char* env;
  const PAPI_hw_info_t* hwinfo = NULL;

  if (!PAPI_is_initialized()) {
      /* read configuration variable "EPK_METRICS". Return if unset. */
      env = epk_get(EPK_METRICS);
      if ( env == NULL || strlen(env)==0)
          return 0.0;

      /* initialize PAPI, since it hasn't already been initialized */
      int retval = PAPI_library_init(PAPI_VER_CURRENT);
      if ( retval != PAPI_VER_CURRENT )
        esd_metric_error(retval, "PAPI_library_init");
  }

  hwinfo = PAPI_get_hardware_info(); 
  if ( hwinfo == NULL)
    elg_error_msg("Failed to access PAPI hardware info");
  elg_cntl_msg("Clock rate: %f MHz", hwinfo->mhz);

  return hwinfo->mhz * 1000000.0;
}

const char* esd_metric_name(int i)
{
  return metricv[i]->name;
}

const char* esd_metric_descr(int i)
{
  return metricv[i]->descr;
}

int esd_metric_dtype(int i)
{
  return ELG_INTEGER;
}

int esd_metric_mode(int i)
{
  return ELG_COUNTER;
}

int esd_metric_iv(int i)
{
  return ELG_START;
}


struct esd_metv* esd_metric_create()
{
  struct esd_metv* metv;
  int retval, i, /* KF */ comp;
  
  if ( nmetrics == 0 )
    return NULL;
  
  metv = malloc(sizeof(struct esd_metv));
  if ( metv == NULL )
    elg_error();
  
  /* create event set */
  /* KF: create one per component */
  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    metv->EventSet[i]  = PAPI_NULL;
    metv->NumEvents[i] = 0;
      
    retval = PAPI_create_eventset(&metv->EventSet[i]);
    if ( retval != PAPI_OK)
      esd_metric_error(retval, "PAPI_create_eventset");
  }
  
  for ( i = 0; i < nmetrics; i++ ) {
    /* add event to event set */
      
    /* KF: figure out the right component and track mapping*/
    comp = PAPI_COMPONENT_INDEX( metricv[i]->papi_code );
    metv->Comp2Metr[comp][metv->NumEvents[comp]]=i;

    retval = PAPI_add_event(metv->EventSet[comp], metricv[i]->papi_code);
    if ( retval != PAPI_OK )
      esd_metric_error(retval, "PAPI_add_event");
    else /* KF: success */
      metv->NumEvents[comp]++;
  }

  /* KF: start only those event sets with events added */
  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    if( metv->NumEvents[i]>0 ) {
      retval = PAPI_start(metv->EventSet[i]);
      if ( retval != PAPI_OK )
        esd_metric_error(retval, "PAPI_start");
    }
  }

  elg_cntl_msg("Counters started");

  return metv;
}

void esd_metric_free(struct esd_metv* metv)
{
  int retval, i;
  long_long papi_vals[ELG_METRIC_MAXNUM];

  if ( metv == NULL )
    return;

  /* treat PAPI failures at this point as non-fatal */

  /* KF: do everything for all components with counters */
  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    if( metv->NumEvents[i]==0 )
      continue;
      
    retval = PAPI_stop(metv->EventSet[i], papi_vals);
    if ( retval != PAPI_OK ) {
      esd_metric_warning(retval, "PAPI_stop");
    } else { /* cleanup/destroy require successful PAPI_stop */
	
      retval = PAPI_cleanup_eventset(metv->EventSet[i]);
      if ( retval != PAPI_OK )
        esd_metric_warning(retval, "PAPI_cleanup_eventset");
	
      retval = PAPI_destroy_eventset(&metv->EventSet[i]);
      if ( retval != PAPI_OK )
        esd_metric_warning(retval, "PAPI_destroy_eventset");
	
      free(metv);
	
      elg_cntl_msg("Counters stopped");
    }
  }
}

void esd_metric_read(struct esd_metv* metv, elg_ui8 values[])
{
  int retval;
  int i, j, k;
  long_long papi_vals[ELG_METRIC_MAXNUM];
  
  if ( metv == NULL )
    return;

  /* KF: since contiguous counters in a component could be
     non-contiguous in the metric specification we always have 
     to read in a temp. array and redistribute counter values
     accordingly */

  for( i=0; i<ELG_PAPIC_MAX_COMP; i++ ) {
    if( (metv->NumEvents[i])==0 )
      continue;

    retval = PAPI_read(metv->EventSet[i], papi_vals);
    if ( retval != PAPI_OK )
      esd_metric_error(retval, "PAPI_read");

    for( j=0; j<metv->NumEvents[i]; j++ ) {
      k = metv->Comp2Metr[i][j];
      values[k] = (elg_ui8) papi_vals[j];
    }
  }
}
