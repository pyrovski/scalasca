/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef SALSASTRINGUTILS_H_
#define SALSASTRINGUTILS_H_

#include <sstream>
#include <string>

// object<->string conversion routines
template<typename T>
std::string val2string(const T& i) {
  std::ostringstream num;
  num << i;
  return num.str();
}

template<typename T>
bool string2val(const std::string& str, T& t) {
  std::istringstream buf(str);
  buf >> t;
  return (buf.eof());
}

#endif /* SALSASTRINGUTILS_H_ */
