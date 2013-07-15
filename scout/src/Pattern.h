/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_PATTERN_H
#define SCOUT_PATTERN_H


#include <cstdio>
#include <map>
#include <string>


/*-------------------------------------------------------------------------*/
/**
 * @file  Pattern.h
 * @brief Declaration of the Pattern class.
 *
 * This header file provides the declaration of the Pattern class which
 * serves as an abstract base class for all performance properties calculated
 * by SCOUT.
 */
/*-------------------------------------------------------------------------*/


//--- Forward declarations --------------------------------------------------

namespace pearl
{

class CallbackManager;
class CNode;
class GlobalDefs;
class LocalTrace;

}   // namespace pearl


namespace scout
{

//--- Forward declarations --------------------------------------------------

struct ReportData;


/*-------------------------------------------------------------------------*/
/**
 * @class Pattern
 * @brief Abstract base class for all properties calculated by SCOUT.
 *
 * This class serves as an abstract base class for all performance properties
 * calculated by SCOUT. This includes basic patterns such as Time and Visits,
 * as well as the inefficiency patterns (i.e., wait states) occuring at
 * synchronization points.
 */
/*-------------------------------------------------------------------------*/

class Pattern
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~Pattern();

    /// @}
    /// @name Registering callbacks
    /// @{

    virtual void reg_cb(pearl::CallbackManager* cbmanager);

    /// @}
    /// @name Retrieving pattern information
    /// @{

    virtual long get_id() const = 0;
    virtual long get_parent() const;
    virtual std::string get_name() const = 0;
    virtual std::string get_unique_name() const = 0;
    virtual std::string get_descr() const = 0;
    virtual std::string get_unit() const = 0;
    std::string get_url() const;
    virtual bool is_hidden() const;

    /// @}
    /// @name Writing severity values
    /// @{

    virtual void gen_severities(ReportData& data, int rank,
                                const pearl::LocalTrace& trace,
                                std::FILE* fp);

    /// @}


  protected:
    /// Local severity map. Stores a cnode @f$\times@f$ severity value matrix.
    std::map<pearl::CNode*,double> m_severity;


    /// @name Pattern management
    /// @{
 
    virtual void init();

    /// @}
    /// @name Data collation control
    /// @{

    virtual bool skip_cnode(const pearl::CNode& cnode) const;

    /// @}
};


}   // namespace scout


#endif   // !SCOUT_PATTERN_H
