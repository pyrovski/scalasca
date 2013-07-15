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

#include "epk_conf.h"
#include "elg_error.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#undef KOJAK_DEFAULTS

#ifndef STANDALONE

#define EPIK_CONF         "EPIK.CONF"

#ifdef DOCDIR
#define G_CONF            DOCDIR "/" EPIK_CONF
#endif

#ifndef PFORM_LDIR
#define PFORM_LDIR "."
#endif

#ifndef MACHINE
#define MACHINE "Machine"
#endif 

#ifdef ELG_COMPRESSED
#define COMPRESSION "6"
#else
#define COMPRESSION "off" /* no compression support */
#endif

#define MAX_LN_LEN       512

typedef struct {
          char*    key;
          char*    def;
          char*    comment;
	  union {
	    epk_type_i  eid; /* only used at setup time for consistency check */
            char*       val;
	  } u;
        } epk_data;

/* configuration variable table, indexed by epk_type_i */

static epk_data epk_vars []= {
  {"EPK_CONF",NULL,"E P I K configuration",{EPK_CONF}},
  {"EPK_TITLE","a","Experiment archive title",{EPK_TITLE}},
  {"EPK_GDIR",".","Name of global, cluster-wide directory to store final experiment archive",{EPK_GDIR}},
  {"EPK_LDIR",PFORM_LDIR,"Name of (node-local) directory that can be used to store temporary files",{EPK_LDIR}},
  {"EPK_FILTER","","Name of file containing a list of names of functions (one per line)\n"
                   "# which should be ignored during the measurement",{EPK_FILTER}},
  {"EPK_METRICS","",
#ifdef EPK_METR
        "Colon-separated list of counter metrics to be measured with events",
#else
        NULL,
#endif
	{EPK_METRICS}},
  {"EPK_METRICS_SPEC","$DOCDIR/METRICS.SPEC",
#ifdef EPK_METR
        "Name of file specifying metrics, groups and relations",
#else
        NULL,
#endif
	{EPK_METRICS_SPEC}},
  {"EPK_CLEAN","1","Automatic removal of intermediate files",{EPK_CLEAN}},
#ifdef KOJAK_DEFAULTS
  {"EPK_SUMMARY","0","Run-time summarization",{EPK_SUMMARY}},
  {"EPK_TRACE","1","Event trace collection",{EPK_TRACE}},
#else
  {"EPK_SUMMARY","1","Run-time summarization",{EPK_SUMMARY}},
  {"EPK_TRACE","0","Event trace collection",{EPK_TRACE}},
#endif
  {"EPK_MACHINE_ID","0","Unique identifier of the Machine",{EPK_MACHINE_ID}},
  {"EPK_MACHINE_NAME",MACHINE,"Define the name of the Machine",{EPK_MACHINE_NAME}},
  {"EPK_VERBOSE","0","Verbosity of EPIK related control information reported",{EPK_VERBOSE}},
  /** @anchor mpi_enabled_defaults CG:COLL:ENV:IO:P2P:RMA:TOPO */
  {"EPK_MPI_ENABLED","CG:COLL:ENV:IO:P2P:RMA:TOPO","Enable measurement for specific parts of MPI",{EPK_MPI_ENABLED}},
  {"EPK_MPI_HANDLES","64","Maximum number of MPI comm/group/window/epoch handles tracked simultaneously",{EPK_MPI_HANDLES}},

  {"ESD_CONF",NULL,"E P I S O D E configuration",{ESD_CONF}},
  {"ESD_PATHS","4096","Maximum number of measurement call-paths",{ESD_PATHS}},
  {"ESD_FRAMES","32","Maximum number of measurement stack frames",{ESD_FRAMES}},
  {"ESD_BUFFER_SIZE","100000","Size of internal definitions buffer in bytes",{ESD_BUFFER_SIZE}},
  {"ESD_MAX_THREADS","OMP_NUM_THREADS","Maximum number of threads for which measurements should be taken",{ESD_MAX_THREADS}},

  {"ELG_CONF",NULL,"E P I L O G configuration",{ELG_CONF}},
  {"ELG_BUFFER_SIZE","10000000","Size of internal event trace buffer in bytes",{ELG_BUFFER_SIZE}},
  {"ELG_COMPRESSION",COMPRESSION,"Compression level of trace files (0..9 or u=uncompressed)",{ELG_COMPRESSION}},
  {"ELG_MERGE","0","Automatic merge of trace files",{ELG_MERGE}},
  {"ELG_VT_MODE","0","Generate VAMPIR-compatible traces",{ELG_VT_MODE}},
#ifdef USE_SIONLIB
  {"ELG_SION_FILES","0","number of physical sion files  (0 = one file per process, without sionlib)",{ELG_SION_FILES}},
#endif
};


static char* replace_vars(char *v) {
  char* start;
  char* end;
  char* vname;
  char* vval;
  char* res;
  int extra = 0;
  int plen = 0;

  if ((start = strchr(v, '$')) == NULL ) {
    /* no $ in v -> no replacement necessary */
    return v;
  } else {
    if ( start[1] == '{' ) {
      /* ${....} form */
      extra = 1;
      end = start += 2;
      while ( *end && *end != '}' ) end++;
    } else {
      /* $### form where # is letter, digit, or underscore */
      end = ++start;
      while ( *end && (isalnum(*end) || *end == '_')) end++;
    }
    /* determine name of variable */
    vname = malloc(end-start+1);
    strncpy(vname, start, end-start);
    vname[end-start] = '\0';
    /* get its content */
#ifdef DOCDIR
    if (!strcmp(vname,"DOCDIR")) {
      vval=DOCDIR;
    }
    else 
#endif
    if ((vval = getenv(vname)) == NULL) vval = "";
    free(vname);
    /* put together string with variable replaced by value */
    /* -- allocate enough space and copy stuff before variable part */
    res = malloc(strlen(v)+strlen(vval));
    plen = (start - v) - 1 - extra;
    if (plen) strncpy(res, v, plen);
    res[plen] = '\0';
    /* -- add variable content */
    strcat(res, vval);
    /* -- add stuff after variable */
    if ( *end ) strcat(res, end + extra);
    return res;
  }
}

void epk_set(int index, char *val) {
  if (val) {
    if(epk_vars[index].u.val){
      free(epk_vars[index].u.val);
      }
    val=replace_vars(val);
    epk_vars[index].u.val=malloc(strlen(val)+1);
    strcpy(epk_vars[index].u.val,val);
 }
}

int epk_str2int(const char* val) {
  if (!val) return INT_MIN;
  if ( strcmp(val, "unlimited") == 0 ||
       strcmp(val, "Unlimited") == 0 ||
       strcmp(val, "UNLIMITED") == 0 )
    {
      return INT_MAX;
    }
  else if (atoi(val) < 0)
    return INT_MIN;
  else
    return atoi(val);
}

int epk_str2bool(const char *str) {
  char strbuf[128];
  char* ptr = strbuf;

  strncpy(strbuf, str, 128);
  while ( *ptr )
    {
      *ptr = tolower(*ptr);
      ++ptr;
    }

  if ( strcmp(strbuf, "yes") == 0  ||
       strcmp(strbuf, "true") == 0 ||
       atoi(strbuf) > 0 )
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

void epk_conf_set(char *key,char *val) {
  int i;
  for (i=0; i<EPK_CONF_VARS;i++) {
    if (!strcmp(key,(char *)epk_vars[i].key)) {
      epk_set(i,val);
      return;
    }
  }
  elg_warning("Ignoring configuration specification %s=%s", key, val);
}

static void epk_conf_set_epk_defaults() {
  int i;
  for (i=0; i<EPK_CONF_VARS;i++) {
    if (epk_vars[i].u.eid == i) epk_vars[i].u.val = NULL;
    else
        elg_error_msg("Internal epk_vars table key inconsistency: %d <> %d [%s]",
                epk_vars[i].u.eid, i, epk_vars[i].key);
    epk_set(i,epk_vars[i].def);
  }
}

static void epk_conf_set_env() {
  int i;
  for (i=0; i<EPK_CONF_VARS;i++) {
    if (epk_vars[i].def != NULL) epk_set(i,getenv(epk_vars[i].key));
  }
} 

static char *epk_format(char *val)
{
  char *it;

  while(isspace(*val)) {
    val++;
  }

  if (*val==0) {
    return NULL;
  }

  it=val+strlen(val)-1;
  while(isspace(*it)) {
    it--;
  }
  *(++it)=0;
  return val;
  
}

/* Converts a string to positive "int" size (or INT_MIN if invalid) */
int epk_str2size (const char* val) {

  /* Parse size integer and multipler string */ 
  char *multiplier;
  long long size = strtoll (val,&multiplier,10);

  if (size <= 0) return 0; /* INT_MIN? */
  if (size >= INT_MAX) return INT_MAX;

  if ((multiplier != NULL) && (strlen(multiplier) > 0)) {
    if (strcmp(multiplier, "k") == 0 || strcmp(multiplier, "K") == 0) {
      size *= 1024;
      return (size >= INT_MAX) ? INT_MAX : size;
    } else if (strcmp(multiplier, "M") == 0) {
      size *= 1024 * 1024;
      return (size >= INT_MAX) ? INT_MAX : size;
    } else if (strcmp(multiplier, "G") == 0) {
      size *= 1024 * 1024 * 1024;
      return (size >= INT_MAX) ? INT_MAX : size;
    }
    return 0; /* INT_MIN?*/ /* unknown multiplier */
  }

  return size;
}

static void epk_check_syntax (char *val, char *epos, char *fname) {
  char *tmp=val;
  while(isspace(*val)) {
    val++;
  }
  if (val!=epos) {
    elg_warning("Syntax error in %s::%s", fname, tmp);
  }
}

static void epk_parse_config(FILE *cfgFile, char *fname)
{
  char buf[MAX_LN_LEN], *it, *val;
  
  while (fgets(buf, MAX_LN_LEN, cfgFile)) {  
    if ((strlen(buf)==MAX_LN_LEN-1) && (buf[MAX_LN_LEN-1] != '\n')) {
      elg_warning("Syntax error in %s::%s (Skipped parsing at overlong line)\n", fname, buf);
      break;
    }else{
      it = buf;
      while (*it && isspace(*it)) { /* Skip until either end of string or char  */
	it++;
      }
      if (*it=='#') {
	continue;         /* If it is a comment, skip the line */
      }
      while (*it && *it!='=' ) { /* Skip until end of string or = or # */
	it++;
      }
      if (*it!='=') {
	*--it=0;
	epk_check_syntax(buf,it, fname);
	continue;
      }
      *it++ = 0;
      val = it;
      while (*it  && *it!='#') { /* Skip until either  end of string or # */
	it++;
      }
      *it = 0;    
      epk_conf_set(epk_format(buf), epk_format(val));
    }
  }
}

static char* epk_gfile() 
{  
  static char* epk_conf_path = NULL; 
  char * tmp;
  
  if(epk_conf_path == NULL) {
    tmp  = getenv("EPK_CONF"); 
    if (tmp) {
      if (strstr(tmp,EPIK_CONF)) {
	epk_conf_path=malloc(strlen(tmp) + 1);
	strcpy(epk_conf_path,tmp);
      } else {
	epk_conf_path=malloc(strlen(tmp) + strlen(EPIK_CONF) + 2);
	strcpy(epk_conf_path,tmp);
	strcat(epk_conf_path,"/" EPIK_CONF);
      }
    }
#ifdef G_CONF
    else {
      epk_conf_path=malloc(strlen(G_CONF) + 1);
      strcpy(epk_conf_path,G_CONF);
    }
#endif
  }
  return epk_conf_path;
}

void epk_conf()
{
  char msg [256] = "";

  epk_conf_set_epk_defaults();
  if (access(epk_gfile(), R_OK) == 0) {
    FILE *cfgFile=fopen(epk_gfile(), "r");
    epk_parse_config(cfgFile, epk_gfile());
    fclose(cfgFile);
    snprintf(msg, 256, "[EPK_CONF] %s parsed ", epk_gfile());
  }
  else if (epk_gfile()) {
    /* the following should become an elg_warning when EPIK.CONF is
       automatically installed in the installation DOCDIR */
    elg_cntl_msg("[EPK_CONF] Could not access %s",epk_gfile());
  }
  if (access("./" EPIK_CONF, R_OK) == 0) {
    FILE *cfgFile=fopen("./" EPIK_CONF, "r");
    epk_parse_config(cfgFile,"./" EPIK_CONF);
    fclose(cfgFile);
    snprintf(msg+strlen(msg), 256-strlen(msg),"[CURDIR] %s parsed", "./" EPIK_CONF);
  }
  epk_conf_set_env();
  msg[255]=0;
  if (strlen(msg)) elg_cntl_msg(msg);
 
}

void epk_conf_print(FILE *stream) {
  int i;

  for (i=0; i<EPK_CONF_VARS;i++) {
      const char* val = epk_get((epk_type_i)i);
      if (val == NULL)
          fprintf(stream, "## %s\n\n", epk_vars[i].comment);
      else if (epk_vars[i].comment == NULL) continue;
      else if (strstr(epk_vars[i].key, "_SIZE") != NULL) {
          int i_sz = epk_str2int(epk_get((epk_type_i)i));
          int size = epk_str2size(epk_get((epk_type_i)i));
          fprintf(stream, "# %s\n",epk_vars[i].comment);
          fprintf(stream, "%s = %s",epk_vars[i].key,epk_get((epk_type_i)i));
          if (size <= 0) fprintf(stream, "   # INVALID!");
          else if (size != i_sz) fprintf(stream, "   # %d", size);
          fprintf(stream, "\n\n");
      } else {
          fprintf(stream, "# %s\n",epk_vars[i].comment);
          fprintf(stream, "%s = %s\n\n",epk_vars[i].key,epk_get((epk_type_i)i));
      }
  }
}

const char *epk_key(unsigned index) {
  if (index >= EPK_CONF_VARS) return NULL;
  return epk_vars[index].key;
}

const char *epk_get(epk_type_i index) {
  static int isinit = 0;

  if(isinit == 0) {
    isinit++;
    epk_conf();
    elg_verbosity(epk_str2bool(epk_get(EPK_VERBOSE)));
  }
  if (index >= EPK_CONF_VARS) return NULL;
  return epk_vars[(int)index].u.val;
}

#else /* STANDALONE */
int main(int argc, char** argv)
{
  /* check # of arguments */
  if ( argc > 1) {
      elg_error_msg("Wrong # of arguments\nUsage: %s", argv[0]);
      return 1;
    }
  epk_conf_print(stdout);
  return 0;
}
#endif
