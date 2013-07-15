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
* \file services.cpp
* \brief Provides an implementation of the service functions for cube library.

*/
#ifndef CUBE_SERVICES_CPP
#define CUBE_SERVICES_CPP 0
#include <iostream>
#include <string>
#include <list>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include "services.h"



/**
* Performs the actual replacing (escaping).
*
* Replacement happens in two steps
* 1. First the list of the positions of parts, which have to be replaced, is created.
* 2. Then replacement only at saved positions is performed.
*
* Note, that in the case  "String -> XML", first ampersands are replaced, then all another symbols.
* in the case "XML ->String", first all another symbols, and then ampersand.
* It removes possible infinite loop with ampersands.
*********************************/
std::string replaceSymbols(unsigned from, unsigned to, std::string  str, ReplaceDirection direction)
{

    static const unsigned MapSize = 5;
    static std::string ReplaceMap[MapSize][2] = {  {"&", "&amp;"}, { "<", "&lt;"}, {">", "&gt;" }, {"'", "&apos;"}, {"\"", "&quot;"} };

    std::string::size_type pos;
    int Start = 0;
    int Condition = MapSize;
    int increment = 1;
    if (direction == SERVICES_REPLACE_BACKWARD)
    {
        Start = MapSize - 1 ;
        Condition = -1;
        increment = -1;
    }

    for(int i=Start; i!=Condition; i=i+(increment))
    {
        std::list<unsigned> PositionsToReplace;
        PositionsToReplace.clear();

        pos = str.find(ReplaceMap[i][from].c_str());
        while(pos != std::string::npos)
        {
            PositionsToReplace.push_back(pos);
            pos = str.find(ReplaceMap[i][from].c_str(), pos+ReplaceMap[i][from].size());
        }

        unsigned n_replace= 0;
        for(std::list<unsigned>::iterator iter = PositionsToReplace.begin();   iter != PositionsToReplace.end(); iter++, n_replace++)
        {
            str.replace (*iter + n_replace * ((int)ReplaceMap[i][to].size()  - (int)ReplaceMap[i][from].size()), ReplaceMap[i][from].size(),  ReplaceMap[i][to].c_str());
        }
    }
    return str;
 }



/**
*  Replaces all symbols like "<" or ">" with corresponding HTML equivalents "&lt;", "&gt;"
*********************************/
std::string services::escapeToXML(std::string  str)
{
    return replaceSymbols(0, 1, str, SERVICES_REPLACE_FORWARD);
 }

/**
*  Replaces all symbols like "&gl;" or "&lt;" with corresponding HTML equivalents ">", "<"
*********************************/
std::string services::escapeFromXML(std::string   str)
{
    return replaceSymbols(1, 0 , str, SERVICES_REPLACE_BACKWARD);

 }

/**
*  Checks the access permissions for a file, whether it is a directory or file,
* whether the path is searchable and reports an error in the positive case.
* It helps to verbalize the error messages in the case of "wrong file".
*********************************/
int services::check_file(const char * filename)
{
    struct stat buffer;
    int         status;
    errno = 0;
    status = stat(filename, &buffer);
    int error = errno;

    if (error == ENOENT)
    {
        std::cerr << std::endl << "File \"" << filename << "\" doesn't exist."  << std::endl;
        return -1;
    };
    if (error == EACCES)
    {
        std::cerr << std::endl << "Cannot access  file \"" << filename << "\". Path is not searchable."  << std::endl;
        return -1;
    };
    if (S_ISDIR(buffer.st_mode))
    {
        std::cerr << std::endl << "\"" << filename << "\" is a directory and not a cube file." << std::endl;
        return -1;
    };

      if (
            (buffer.st_mode & S_IRUSR) == 0  &&
            geteuid() == buffer.st_uid
            )
          {
                 std::cerr << std::endl << "Insufficient user permission to read  \"" << filename  << "\"." << std::endl;
                 return -1;
           } else
             if (
                   geteuid() != buffer.st_uid
                 )
                {
                    // std::cerr << std::endl << "Not owner of  \"" << filename  << "\"." << std::endl;
                    if (
                            (buffer.st_mode & S_IRGRP) == 0  &&
                            getgid() == buffer.st_gid
                            )
                        {
                            std::cerr << std::endl << "Insufficient group permission to read \"" << filename  << "\"." << std::endl;
                            return -1;
                        } else
                            if (
                                getgid() != buffer.st_gid
                                )
                                {
                                // std::cerr << std::endl << "Not in group of  \"" << filename  << "\"." << std::endl;
                                if (
                                    (buffer.st_mode & S_IROTH) == 0.
                                    )
                                    {
                                        std::cerr << std::endl << "Insufficient others permission to read \"" << filename  << "\"." << std::endl;
                                        return -1;
                                      }
                                }
                }

    if (status==0) return 0;

    return -1;
}


#endif
