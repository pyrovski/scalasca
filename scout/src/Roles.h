/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_ROLES_H
#define SCOUT_ROLES_H


#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  Roles.h
 * @brief Declaration of constants for event roles.
 *
 * This header file provides a set of numerical constants which provide
 * symbolic names for the different roles that can be attributed to events.
 * These are used when transferring events between processes using the
 * pearl::EventSet and pearl::RemoteEventSet classes, respectively.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Role ID constants -----------------------------------------------------

const uint32_t ROLE_SEND              =  0;
const uint32_t ROLE_RECV              =  1;
const uint32_t ROLE_ENTER_SEND        =  2;
const uint32_t ROLE_ENTER_RECV        =  3;
const uint32_t ROLE_EXIT_SEND         =  4;
const uint32_t ROLE_EXIT_RECV         =  5;
                                   
const uint32_t ROLE_SEND_LS           =  6;
const uint32_t ROLE_RECV_LS           =  7;
const uint32_t ROLE_ENTER_SEND_LS     =  8;
const uint32_t ROLE_ENTER_RECV_LS     =  9;
const uint32_t ROLE_EXIT_SEND_LS      = 10;
const uint32_t ROLE_EXIT_RECV_LS      = 11;

const uint32_t ROLE_SEND_LSWO         = 12;
const uint32_t ROLE_RECV_LSWO         = 13;

const uint32_t ROLE_SEND_LR           = 14;
const uint32_t ROLE_RECV_LR           = 15;
const uint32_t ROLE_ENTER_SEND_LR     = 16;
const uint32_t ROLE_ENTER_RECV_LR     = 17;
const uint32_t ROLE_EXIT_SEND_LR      = 18;
const uint32_t ROLE_EXIT_RECV_LR      = 19;

const uint32_t ROLE_COLL_ENTER        = 20;
const uint32_t ROLE_COLL_EXIT         = 21;

const uint32_t ROLE_BARRIER_ENTER     = 22;
const uint32_t ROLE_BARRIER_EXIT      = 23;
                                     
const uint32_t ROLE_RMA_START_ENTER   = 24;
const uint32_t ROLE_RMA_START_EXIT    = 25;
const uint32_t ROLE_RMA_COMPLETE_EXIT = 26;


}   // namespace scout


#endif   // !SCOUT_ROLES_H
