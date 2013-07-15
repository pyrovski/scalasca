/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_EXPERTOPT_H
#define EXP_EXPERTOPT_H

#include <cstdlib>
#include <string>
 
class ExpertOpt
{
public:
  ExpertOpt(int argc, char* argv[]);
  int get_verbose() const { return verbose; }
  int get_patterntrace() const { return patterntrace; }
  bool get_gatherStatistic() const { return gatherStatistic; } 
  bool get_selectPatterns() const { return selectPatterns; }
  std::string get_filename() const { return filename; }
  const std::string& get_selectPatternFilename()
                                 { return selectPatternFilename; }
  std::string get_prefix() const { return prefix_p + prefix_s; }
   
  enum {PT_NONE=0x00, PT_PATTERN=0x01, PT_SEVERITY=0x02, PT_ALL=0x03}; 

  static int env_int (std::string envvar, int def) {
     char* tmp = std::getenv(envvar.c_str());
     if (tmp == NULL) return def;
     return std::atoi(tmp);
  }
  static bool env_bool (std::string envvar, bool def) {
    return (env_int(envvar, def) != 0);
  }

private:
  int verbose;
  int patterntrace;
  bool gatherStatistic;
  bool selectPatterns;
  std::string selectPatternFilename;
  std::string filename;
  std::string prefix_p;
  std::string prefix_s;
};

#endif
