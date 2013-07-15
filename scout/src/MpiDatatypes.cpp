/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <epk_defs_mpi.h>

#include "MpiDatatypes.h"
#include "scout_types.h"

using namespace scout;


//--- Global variables ------------------------------------------------------

/// MPI datatype handle for TimeVec2 struct
MPI_Datatype scout::TIMEVEC2;

/// MPI datatype handle for TopMostSevere struct
MPI_Datatype scout::TOPSEVERE;

/// MPI datatype handle for array with TOP_SEVERE_INSTANCES entries of
/// type TopMostSevere
MPI_Datatype scout::TOPSEVEREARRAY;

#ifdef MOST_SEVERE_MAX
  /// MPI datatype handle for TopMostSevereMaxSum struct
  MPI_Datatype scout::TOPSEVERECOLL;
#endif


//--- Related functions -----------------------------------------------------

/**
 * @brief Creates all SCOUT-specific MPI datatypes.
 *
 * This function creates all SCOUT-specific MPI derived datatypes. It can
 * only be called after MPI has been successfully initialized.
 */
void scout::InitDatatypes()
{
  // Define MPI datatype for TimeVec2 struct
  MPI_Type_contiguous(2, MPI_DOUBLE, &TIMEVEC2);
  MPI_Type_commit(&TIMEVEC2);

  // Define MPI datatype for TopMostSevere struct
  TopMostSevere topMost[2];   // We need 2 entries to account for padding
  int           counts[3];
  MPI_Aint      displs[3];
  MPI_Datatype  types[3];

  counts[0] = 3;
  types [0] = MPI_DOUBLE;
  counts[1] = 2;
  types [1] = MPI_UNSIGNED;
  counts[2] = 1;
  types [2] = MPI_UB;
#ifdef HAS_MPI_GET_ADDRESS
  MPI_Get_address(&topMost[0].idletime, &displs[0]);
  MPI_Get_address(&topMost[0].cnode,    &displs[1]);
  MPI_Get_address(&topMost[1],          &displs[2]);
#else
  MPI_Address(&topMost[0].idletime, &displs[0]);
  MPI_Address(&topMost[0].cnode,    &displs[1]);
  MPI_Address(&topMost[1],          &displs[2]);
#endif
  displs[2] -= displs[0];
  displs[1] -= displs[0];
  displs[0]  = 0;
#ifdef HAS_MPI_TYPE_CREATE_STRUCT
  MPI_Type_create_struct(3, counts, displs, types, &TOPSEVERE);
#else
  MPI_Type_struct(3, counts, displs, types, &TOPSEVERE);
#endif
  MPI_Type_commit(&TOPSEVERE);

  // Define MPI datatype for array of TopMostSevere struct entries
  MPI_Type_contiguous(TOP_SEVERE_INSTANCES, TOPSEVERE, &TOPSEVEREARRAY);
  MPI_Type_commit(&TOPSEVEREARRAY);

#ifdef MOST_SEVERE_MAX
  // Define MPI datatype for TopMostSevereMaxSum struct
  TopMostSevereMaxSum topMostCollMaxSum;

  counts[0] = 3;
  types [0] = MPI_DOUBLE;
  counts[1] = 2;
  types [1] = MPI_UNSIGNED;
  counts[2] = 1;
  types [2] = MPI_DOUBLE;
#ifdef HAS_MPI_GET_ADDRESS
  MPI_Get_address(&topMostCollMaxSum.idletime,     &displs[0]);
  MPI_Get_address(&topMostCollMaxSum.cnode,        &displs[1]);
  MPI_Get_address(&topMostCollMaxSum.idletime_max, &displs[2]);
#else
  MPI_Address(&topMostCollMaxSum.idletime,     &displs[0]);
  MPI_Address(&topMostCollMaxSum.cnode,        &displs[1]);
  MPI_Address(&topMostCollMaxSum.idletime_max, &displs[2]);
#endif
  displs[2] -= displs[0];
  displs[1] -= displs[0];
  displs[0]  = 0;
#ifdef HAS_MPI_TYPE_CREATE_STRUCT
  MPI_Type_create_struct(3, counts, displs, types, &TOPSEVERECOLL);
#else
  MPI_Type_struct(3, counts, displs, types, &TOPSEVERECOLL);
#endif
  MPI_Type_commit(&TOPSEVERECOLL);
#endif // MOST_SEVERE_MAX
}


/**
 * @brief Releases all SCOUT-specific MPI datatypes.
 *
 * This function releases all SCOUT-specific MPI derived datatypes created
 * by InitDatatypes(). It must be called before MPI is finalized.
 */
void scout::FreeDatatypes()
{
  // Release MPI datatypes
#ifdef MOST_SEVERE_MAX
  MPI_Type_free(&TOPSEVERECOLL);
#endif
  MPI_Type_free(&TOPSEVEREARRAY);
  MPI_Type_free(&TOPSEVERE);
  MPI_Type_free(&TIMEVEC2);
}
