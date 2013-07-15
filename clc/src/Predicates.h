/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <Event.h>
#include <clc_types.h>

// Predicate functions
bool isSend  (const pearl::Event& event, int rank);
bool isRecv  (const pearl::Event& event, int rank);
bool isNxN   (const pearl::Event& event);
bool isNx1   (const pearl::Event& event);
bool is1xN   (const pearl::Event& event);
bool isOnRoot(const pearl::Event& event, int rank);
bool is_mpi_scan(const pearl::Event& event);
bool is_mpi_exscan(const pearl::Event& event);
bool isParallel(const pearl::Event& event);
bool isclcRoot(clc_location loc);
std::string lowercase(const std::string& str);
