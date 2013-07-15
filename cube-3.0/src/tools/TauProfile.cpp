/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2004-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file TauProfile.cpp
* \brief Defines a class to handle TAU trace format.
*/

#include "TauProfile.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#define MAX_LINE_LENGTH (128 * 1024)

using namespace std;
/**
* Defines a line in TAU profile.
*/
struct TauLine
{
  vector<string> callv;
  long   calls;
  long   subr;
  double excl;
  double incl;
};  

/// @cond PROTOTYPES
void get_tau_file_names(string path, vector<string>& out);
/// @endcond

TauProfile::TauProfile(const char* path)
{
  // get names of all single location profiles
  vector<string> file_namev;
  get_tau_file_names(path, file_namev);
  if ( file_namev.empty() )
    {
      cerr << endl << "ERROR: No TAU profile files found at \"" << path << "\"." << endl;
      exit(EXIT_FAILURE);
    }
  
  // parse each of them
  for ( size_t i = 0; i < file_namev.size(); i++ ) 
    {
      cerr << file_namev[i] << endl;
     if (services::check_file(file_namev[i].c_str()) != 0 ) exit(-1);
      parse_single_tau_file(file_namev[i].c_str());
    }
}    


TauProfile::~TauProfile()
{
  for ( size_t i = 0; i < this->regionv.size(); i++ ) delete this->regionv[i];
  for ( size_t i = 0; i < this->cpathv.size(); i++ )  delete this->cpathv[i];
  for ( size_t i = 0; i < this->metv.size(); i++ )    delete this->metv[i];
  for ( size_t i = 0; i < this->locv.size(); i++ )    delete this->locv[i];
  for ( size_t i = 0; i < this->datav.size(); i++ )   delete this->datav[i];
}

/**
* Returns data to triplet (metric, callpath, location) as a step-by-step searching in mapping.
*/
const TauData*  
TauProfile::get_data(const TauMetric* met, const TauCpath* cpath, const TauLoc* loc) const
{
  map<const TauMetric*, map<const TauCpath*, map<const TauLoc*, TauData*> > >::const_iterator mit;
  map<const TauCpath*, map<const TauLoc*, TauData*> >::const_iterator cit;
  map<const TauLoc*, TauData*>::const_iterator lit;
  
  if ( ( mit = profilem.find(met) ) == profilem.end() )
    {
      cerr << endl << "ERROR: No such metric." << endl;
      exit(EXIT_FAILURE);
    }
  if ( ( cit = mit->second.find(cpath) ) == mit->second.end() )
    {
      cerr << endl << "ERROR: No such call path." << endl;
      exit(EXIT_FAILURE);
    }
  if ( ( lit = cit->second.find(loc) ) == cit->second.end() )
    {
      // cerr << "No such location." << endl;
      return NULL;
    }
  else
    return lit->second;
} 

/**
* Parses a TAU file and fills the object with a data.
*/
void 
TauProfile::parse_single_tau_file(const char* path)
{
  ifstream file(path);
  if ( !file )
    {
      cerr << endl << "ERROR: File \"" << path << "\" not found." << endl;
      exit(EXIT_FAILURE);
    }

  // identify proc_id and thrd_id. We assume that this is the TAU node
  // and the TAU thread, respectively.

  string path_str(path);
  path_str = path_str.substr(path_str.rfind("/profile.") + string("/profile.").size());
  long tau_node, tau_context, tau_thread;
  sscanf(path_str.c_str(), "%ld.%ld.%ld", &tau_node, &tau_context, &tau_thread);
			     
  TauLoc* loc = new TauLoc(tau_node, tau_thread);
  const TauLoc* org_loc = get_org_loc(loc);
  
  // read number of lines and metric string
  char buf[MAX_LINE_LENGTH];
  int nlines;
  string met_str, met_name;
  file.getline(buf, MAX_LINE_LENGTH);
  istringstream met_istr(buf);
  met_istr >> nlines >> met_str;
  
  // identify metric name
  if ( met_str == "templated_functions" )
    met_name = "TIME";
  else
    met_name = met_str.substr(string("templated_functions_MULTI_").size());
  
  // check whether metric refers to time
  bool is_time = false;
  if ( met_name == "TIME" or met_name == "GET_TIME_OF_DAY" )
    is_time = true;

  TauMetric* met = new TauMetric(met_name, is_time);
  const TauMetric* org_met = get_org_met(met);

  // check whether profile contains statistics
  bool has_stats = false; 
  file.getline(buf, MAX_LINE_LENGTH);
  if ( string(buf).find("SumExclSqr") != string::npos )
    has_stats = true;

  // then read the specified number of lines
  vector<TauLine> linev;
  for ( int i = 0; i < nlines; i++ ) 
    {
      file.getline(buf, MAX_LINE_LENGTH);

      TauLine tau_line;
      string cpath = strtok(buf, "\"");
      char* data = strtok(NULL, "\"");
      
      istringstream istr(data);
      istr >> tau_line.calls >> tau_line.subr >> tau_line.excl >> tau_line.incl; 

      // skip statistics if necessary
      if ( has_stats)
	{
	  int num_stat_lines, dummy;
	  istr >> dummy >> num_stat_lines;
	  for ( int j = 0; j < num_stat_lines; j++ )
	    file.getline(buf, MAX_LINE_LENGTH);
	}


      // locate all " => " delimiters in cpath and extract individual
      // region names
      
      string::size_type pos = 0;
      string rest(cpath, 0, cpath.size());
      string region;
      
      while ( true )
	{
	  pos = rest.find(" => ");
	  if ( pos == string::npos )
	    {
	      region = rest;
	      // eliminate space at the end
	      while ( region[region.size() - 1] == ' ' )
		region = region.substr(0, region.size() - 1);
	      tau_line.callv.push_back(region);
	      break;
	    }
	  else
	  {
	      region = rest.substr(0, pos);
	      // eliminate space at the end
	      while ( region[region.size() - 1] == ' ' )
		region = region.substr(0, region.size() - 1);
	      rest = rest.substr(pos + 4);
	      tau_line.callv.push_back(region);
	    }
	}
      linev.push_back(tau_line);
    }

  // extend region and call path table
  for ( size_t i = 0; i < linev.size(); i++ )
    {
      TauCpath* cpath = new TauCpath(); 
      for ( size_t j = 0; j < linev[i].callv.size(); j++ )
	{
	  string region_name = linev[i].callv[j];
	  if ( this->name2region.find(region_name) == this->name2region.end() )
	    {
	      // create new region object
	      TauRegion* region = new TauRegion(region_name);
	      this->regionv.push_back(region);
	      name2region[region_name] = region;
	    }
	  cpath->push_back(name2region[region_name]);
	}
      if ( cpath->empty() )
	{
	  cerr << endl << "ERROR: Empty call path." << endl;
	  exit(EXIT_FAILURE);
	}
      // check whether call path already exists
      const TauCpath* org_cpath = this->get_org_cpath(cpath);
      
      // create data object for this call path
      TauData* data = new TauData(linev[i].incl, linev[i].excl, linev[i].calls);
  
      // enter data into profile map
      this->profilem[org_met][org_cpath][org_loc] = data;
    }
}


const TauMetric*
TauProfile::get_org_met(TauMetric* met)
{
  for ( size_t i = 0; i < this->metv.size(); i++ )
    if ( *this->metv[i] == *met )
      {
	// dispose the fake
	delete met;
	return this->metv[i];
      }
  this->metv.push_back(met);
  return met;
}

const TauCpath*
TauProfile::get_org_cpath(TauCpath* cpath)
{
  for ( size_t i = 0; i < this->cpathv.size(); i++ )
    if ( *this->cpathv[i] == *cpath )
      {
	// dispose the fake
	delete cpath;
	return this->cpathv[i];
      }
  this->cpathv.push_back(cpath);
  return cpath;
}

const TauLoc*
TauProfile::get_org_loc(TauLoc* loc)
{
  for ( size_t i = 0; i < this->locv.size(); i++ )
    if ( *this->locv[i] == *loc )
      {
	// dispose the fake
	delete loc;
	return this->locv[i];
      }
  this->locv.push_back(loc);
  return loc;
}

bool operator==(const TauMetric& lhs, const TauMetric& rhs)
{
  return lhs.get_name() == rhs.get_name();
}

bool operator!=(const TauMetric& lhs, const TauMetric& rhs)
{
  return lhs.get_name() != rhs.get_name();
}

bool operator==(const TauRegion& lhs, const TauRegion& rhs)
{
  return lhs.get_name() == rhs.get_name();
}

bool operator!=(const TauRegion& lhs, const TauRegion& rhs)
{
  return lhs.get_name() != rhs.get_name();
}

ostream& operator<<(ostream& out, const TauRegion& rhs)
{
  out << rhs.get_name();
  return out;
} 

bool operator==(const TauCpath& lhs, const TauCpath& rhs)
{
  if ( lhs.size() != rhs.size() )
    return false;
  else
    for ( size_t i = 0; i < lhs.size(); i++ )
      if ( *lhs[i] != *rhs[i] )
	return false;
  
  return true;
}

bool operator==(const TauLoc& lhs, const TauLoc& rhs)
{
  return ( lhs.get_proc_id() == rhs.get_proc_id() && lhs.get_thrd_id() == rhs.get_thrd_id() );	  
}
/**
* Prints TAU call path as "path->path->path->.."
*/
ostream& operator<<(ostream& out, const TauCpath& rhs)
{
  for ( size_t i = 0; i < rhs.size(); i++ )
    {
      out << rhs[i]->get_name();
      if ( i != rhs.size() - 1 )
	out << " -> ";
    }
  return out;
} 

/**
* Checkt the directory "path" and returns all "profile." files
*/
void get_tau_file_names(string path, vector<string>& out)
{
  string cmd = string("find ") + path + " -name 'profile.*'"; 
  FILE* file = popen(cmd.c_str(), "r");

  char buf[MAX_LINE_LENGTH];

  while ( fscanf(file, "%s", buf) != EOF ) 
    out.push_back(buf);
  
  pclose(file);
}


