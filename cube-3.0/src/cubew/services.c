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
* \brief Provides an implementation of the service functions.

*/
#ifndef CUBE_SERVICES_CPP
#define CUBE_SERVICES_CPP 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "services.h"


// enum ReplaceDirection { SERVICES_REPLACE_FORWARD, SERVICES_REPLACE_BACKWARD};



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
// // char * __replace_symbols(unsigned from, unsigned to, char *  str, enum ReplaceDirection direction)
// // {
// //
// //     static const unsigned MapSize = 5;
// //     static char * ReplaceMap[MapSize][2] = {  {"&", "&amp;"}, { "<", "&lt;"}, {">", "&gt;" }, {"'", "&apos;"}, {"\"", "&quot;"} };
// //     static int ReplaceMapSizes[MapSize][2] = {  {1, 5}, { 1, 4}, {1, 4 }, {1, 6}, {1, 6} };
// //     static const unsigned MaxSizeInMap = 6; /* size of &quot; */
// //
// //
// //     unsigned pos;
// //     unsigned strlength = strlen(str);
// //     char * result = (char *)malloc(  strlength * MaxSizeInMap ); /* MaxSizeInMap is maximal length of target symbol. &apos;*/
// //     memset(result, 0, strlength * MaxSizeInMap);
// //     memcpy(result, str,  strlength);
// //     unsigned actuallength = strlength;
// //     int Start = 0;
// //     int Condition = MapSize;
// //     int increment = 1;
// //     if (direction == SERVICES_REPLACE_BACKWARD)
// //     {
// //         Start = MapSize - 1 ;
// //         Condition = -1;
// //         increment = -1;
// //     }
// //     int i=Start;
// //     for(i=Start; i!=Condition; i=i+(increment))
// //     {
// //         int *  PositionsToReplace = (int *)malloc( strlength * sizeof(int)) ; /*  maximal amount of symbols to be replace == number of symbols */
// //         unsigned index_to_rep =0;
// //         for (index_to_rep=0;  index_to_rep < strlength; index_to_rep++) PositionsToReplace[index_to_rep] = -1;      /* set all elements on -1 */
// //         index_to_rep=0;
// // //         pos = str.find(ReplaceMap[i][from].c_str());
// //         char * _pos = NULL; /* used to mark the found place */
// //         _pos = strstr(result, ReplaceMap[i][from]);
// //         while(_pos != NULL)
// //         {
// //             PositionsToReplace[index_to_rep] = (int)(_pos - result);
// //             _pos = strstr(_pos, ReplaceMap[i][from]);
// //             index_to_rep++;
// //         }
// //
// //         unsigned n_replace= 0;
// //         for(n_replace= 0; n_replace < index_to_rep; n_replace++)
// //         {
// // //             str.replace (*iter + n_replace * ((int)ReplaceMap[i][to].size()  - (int)ReplaceMap[i][from].size()), ReplaceMap[i][from].size(),  ReplaceMap[i][to].c_str());
// //
// //             /* REplacement in two steps
// //                 1. Moving memory after place to replace via memmove (coz of overlappyng regions)
// //                 2. Memcpy on empty space
// //             */
// //             char * _pos_of_source =     result +
// //                                         PositionsToReplace[n_replace]  +
// //                                         n_replace * (ReplaceMapSizes[i][to] - ReplaceMapSizes[i][from]);
// //             char * _pos_of_distination = result +
// //                                          PositionsToReplace[n_replace] +
// //                                          n_replace *  (ReplaceMapSizes[i][to] - ReplaceMapSizes[i][from]) +
// //                                          ReplaceMapSizes[i][to];      /*  distanation is shifted by size differences before and size of target replacement string*/
// //             int move_size = (result + actuallength ) - _pos_of_source;
// //             memmove(_pos_of_source, _pos_of_distination, move_size);
// //             memcpy(_pos_of_source, ReplaceMap[i][to], ReplaceMapSizes[i][to]);
// //             actuallength = actuallength + (ReplaceMapSizes[i][to] - ReplaceMapSizes[i][from]);
// //         }
// //         free(PositionsToReplace);
// //     }
// //     return str;
// //  }
// //
// //
// //


char * __replace_symbols2(char * str)
{
  char* r = NULL;
  char* s = str;
  int len = strlen(s);
  unsigned toks=0;
  char* p=s;
  /* determine number of tokens to convert */
  while (p <= s+len) {
    size_t n=strcspn(p, "<>&\"'"); /* strcspn(p, "<>&\'\""); */
    p+=(n+1);
    toks++;
  }
  if (toks) r=(char *)calloc(len+(toks*6),1); /* allocate sufficiently long string */
  p=s;
  /* replace each token in new string */
  if (r) while (p <= s+len) {
    size_t n=strcspn(p, "<>&\"'"); /* strcspn(p, "<>&\'\""); */
    strncat(r, p, n);
    p+=n;
    switch(*p) {
      case '<': strcat(r,"&lt;"); break;
      case '>': strcat(r,"&gt;"); break;
      case '&': strcat(r,"&amp;"); break;
      case '\'': strcat(r,"&apos;"); break;
      case '\"': strcat(r,"&quot;"); break;
      default : break;
    }
    p++;
  }

  return (char *)r;

}


/**
*  Replaces all symbols like "<" or ">" with corresponding HTML equivalents "&lt;", "&gt;"
*********************************/
char * services_escape_to_xml(char * str)
{
    if (str == NULL) return NULL;
    return __replace_symbols2(str);
 }

/**
*  Replaces all symbols like "&gl;" or "&lt;" with corresponding HTML equivalents ">", "<"
*********************************/
// // char * services_escape_from_xml(char *   str)
// // {
// //     return __replace_symbols(1, 0 , str, SERVICES_REPLACE_BACKWARD);
// //
// //  }





/*  Checks the acces permissions for a file, whether it is a director or file, whether the path is searchable and reports an error in the positive case. It helps to verbalize the error messages in the case of "wrong file"
*********************************/
int services_check_file(const char * filename)
{
    struct stat buffer;
    int         status;
    status = stat(filename, &buffer);
    int error = errno;
    fprintf(stderr, "\n");
    if (error == ENOENT)
    {
    	fprintf(stderr, "File \"%s\" doen't exist.", filename);
        return -1;
    };
    if (error == EACCES)
    {
    	fprintf(stderr, "Cannot acces the file \"%s\". Path is not searchable.", filename);
        return -1;
    };
    if (S_ISDIR(buffer.st_mode))
    {
    	fprintf(stderr, "\"%s\" is a directory and not a cube file.", filename);
        return -1;
    };


      if (
            (buffer.st_mode & S_IRUSR) == 0  &&
            geteuid() == buffer.st_uid
            )
          {
		        fprintf(stderr, "No sufficient permission for a user to read  \"%s\".", filename);
                 return -1;
           } else
             if (
                   geteuid() != buffer.st_uid
                 )
                {
		            fprintf(stderr, "Not an owner of  \"%s\".", filename);
                    if (
                            (buffer.st_mode & S_IRGRP) == 0  &&
                            getgid() == buffer.st_gid
                            )
                        {
                            fprintf(stderr, "No sufficient permission for a group to read \"%s\".", filename);
                            return -1;
                        } else
                            if (
                                getgid() != buffer.st_gid
                                )
                                {
                                fprintf(stderr, "Not in group of  \"%s\".", filename);
                                if (
                                    (buffer.st_mode & S_IROTH) == 0
                                    )
                                    {
                                        fprintf(stderr, "No sufficient permission for others to read \"%s\".", filename);
                                        return -1;
                                      }
                                }
                }

    if (status==0) return 0;
    return -1;
}


/**
 * Provides a missing on some platforms functionality of "strdup".
 *
 */
char*
cubew_strdup( const char* str )
{
  if (str == NULL) return NULL;
  char* new_str = calloc( strlen( str ) + 1, 1 );
  strcpy( new_str, str );
  return new_str;
}

#endif
