/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include "opari2/pomp2_lib.h"
#include "pomp2_region_info.h"

#include "elg_error.h"

#include "epk_pomp2.h"
#include "epk_omplock.h"
#include "epk_ompreg.h"

#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

/*
 * Global variables
 */

int pomp2_tracing = 0;
int pomp_omp_regions = 0;
int pomp_usr_regions = 0;

EPIK_Region_descriptor* epik_region_descriptors;


/*
 * C pomp2 function library
 */

void POMP2_Status() {
#ifdef ELG_OMP
    elg_cntl_msg("%d OpenMP regions and %d user-defined POMP regions",
                pomp_omp_regions, pomp_usr_regions);
#else
    elg_cntl_msg("%d user-defined POMP regions", pomp_usr_regions);
#endif
}

void
POMP2_Finalize()
{
    static int   pomp2_finalize_called = 0;

    if ( epik_region_descriptors )
    {
        free( epik_region_descriptors );
        epik_region_descriptors = 0;
    }

    if ( !pomp2_finalize_called )
    {
        pomp2_finalize_called = 1;
    }
}

void
POMP2_Init()
{
  static int pomp2_init_called = 0;
  int pomp2_max_id = POMP2_Get_num_regions();

    if ( !pomp2_init_called )
    {
      pomp2_init_called = 1;

      atexit( POMP2_Finalize );
        
      esd_open();

      /* Allocate memory for your pomp2_max_id regions */
      epik_region_descriptors = calloc(pomp2_max_id, sizeof(EPIK_Region_descriptor));
      if (!epik_region_descriptors)
        elg_error_msg("POMP2_Init: Failed to allocate table for %d regions", pomp2_max_id);

#ifdef ELG_OMP
      /* register wrapper functions for OpenMP API */
      epk_omp_register();
#endif

      /* register POMP regions (OpenMP + user) */
      POMP2_Init_regions();

      pomp2_tracing = 1;

      if ((pomp_omp_regions+pomp_usr_regions)==0)
        elg_warning("POMP2_Init: No POMP2 instrumented regions defined! (POMP_MAX_ID=%d)", pomp2_max_id);
      else POMP2_Status();

      POMP2_On();
    }
}

void
POMP2_Off()
{
    pomp2_tracing = 0;
}

void
POMP2_On()
{
    pomp2_tracing += 1;
}

void
POMP2_Begin( POMP2_Region_handle* pomp2_handle )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_End( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

int POMP2_Register_region ( POMP2_Region_info*      regionInfo, 
                            EPIK_Region_descriptor* regDescr,
                            const char*             rdesc )
{
  const char* pompRegionName = pomp2RegionType2String( regionInfo->mRegionType );
  char* regionName;
  char anno_regionName[256]; // annotated region name 
  elg_ui4 regionType = ELG_NO_ID;
  elg_ui4 fid = esd_def_file(regionInfo->mStartFileName);

  switch ( regionInfo->mRegionType ) {
  case POMP2_User_region:
    regionType = ELG_USER_REGION;
    regionName = regionInfo->mUserRegionName;
    break;
#ifdef ELG_OMP
  case POMP2_Atomic:
    regionType = ELG_OMP_ATOMIC;
    regionName = "!$omp atomic";
    break;
  case POMP2_Barrier:
    regionType = ELG_OMP_BARRIER;
    regionName = "!$omp barrier";
    break;
  case POMP2_Critical:
    regionType = ELG_OMP_CRITICAL;
    regionName = "!$omp critical"; 
    sprintf(anno_regionName, "%s @%s:%d", "!$omp critical sblock",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine2+1);
    regDescr->sbrid = esd_def_region(anno_regionName,
                                     fid, regionInfo->mStartLine2+1,
                                     regionInfo->mEndLine1-1, "POMP",
                                     ELG_OMP_CRITICAL_SBLOCK);
    if ( regionInfo->mCriticalName ) {
      regionName = malloc(24 + strlen(regionInfo->mCriticalName));
      sprintf(regionName, "!$omp critical %s", regionInfo->mCriticalName);
      regDescr->brid = epk_lock_id(regionInfo->mCriticalName);
      if (regDescr->brid == ELG_NO_ID) {
        regDescr->brid = epk_lock_init(regionInfo->mCriticalName);
      }
    } else {
      regDescr->brid = epk_lock_id("default");
      if (regDescr->brid == ELG_NO_ID) {
        regDescr->brid = epk_lock_init("default");
      }      
    }
    break;
  case POMP2_Do:
    regionType = ELG_OMP_LOOP;
    regionName = "!$omp do";
    break;
  case POMP2_Flush:
    regionType = ELG_OMP_FLUSH;
    regionName = "!$omp flush";
    break;
  case POMP2_For:
    regionType = ELG_OMP_LOOP;
    regionName = "!$omp for";
    break;
  case POMP2_Master:
    regionType = ELG_OMP_MASTER;
    regionName = "!$omp master";
    break;
  case POMP2_Ordered:
    regionType = ELG_OMP_ORDERED;
    regionName = "!$omp ordered";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp ordered sblock",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine2+1);
    regDescr->sbrid = esd_def_region(anno_regionName, fid,
                                     regionInfo->mStartLine2+1,
                                     regionInfo->mEndLine1-1, "POMP",
                                     ELG_OMP_ORDERED_SBLOCK);
    break;
  case POMP2_Parallel:
    regionType = ELG_OMP_PARALLEL;
    regionName = "!$omp parallel";
    break;
  case POMP2_Parallel_do:
    regionType = ELG_OMP_PARALLEL;
    regionName = "!$omp parallel";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp do",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->sbrid = esd_def_region(anno_regionName, fid,
                                     regionInfo->mStartLine1,
                                     regionInfo->mEndLine2, rdesc,
                                     ELG_OMP_LOOP);
    break;
  case POMP2_Parallel_for:
    regionType = ELG_OMP_PARALLEL;
    regionName = "!$omp parallel";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp for",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->sbrid = esd_def_region(anno_regionName, fid,
                                     regionInfo->mStartLine1,
                                     regionInfo->mEndLine2, rdesc,
                                     ELG_OMP_LOOP);
    break;
  case POMP2_Parallel_sections:
    regionType = ELG_OMP_PARALLEL;
    regionName = "!$omp parallel";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp sections",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->sbrid = esd_def_region(anno_regionName, fid,
                                     regionInfo->mStartLine1,
                                     regionInfo->mEndLine1, rdesc,
                                     ELG_OMP_SECTIONS);
    break;
  case POMP2_Parallel_workshare:
    regionType = ELG_OMP_PARALLEL;
    regionName = "!$omp parallel";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp workshare",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->sbrid = esd_def_region(anno_regionName, fid,
                                     regionInfo->mStartLine1,
                                     regionInfo->mEndLine1, rdesc,
                                     ELG_OMP_WORKSHARE);
    break;
  case POMP2_Sections:
    regionType = ELG_OMP_SECTIONS;
    regionName = "!$omp sections";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp section",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->sbrid = esd_def_region(anno_regionName,
                                     fid, regionInfo->mStartLine1,
                                     regionInfo->mEndLine2, rdesc,
                                     ELG_OMP_SECTION);
    break;
  case POMP2_Single:
    regionType = ELG_OMP_SINGLE;
    regionName = "!$omp single";
    sprintf(anno_regionName, "%s @%s:%d", "!$omp single sblock",
            basename(regionInfo->mStartFileName), regionInfo->mStartLine2+1);
    regDescr->sbrid = esd_def_region(anno_regionName,
                                     fid, regionInfo->mStartLine2+1,
                                     regionInfo->mEndLine1-1, rdesc,
                                     ELG_OMP_SINGLE_SBLOCK);
    break;
  case POMP2_Task:
    /* TODO */
    break;
  case POMP2_Taskuntied:
    /* TODO */
    break;
  case POMP2_Taskwait:
    /* TODO */
    break;
  case POMP2_Workshare:
      regionType = ELG_OMP_WORKSHARE;
      regionName = "!$omp workshare";
    break;
#endif
  case POMP2_No_type:
  default:
    regionType = ELG_UNKNOWN;
    regionName = (char*) pompRegionName;
    rdesc = "UNKNOWN";
    break;
  } 

  /* register region */
  if ( regionType == ELG_USER_REGION ) {
    regDescr->rid = esd_def_region(regionInfo->mUserRegionName,
                               fid,
                               regionInfo->mStartLine1,
                               regionInfo->mEndLine2,
                               "USR",
                               regionType);
    regDescr->sbrid = ELG_NO_ID;
    regDescr->brid  = ELG_NO_ID;
    regDescr->regionQualifier = pompRegionName[0];
    elg_cntl_msg("POMP2[%d]: rid=%d %s:\"%s\"", pomp_omp_regions,
                 regDescr->rid, rdesc, regionName);
    pomp_usr_regions++;
    return 1;
  } else if ( regionType != ELG_NO_ID ) {
    /* process OpenMP regions */
    sprintf(anno_regionName, "%s @%s:%d", regionName,
            basename(regionInfo->mStartFileName), regionInfo->mStartLine1);
    regDescr->rid = esd_def_region(anno_regionName, 
                               fid,
                               regionInfo->mStartLine1,
                               regionInfo->mEndLine2,
                               rdesc, 
                               regionType);
    elg_cntl_msg("POMP2[%d]: rid=%d %s:\"%s\"", pomp_omp_regions,
                 regDescr->rid, rdesc, regionName);
    if (regionType == ELG_OMP_PARALLEL ||
        regionType == ELG_OMP_LOOP     ||
        regionType == ELG_OMP_SECTIONS ||
        regionType == ELG_OMP_SINGLE   ||
        regionType == ELG_OMP_WORKSHARE) {
          /* -- register implicit barrier -- */
          regionName = "!$omp ibarrier";
          sprintf(anno_regionName, "%s @%s:%d", regionName,
                  basename(regionInfo->mEndFileName), regionInfo->mEndLine1);
          regDescr->brid = esd_def_region(anno_regionName,
                                  fid, 
                                  regionInfo->mEndLine1,
                                  regionInfo->mEndLine1, 
                                  rdesc, 
                                  ELG_OMP_IBARRIER);
          elg_cntl_msg("POMP2[%d]: rid=%d %s:\"%s\"", pomp_omp_regions,
                       regDescr->brid, rdesc, anno_regionName);
    } else if (regionType != ELG_OMP_CRITICAL) {
          regDescr->brid = ELG_NO_ID;
    }
    regDescr->regionQualifier = pompRegionName[0];
    pomp_omp_regions++;
    return 1;
  } 
  return 0;
}


void
POMP2_Assign_handle( POMP2_Region_handle* region_handle,
                     const char           init_string[] )
{
  static int count = 0;
  EPIK_Region_descriptor* epikRegDescr = &epik_region_descriptors[count];
  const char* rdesc = "OMP";
  POMP2_Region_info regionInfo;

  ctcString2RegionInfo(init_string, &regionInfo);

  elg_cntl_msg("POMP2[%d]: Assign handle for: %s, in file: %s:%i", 
               count,
               pomp2RegionType2String( regionInfo.mRegionType ), 
               regionInfo.mStartFileName,
               regionInfo.mStartLine1);

  /* only return region descriptor (and increment count) if 
   * a new region was registered ( returnvalue == true */
  if ( POMP2_Register_region( &regionInfo, epikRegDescr, rdesc ) ) {
    elg_cntl_msg("POMP[%d]: Assigned handle - rid=%d %s:\"%s\"-----------------%p\n", 
               count, epikRegDescr->brid, "OMP", 
               pomp2RegionType2String( regionInfo.mRegionType), epikRegDescr );

    // Set return value
    *region_handle = epikRegDescr;

    // Increase array index
    ++count;
  }
}
