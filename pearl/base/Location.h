/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_LOCATION_H
#define PEARL_LOCATION_H


#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Location.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Location and related functions.
 *
 * This header file provides the declaration of the class Location and all
 * related functions.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Machine;
class Node;
class Process;
class Thread;


/*-------------------------------------------------------------------------*/
/**
 * @class   Location
 * @ingroup PEARL_base
 * @brief   Stores the information that uniquely identifies a location in
 *          the system hierarchy.
 *
 * Instances of the class Location provide information about a specific
 * location in the system hierarchy. A location is a tuple consisting of
 * a Machine, a Node, a Process, and a Thread. There is a one-to-one mapping
 * between locations and threads, but a location additionally provides
 * direct access to the upper levels of the system hierarchy.
 *
 * The numerical identifiers of the individual locations are globally defined
 * and continuously enumerated, i.e., the ID is element of [0,@#locations-1].
 *
 * @note The Machine, Node, Process, and Thread objects associated to a
 *       location are only referenced and not owned by the corresponding
 *       Location object. Therefore, they will not be deleted if the
 *       Location object is released.
 */
/*-------------------------------------------------------------------------*/

class Location : public IdObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Location(ident_t  id,
             Machine* machine,
             Node*    node,
             Process* process,
             Thread*  thread);

    /// @}
    /// @name Get location information
    /// @{

    Machine* get_machine() const;
    Node*    get_node() const;
    Process* get_process() const;
    Thread*  get_thread() const;

    /// @}


  private:
    /// %Machine the node belongs to
    Machine* m_machine;

    /// %Node the process is running on
    Node* m_node;

    /// %Process the thread belongs to
    Process* m_process;

    /// %Thread corresponding to this location
    Thread* m_thread;
};


//--- Related functions -----------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const Location& item);


}   /* namespace pearl */


#endif   /* !PEARL_LOCATION_H */
