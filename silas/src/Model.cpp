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

#include <map>
#include <set>
#include <silas_util.h>

#include "CallbackManager.h"    // pearl::CallbackManager
#include "Model.h"              // silas::Model

using namespace std;
using namespace pearl;
using namespace silas;

Model::~Model()
{
}

void Model::set_flag(const std::string& flag) 
{ 
    log("setting flag: %s\n", flag.c_str());
    m_flags.insert(flag);
}

void Model::unset_flag(const std::string& flag)
{
    std::set<std::string>::iterator it = m_flags.find(flag);

    if (it != m_flags.end())
        m_flags.erase(it);
}

bool Model::is_set(const std::string& flag) const 
{
    std::set<std::string>::const_iterator it = m_flags.find(flag);

    if (it != m_flags.end())
        return true;

    return false;
}

void Model::set_option(const std::string& option, 
        const std::string& value)
{
    log("setting option: %s = %s\n", option.c_str(), value.c_str()); 
    m_options[option] = value;
}

void Model::unset_option(const std::string& option) 
{
    std::map<std::string, std::string>::iterator it = 
            m_options.find(option);
    
    if (it != m_options.end())
        m_options.erase(it);
}

const std::string Model::get_option(const std::string& option, 
                                    const std::string& defval) const 
{ 
    std::map<std::string, std::string>::const_iterator it = 
            m_options.find(option);

    return it == m_options.end() ? std::string(defval) : it->second;
}
