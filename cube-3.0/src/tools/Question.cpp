/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <algorithm>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <limits>
#include <sstream>
#include <vector>
#include <iterator>
#include <stdexcept>
#include <cctype>
char comment='#';

struct upper {
  int operator()(int c)
  {
    return std::toupper((unsigned char)c);
  }
};

std::string intToStr(int number)
{
  /** 
   * Converts an int into a std::string. itoa uses c-style strings, and boost creates unnecessary dependency
   * @param the integer to be converted into a std::string
   */
  std::ostringstream ss;
  if (!(ss << number)) throw std::logic_error("IntToStr failed");
  return ss.str();
}

unsigned int strToUInt(std::string * s)
{
  /** 
   * Converts a std::string into an unsigned int. strtoul c-style strings, and boost creates unnecessary dependency
   * @param the input string to be converted.
   */
  std::stringstream ss(*s);
  unsigned int number;
  ss >> number;
  return(number);
}

bool isNumber(std::string * s) 
{
  int myNumber;
  std::stringstream ss(*s);
   if (ss >> myNumber)
     return(true); else return(false);
}

bool isComment(std::string * s) {
  /** 
   * Verifies if string has the "comment" char at the beginning.
   * @param the input string to be verified.
   */  
  if((*s).size()==0) return true;
  if((char)s->at(0)==comment) return true; else return false;
}


void question(std::string q, std::string * a) 
{
  /**
   * Generic function for question/answer. Will be overloaded by different methods according to the need (with an index, for example).
   * @param[in] q a string containing the question
   * @param[in,out] a a pointer to a string which will contain the answer
   */
  do{
    std::cout << q << std::endl; 
    std::cin.clear();
    std::cin.sync(); 
    getline(std::cin,*a);
    } while(isComment(a));
}



void question(int i, std::string q, std::string * a){
  /**
   * specialization of the question. Takes an integer which will be used in the question. The question needs to have a %i where this integer will be placed
   * note: if a std::cin was used before, it is quite probable that you need a getline(cin, string) in your code that will not do anything.
   * @param[in] i an integer that will be used in this question - for instance, in a loop
   * @param[in] q a string with the question
   * @param[in,out] a pointer to a string,int which will contain the answer
   */
  std::string::size_type pos=q.find("%i");
  if(pos!=std::string::npos) {
    q.replace(pos,2,intToStr(i));
  } 
  do{
    std::cout << q << std::endl;
    std::string test;
    std::cin.clear(); //clear the error bits for the cin input stream
    std::cin.sync(); //synchronize the input buffer, discarding any leftover characters in the buffer 
    getline(std::cin,*a);
  } while(isComment(a));
}



void question(int i, std::string q, size_t * a){
  /**
   * specialization of the question. Takes an integer which will be used in the question and answers to a size_t.
   * The question needs to have a %i where this integer will be placed
   * @param[in] i an integer that will be used in this question - for instance, in a loop
   * @param[in] q a string with the question
   * @param[in,out] a pointer to a size_t which will contain the answer
   */


  size_t pos=q.find("%i");
  if(pos!=std::string::npos) {
    q.replace(pos,2,intToStr(i));
  }
  std::string s=""; // isComment expects a string. cin might give the expected integer or a string comment.
  do {
    std::cout << q << std::endl;
    std::cin.clear(); //clear the error bits for the cin input stream
    std::cin.sync(); //synchronize the input buffer, discarding any leftover characters in the buffer 
    getline(std::cin,s);
  } while(isComment(&s)&&(s!=""));
  *a=std::atol(s.c_str()); // Every time a c string appears in c++ code, a kitten dies.
}

void question(std::string q, unsigned int * a){
  /**
   * specialization of the question. Returns an integer.
   * @param[in] q a string with the question
   * @param[in,out] a integer to be returned
   */ 
  std::string s;  // isComment expects a string. cin might be the expected integer or a comment string
  do {
    std::cout << q << std::endl;
    std::cin.clear(); //clear the error bits for the cin input stream
    std::cin.sync(); //synchronize the input buffer, discarding any leftover characters in the buffer
    getline(std::cin,s);
  } while(isComment(&s)||!isNumber(&s)); // isComment here is moot, as isNumber will always return false in a text string.
  *a=strToUInt(&s);

}

void question(std::string q, bool * a) {
  /**
   * this specialization of a question returns a bool, and keeps asking while the answer is not a straight yes/no
   * @param[in] q question
   * @param[in,out] a a pointer to a bool
   */
  std::string s;
  do{
    std::cout << q << std::endl;
    std::cin.clear(); //clear the error bits for the cin input stream
    std::cin.sync(); //synchronize the input buffer, discarding any leftover characters in the buffer
    getline(std::cin,s);
    std::transform(s.begin(), s.end(), s.begin(), upper()); // uppercase. encloses std::toupper to avoid trouble w/iostream version
  } while(!( s=="Y"
	     || s=="YES"
	     ||s=="N"
	     ||s=="NO"));
    if (s== "Y" || s=="YES"){
      *a=true;
    } else *a=false;
}


void question(int i,std::string q, bool * a) {
  /**
   * This question receives an integer, and a string containing "%i" in order to replace it with the integer during the
   * question, and returns a bool. Example: Is dimension X periodic? (Y/N)
   * @param[in] i the integer
   * @param[in] q the question
   * @param[in,out] a the bool answer
   */
  size_t pos=q.find("%i");
  if(pos!=std::string::npos) {
    q.replace(pos,2,intToStr(i));
  }
  std::string s;
  do{
    std::cout << q << std::endl; 
    std::cin.clear(); //clear the error bits for the cin input stream
    std::cin.sync(); //synchronize the input buffer, discarding any leftover characters in the buffer
    getline(std::cin,s);
    std::transform(s.begin(), s.end(), s.begin(), upper()); // uppercase. encloses std::toupper to avoid trouble with iostream version
  } while(!( s=="Y"
	   ||s=="YES"
	   ||s=="N"
	   ||s=="NO"));
  if (s== "Y" || s=="YES"){
      *a=true;
    } else *a=false;
}
