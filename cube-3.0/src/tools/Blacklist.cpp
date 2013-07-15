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
/**
* \file Blacklist.cpp
* \brief Defines a blacklist. 
*/


#include "Blacklist.h"

#include <cstdlib>
#include <fstream>
#include <limits.h>
#ifdef __MINGW32__
#  include <string.h>
#  define fnmatch(s1,s2,mode) strcmp(s1,s2)
#else
#  include <fnmatch.h>
#endif

#include "Region.h"
#include "Cube.h"
#include "RegionInfo.h"
#include "helper.h"

using namespace std;

namespace cube
{
    CBlacklist::CBlacklist(Cube* input, std::string sFilename)
	: m_cube(input), m_reginfo(0), good(false)
    {
	init(sFilename);
    }
  
    CBlacklist::CBlacklist(Cube* input, std::string sFilename, const CRegionInfo* ri)
	: m_cube(input), m_reginfo(ri), good(false)
    {
	init(sFilename);
    }

    CBlacklist::CBlacklist(Cube* input, const CRegionInfo* ri)
	: m_cube(input), m_reginfo(ri), good(false)
    {
	initUSR();
    }

/**
* Initialize a blacklist.
* Steps:
* - Read a list of methods. Save then from file in a buffer. 
* - See if each of the strings from vRegionnames  describes a valid region and 
if so insert the id of the region to vBlacklist and see that vBlacklist  stays sorted.
* - Print warnings for unknown regions and regions of type EPK, MPI, OMP and COM.
*/
    void CBlacklist::init(std::string sFilename)
    {
	ifstream rl(sFilename.c_str());
    
	if (!rl)
	{
	    cerr << "[Filter] Error: open " << sFilename << endl;
            exit(1);
	}
	else
	{
	    vector<string> vRegionnames;
            unsigned int line = 0;

	    while (!rl.eof())
	    {
                string buffer;
                rl >> buffer;
                line++;

                if (buffer.empty())
                  continue;
                if (buffer[0] == '#') { // ignore rest of line
                  rl.ignore(4096,'\n');
                  continue;
                }
#ifdef DEBUG
                std::cerr << "[Filter] " << buffer << endl;
#endif
                if (buffer.length() > 1023) {
                  std::cerr << "[Filter] Ignoring overlong filter line "
                            << line << endl;
                  continue;
		}
                else
		{
		    vRegionnames.push_back(buffer);
#ifdef DEBUG
		    std::cerr << "[Filter] pushing method " << buffer << endl;
#endif
		}
                rl.ignore(4096,'\n'); // ignore rest of line
	    }

	    const vector<Region*>& vRegions = m_cube->get_regv();
	    const size_t rcnt = vRegions.size();
	    const size_t numstrings = vRegionnames.size();
	    vBlacklist.reserve(numstrings+1);

	    /*
	      see if each of the strings from vRegionnames
	      describes a valid region and if so insert the id
	      of the region to vBlacklist and see that vBlacklist
	      stays sorted.

	      Print warnings for unknown regions and regions of
	      type MPI, OMP and COM.
	    */

	    for (size_t j=0; j<rcnt; j++)
	    {
	        for (size_t i=0; i<numstrings; i++)
		{
		    Region* region = vRegions[j];
		    const string& regname(region->get_name());
        	    if (!fnmatch(vRegionnames[i].c_str(), regname.c_str(), 0))
		    {
			const size_t regionId(region->get_id());
			if (m_reginfo!=0)
			{
			    CallpathType regtype((*m_reginfo)[regionId]);
			    if (regtype == COM)
			    {
                                if (regname == vRegionnames[i])
				    std::cerr << "Warning: Filter contains region \""
					  << regname << "\" of type "
					  << Callpathtype2String(regtype) << std::endl;
                                else
                                    std::cerr << "Warning: Filter \""
                                          << vRegionnames[i] << "\" matches region \""
                                          << regname << "\" of type "
					  << Callpathtype2String(regtype) << std::endl;
			    }
			    else if ((regtype == MPI) || (regtype == OMP)) {
                              std::cerr << "Warning: Filter matches region \""
                                        << regname << "\" of type " << Callpathtype2String(regtype)
                                        << " which cannot be filtered." << std::endl;
                              break;
                            }
                            else if (regtype == EPK) {
                              std::cerr << "Warning: Filter contains measurement-related region \""
                                        << regname << "\" which cannot be filtered."
                                        << std::endl;
                              break;
                            }
			}
		    
			vector<unsigned long>::iterator it;
                        it = lower_bound(vBlacklist.begin(), vBlacklist.end(), regionId);
                        if (*it != regionId)
			  vBlacklist.insert(it, regionId);

                        break;
		    }
		}
	    }
	    vBlacklist.push_back(ULONG_MAX);
	    good=true;
	}
    }
  
/**
* Initialize a blacklist with all USR regions.
*/
    void CBlacklist::initUSR()
    {
        const vector<Region*>& vRegions = m_cube->get_regv();
        const size_t rcnt = vRegions.size();
        vBlacklist.reserve(rcnt);
        for (size_t j=0; j<rcnt; j++)
	    {
                Region* region = vRegions[j];
		const size_t regionId(region->get_id());
		if (m_reginfo!=0) {
		    CallpathType regtype((*m_reginfo)[regionId]);
		    if (regtype == USR) {
			vector<unsigned long>::iterator it;
                        it = lower_bound(vBlacklist.begin(), vBlacklist.end(), regionId);
                        if (*it != regionId)
			  vBlacklist.insert(it, regionId);
		    }
                }
            }
        vBlacklist.push_back(ULONG_MAX);
        good=true;
    }

    bool CBlacklist::operator() (size_t regionId)
    {
      vector<unsigned long>::iterator it;
      it = lower_bound(vBlacklist.begin(), vBlacklist.end(), regionId);
      return (*it == regionId);
    }

    bool CBlacklist::operator() (std::string sName)
    {
	const vector<Region*>& vRegions(m_cube->get_regv());
	for (size_t i=0; i<vRegions.size(); i++)
	{
	    if (fnmatch(vRegions[i]->get_name().c_str(), sName.c_str(), 0))
	    {
		const size_t regionId (vRegions[i]->get_id());
		return (*this)(regionId);
	    }
	}
	return false;
    }

    bool CBlacklist::operator() (Region* pRegion)
    {
	const size_t regionId(pRegion->get_id());
	return (*this)(regionId);
    }

    size_t CBlacklist::operator[] (size_t i) const
    {
	return vBlacklist[i];
    }

    void CBlacklist::set_regioninfo(CRegionInfo* ri)
    {
	m_reginfo=ri;
    }
}
