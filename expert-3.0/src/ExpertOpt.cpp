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

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "epk_archive.h"

#include "ExpertOpt.h"
#include "Error.h"

using namespace earl;
using namespace std;

ExpertOpt::ExpertOpt(int argc, char* argv[] ) : 
    gatherStatistic(false), selectPatterns(false), selectPatternFilename(""),
    filename(""), prefix_p(""), prefix_s("") 
{
    int ch;
    bool quiet   = false;
    patterntrace = PT_NONE;
    verbose      = env_int("EPT_VERBOSE",0);
    
    const string USAGE ="\n"
    "Usage: " + string(argv[0]) + " [-v] [-s] [-q] [-p] [-h] <filename>\n"
    "  -h         Help; Output a brief help message.\n" 
    "  -p         PatternTrace; Generates pattern events.\n"
    "  -q         Quiet mode; do  not  write  anything  to  standard  output.\n"
    "             Ignore -v option and EPT_VERBOSE environment variable.\n" 
    "  -s         PatternTrace; Generates severity events.\n"
    "  -v         Verbose mode; print debugging messages about its progress.\n" 
    "             Multiple -v options increase the verbosity. \n"
    "  -%         Statistics; Print statistical information about patterns found.\n"
    "  -l <file>  SelectPatterns; Only patterns listed in <file> are considered\n"
    "             for the options -%, -s and -p. The pattern names can be followed by\n"
    "             a value indicating the minimum severity for the pattern to be considered.\n"
    "             Each pattern name [plus optional value] has to be on a new line.\n"
    "\n"
    "Environment:\n"
    "  EPT_INCOMPLETE_COMPUTATION:  valid values [0|1]\n"
    "                               0 - reject incomplete computation (default).\n"
    "                               1 - accept incomplete computation.\n"
    "  EPT_MEASURED_METRICS:        valid values [0|1|2]\n"
    "                               0 - don't list unparented measured metrics\n"
    "                                   separately (i.e., ignore them).\n"
    "                               1 - list unparented measured metrics\n"
    "                                   separately (default).\n"
    "                               2 - list all measured metrics (parented\n"
    "                                   and unparented) separately.\n"
    "  EPT_VERBOSE:                 default value for verbosity level (-v Option)\n"
    ;

    while ((ch = getopt(argc, argv, "vsqp%l:h?")) != EOF)
    {
        switch (ch)
        {
        case 'v':
            verbose++;
            break;
        case 's':
            patterntrace|=PT_SEVERITY;
            prefix_s = "s";
            break;
        case 'q':
            quiet=true;
            break;
        case 'p':
            patterntrace|=PT_PATTERN;
            prefix_p = "p";
            break;
        case '%':
           gatherStatistic = true;
           break;
        case 'l':
           selectPatterns = true;
           selectPatternFilename = optarg;
           break;
        case 'h':
        case '?':
           cerr << USAGE << endl; 
           exit(EXIT_SUCCESS);
        default:
          throw FatalError( USAGE + "\nError: Wrong arguments.");
        }
    }
    if ( quiet )
      verbose=-1;
   
    if (optind < argc) 
      {
      filename = string(argv[optind]);
      }
    else
      {
      throw FatalError( USAGE + "\nError: File name required.");
      }

    // check for epik experiment archive
    bool epik = (filename.rfind("epik_")!=string::npos);
    const string epik_fn = "/epik.elg";
    if (epik && ((filename.length() <= epik_fn.length()) ||
            (filename.substr(filename.length()-epik_fn.length())!=epik_fn))) {
        // verify archive
        if (epk_archive_exists(filename.c_str()))
            epk_archive_set_name(filename.c_str());
            
        // extend path to epik experiment archive with actual filename
        filename += epik_fn;

        // check file exists
        ifstream elgfile(filename.c_str());
        if (elgfile) elgfile.close();
        else { // no merged trace!
            cerr << epk_archive_get_name() << epik_fn << " not found!" << endl;
            // check for ELG subdirectory and run elg_merge
            string elgdirname = epk_archive_directory(EPK_TYPE_ELG);
            ifstream elgdir(elgdirname.c_str());
            if (elgdir) {
                elgdir.close();
                string mergecmd = string("elg_merge ") + epk_archive_get_name();
#ifndef DO_IT_YOURSELF
                cerr << "Run: " << mergecmd << endl; exit(1);
#else
                if (verbose) cout << mergecmd << endl;
                if (system(mergecmd.c_str()) != 0)
                    perror("elg_merge");                
#endif
            } else {
                cerr << "Can't analyze anything in " << epk_archive_get_name() << " directory." << endl;
                exit(1);
            }
        }
    }
}
