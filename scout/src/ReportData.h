/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_REPORTDATA_H
#define SCOUT_REPORTDATA_H


#include <map>
#include <vector>

#include <cube.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  ReportData.h
 * @brief Declaration of the ReportData struct.
 *
 * This header file provides the declaration of the ReportData struct which
 * stores the data required for generating the final analysis report.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

/*-------------------------------------------------------------------------*/
/**
 * @struct ReportData
 * @brief  Datatype aggregating all data which is required for generating
 *         the final analysis report.
 *
 * The ReportData struct provides a datatype which aggregates a number of
 * data structures required to produce the final analysis report. In
 * particular, these are mappings from SCOUT-internal object identifiers
 * to the corresponding CUBE objects.
 */
/*-------------------------------------------------------------------------*/

struct ReportData
{
  /// CUBE output object
  cube_t* cb;

  /// Location ID to CUBE thread mapping
  std::vector<cube_thread*> locs;

  /// Region ID to CUBE region mapping
  std::vector<cube_region*> regions;

  /// Call-tree node to CUBE call-tree node mapping
  std::vector<cube_cnode*> cnodes;

  /// Pattern ID to CUBE metric mapping
  std::map<long, cube_metric*> metrics;
};


}   // namespace scout


#endif   // !SCOUT_REPORTDATA_H
