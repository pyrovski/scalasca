#!/usr/bin/gawk -f
##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

#
# pomp2_parse_init_regions.awk
#
# Expects the output of
# $(NM) $ALL_OBJS_AND_LIBS | $(GREP) -i POMP2_Init_reg | $(GREP) " [TD] "
# as input. See <prefix/share/opari/doc/example/Makefile> for a working
# example.
#
# The output is C-code that needs to be linked to your application. It
# provides several functions:
#
# void POMP2_Init_regions(): need to be called from your POMP2 library to
# initialize all instrumented POMP2 region by calling the instrumented
# functions POMP2_Init_reg_*.
#
# size_t POMP2_Get_num_regions() returns the number of POMP2 regions found
# in $ALL_OBJS_AND_LIBS.
#
# const char* POMP2_Get_opari2_version() returns a version string.
#
# Three functions returning int and specifying the library interface version:
# int POMP2_Get_required_pomp2_library_version_(current|revision|age)()
# 
# Author: Christian Roessel <c.roessel@fz-juelich.de>
#

/_[0-9a-z][0-9a-z]*_[0-9][0-9]*_?_?$/ {
  if (tolower($0) ~ /pomp2_init_reg/ && $0 ~ /[TDA]/ )
  {  
    for (i = 1; i <= NF; i++)
    {
      if (index($i,"POMP2_Init_reg") != 0)
      {
        separator = "POMP2_Init_reg";
      }
      else if (index($i,"pomp2_init_reg") != 0)
      {
        separator = "pomp2_init_reg";
      }
      else if (index($i,"POMP2_INIT_REG") != 0)
      {
        separator = "POMP2_INIT_REG";
      }
      else
      {
        continue;
      }

      # $i looks like "POMP2_Init_reg_uniqueId_nRegions" or
      # like "pomp2_init_reg_uniqueId_nRegions" or
      # like "POMP2_INIT_REG_uniqueId_nRegions"
      split ($i,splitResult,separator);
      _uniqueId_nRegions = splitResult[2];
      if (!(_uniqueId_nRegions in regions))
      {
        regions[_uniqueId_nRegions] = $i;
        split(_uniqueId_nRegions, tokens, "_");
        nRegions += tokens[3];
      }
    }
  }
}
  
END{ 
  print tmp "\n"
  print "#ifdef __cplusplus"
  print "extern \"C\""
  print "{"
  print "#endif"

  print "#include <stddef.h>\n"

  # cut away leading full-stops
  for (i in regions)
  {
    sub(/^\./, "", regions[i]);
  }

  # declare POMP2_Init_reg_* functions extern
  for (i in regions)
  {
    print "extern void " regions[i] "();";
  }

  # define POMP2_Init_regions() and call all POMP2_Init_reg_* functions
  print "\nvoid POMP2_Init_regions()"
  print "{"
  for (i in regions)
  {
    print "    " regions[i] "();";
  }
  print "}\n"

  # define function POMP2_Get_num_regions()
  print "size_t POMP2_Get_num_regions()"
  print "{"
  if (nRegions != 0)
  {
      print "    return " nRegions ";"
  } else {
      print "    return 0;"
  }
  print "}\n"

  # define function POMP2_Get_opari2_version()
  print "const char* POMP2_Get_opari2_version()"
  print "{"
  print "    return \"1.0.7\";"
  print "}\n"

  print "/* "
  print " * The following functions define the POMP2 library interface version"
  print " * the instrumented code conforms with. The library interface version"
  print " * is modeled after"
  print " * https://www.gnu.org/software/libtool/manual/libtool.html#Versioning"
  print " */\n"
  # define function POMP2_Get_required_pomp2_library_version_current()
  print "int POMP2_Get_required_pomp2_library_version_current()"
  print "{"
  print "    return 1;"
  print "}\n"  

  # define function POMP2_Get_required_pomp2_library_version_revision()
  print "int POMP2_Get_required_pomp2_library_version_revision()"
  print "{"
  print "    return 0;"
  print "}\n"

  # define function POMP2_Get_required_pomp2_library_version_age()
  print "int POMP2_Get_required_pomp2_library_version_age()"
  print "{"
  print "    return 0;"
  print "}\n"
  
  print "#ifdef __cplusplus"
  print "}"
  print "#endif"
}  
