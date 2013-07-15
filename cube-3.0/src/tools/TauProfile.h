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
* \file TauProfile.h
* \brief Provides a class to handle TAU trace format.
*/

#ifndef CUBE_TAUPROFILE_H
#define CUBE_TAUPROFILE_H   

/*
 *----------------------------------------------------------------------------
 *
 * class TauProfile
 *
 *----------------------------------------------------------------------------
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "services.h"

/**
* Defines a region of the code in TAU format.
*/

class TauRegion
{
 public:

  TauRegion(std::string name) : name(name) {}
  std::string get_name() const { return name; }

 private:

  std::string name;
};

typedef std::vector<const TauRegion*> TauCpath;///< TAU Calltree path.

/**
* Defines a meric in TAU format.
*/
class TauMetric
{
 public:

  TauMetric(std::string name, bool istime) : name(name), istime(istime) {}
  std::string get_name() const { return name; }
  bool is_time() const { return istime; }

 private:

  std::string name;
  bool istime;
};

/**
* Collectes a data (inclusive value, exclusive, nuber of calls).
*/
class TauData
{
 public:
  
  TauData(double incl, double excl, long ncalls) : incl(incl), excl(excl), ncalls(ncalls) {}
  double get_incl()   const { return incl; }
  double get_excl()   const { return excl; }
  long   get_ncalls() const { return ncalls; }

 private:

  double incl;
  double excl;
  long   ncalls;
}; 

/**
* Collects the information about the machine.
*/
class TauLoc
{
 public:

  TauLoc(long proc_id, long thrd_id) : proc_id(proc_id), thrd_id(thrd_id) {}
  long get_proc_id() const { return proc_id; }
  long get_thrd_id() const { return thrd_id; }

 private:
  
  long proc_id;
  long thrd_id;
};

/**
* Defines a TAU profile.
*/ 
class TauProfile 
{
 public:
  
  TauProfile(const char* path);
  ~TauProfile();

  long get_nmets() const { return metv.size(); }
  const TauMetric* get_met(long met_id) const { return metv[met_id]; }

  long get_nregions() const { return regionv.size(); }
  const TauRegion* get_region(long reg_id) const { return regionv[reg_id]; }
  
  long get_nlocs() const { return locv.size(); }
  const TauLoc* get_loc(long loc_id) const { return locv[loc_id]; }

  long get_ncpaths() const { return cpathv.size(); }
  const TauCpath* get_cpath(long cpath_id) const { return cpathv[cpath_id]; }

  const TauData*  get_data(const TauMetric* met, const TauCpath* cpath, const TauLoc* loc) const; 

  private:

  /// parse single-location TAU profile file
  void parse_single_tau_file(const char* path);

  /// check whether metric already exists
  const TauMetric* get_org_met(TauMetric* met);
  /// check whether call path already exists
  const TauCpath* get_org_cpath(TauCpath* cpath);
  /// check whether loc already exists
  const TauLoc* get_org_loc(TauLoc* loc);
  
  std::vector<TauRegion*>            regionv;
  std::vector<TauCpath*>             cpathv;
  std::vector<TauMetric*>            metv;
  std::vector<TauLoc*>               locv;
  std::vector<TauData*>              datav;

  std::map<const TauMetric*, std::map<const TauCpath*, std::map<const TauLoc*, TauData*> > > profilem;///< TAU profile (three dimensions)
  std::map<std::string, TauRegion*>  name2region;
};


bool operator==(const TauMetric& lhs, const TauMetric& rhs);
bool operator!=(const TauMetric& lhs, const TauMetric& rhs);
bool operator==(const TauRegion& lhs, const TauRegion& rhs);
bool operator!=(const TauRegion& lhs, const TauRegion& rhs);
std::ostream& operator<<(std::ostream& out, const TauRegion& rhs);
bool operator==(const TauCpath& lhs, const TauCpath& rhs);
std::ostream& operator<<(std::ostream& out, const TauCpath& rhs);
bool operator==(const TauLoc& lhs, const TauLoc& rhs);



#endif






