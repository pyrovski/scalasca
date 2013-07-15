/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SalsaParser.h"
#include "SalsaStringUtils.h"
#include "Error.h"
#include "mpi.h"
#include <stdlib.h>
#include <iostream>

using namespace std;
/* private methods */

  // string tokenizer
void SalsaParser::string2tokens(const string& str, list<string>& token_list) {
  stringstream ss(str);
  string buf;
  while (ss >> buf) {
    token_list.push_back(buf);
  }
}

void SalsaParser::error(int err) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if(rank==0) {
    cerr << "Error while parsing opt-file!" << endl << endl;
    cerr << "In line "<< line_number <<":" << endl;
    if(err >= 1 && err <= 3) {
      cerr << "Line has to start with a valid SALSA function";
      cerr << " (count, rate, length or duration)!" << endl;
    } else if(err >= 4 && err <= 6) {
      cerr << "Mode must be one of min, max, avg or sum(*)!" << endl;
      cerr << "(*): Not applicable for rate" << endl;
    } else if(err == 7 || err == 8) {
      cerr << "Constraint must be of type size, tag or comm!" << endl;
    } else if(err == 9 || err == 10) {
      cerr << "List must have the form \"[limit | (limit)*]\"!" << endl;
    } else if(err == 11) {
      cerr << "Argument for block_size must be a positive integer!" << endl;
    } else if(err >= 12 && err <= 28) {
      cerr << "Limitation must be of the form \"operator value1\" or";
      cerr << " \"value1 - value2\"!" << endl;
      cerr << "where operator must be one of ==, !=, >=, >, <= or <," << endl;
      cerr << "value1 (and value2) must be nonnegative integers!" << endl;
    } else if(err == 29) {
      cerr << "Size is not a multiple of Blocksize!" << endl;
    } else {
      cerr << "Unknown error code!" << endl;
    }
    cerr << endl;
    cerr << "Each line must be build as following (mind the order):" << endl;
    cerr << "function mode density(*) format(*) blocksize(*) constraints(*)";
    cerr << endl;
    cerr << "(*): optional"<< endl;
    MPI_Barrier(MPI_COMM_WORLD);
    exit(EXIT_FAILURE);
  }
  else {
    MPI_Barrier(MPI_COMM_WORLD);
    exit(EXIT_FAILURE);
  }
}

void SalsaParser::lmode() {
  if(it != tokens.end()) {
    if(*it == "min") {
      mode = *it;
    } else if(*it == "max") {
      mode = *it;
    } else if(*it == "avg") {
      mode = *it;
    } else {
      error(4);
    }
  } else {
    error(5);
  }
}

void SalsaParser::fmode() {
  if(it != tokens.end()) {
    if(*it == "sum") {
      mode = *it;
    } else {
      lmode();
    }
  } else {
    error(6);
  }
}

void SalsaParser::funct() {
  if(it != tokens.end()) {
    if(*it == "length" || *it == "duration") {
      func = *it;
    } else {
      error(2);
    }
  } else {
    error(3);
  }
}

string SalsaParser::type() {
  if(it != tokens.end()) {
    if(*it == "size" || *it == "tag" || *it == "comm") {
      return *it;
    } else {
      error(7);
    }
  } else {
    error(8);
  }
  return NULL;
}

void SalsaParser::density() {
  if(it != tokens.end()) {
    if(*it == "sparse") {
      sparse = true;
      ++it;
    } else if(*it == "dense") {
      sparse = false;
      ++it;
    }
  }
}

void SalsaParser::output_format() {
  if(it != tokens.end()) {
    if(*it == "ascii") {
      format = ascii;
      ++it;
    } else if(*it == "binary") {
      format = binary;
      ++it;
    } else if(*it == "sion") {
     format = sion;
     ++it;
//    } else if(*it == "netcdf") {
//      format = netcdf;
//      ++it;
    }
  }
}

void SalsaParser::block() {
  if(it != tokens.end()) {
    if(*it == "blocksize") {
      ++it;
      if(it != tokens.end() && string2val(*it, blocksize) && blocksize > 0) {
        ++it;
      } else {
        error(11);
      }
    }
  }
}

bool SalsaParser::op(string s) {
  return (s == "==" || s == "!=" || s == ">=" || s == ">" ||
       s == "<=" || s == "<");
}

void SalsaParser::val(string s) {
  if(it != tokens.end()) {
    string o = *it;
    int val, val2;
    if(s == "size") {
      if(op(o)) {
        ++it;
        if(it != tokens.end() && string2val(*it, val) && val >= 0) {
          mc.add_size_constraint(o,val);
        } else {
          error(12);
        }
      } else if(string2val(o, val) && val >= 0) {
        ++it;
        if(it != tokens.end()) {
          if(*it == "-") {
            ++it;
            if(it != tokens.end() && string2val(*it, val2) && val2 >= 0) {
              mc.add_size_constraint("-", val, val2);
            } else {
              error(13);
            }
          } else {
            error(14);
          }
        } else {
          error(15);
        }
      } else {
        error(16);
      }
    } else if(s == "tag") {
      if(op(o)) {
        ++it;
        if(it != tokens.end() && string2val(*it, val) && val >= 0) {
          mc.add_tag_constraint(o,val);
        } else {
          error(17);
        }
      } else if(string2val(o, val) && val >= 0) {
        ++it;
        if(it != tokens.end()) {
          if(*it == "-") {
            ++it;
            if(it != tokens.end() && string2val(*it, val2) && val2 >= 0) {
              mc.add_tag_constraint("-", val, val2);
            } else {
              error(18);
            }
          } else {
            error(19);
          }
        } else {
          error(20);
        }
      } else {
        error(21);
      }
    } else if(s == "comm") {
      if(op(o)) {
        ++it;
        if(it != tokens.end() && string2val(*it, val) && val >= 0) {
          mc.add_comm_constraint(o,val);
        } else {
          error(22);
        }
      } else if(string2val(o, val) && val >= 0) {
        ++it;
        if(it != tokens.end()) {
          if(*it == "-") {
            ++it;
            if(it != tokens.end() && string2val(*it, val2) && val2 >= 0) {
              mc.add_comm_constraint("-", val, val2);
            } else {
              error(23);
            }
          } else {
            error(24);
          }
        } else {
          error(25);
        }
      } else {
        error(26);
      }
    } else {
      error(27);
    }
  } else {
    error(28);
  }
}

void SalsaParser::lval(string s) {
  if(it != tokens.end()) {
    val(s);
    ++it;
    if(it != tokens.end()) {
      if(*it == "]") {
        return;
      } else if (*it == "|") { // ignore "|" (optional separator)
        ++it;
        lval(s);
      } else {
        lval(s);
      }
    } else {
      error(9);
    }
  } else error(10);
}

void SalsaParser::constraint() {
  if(it != tokens.end()) {
    string s = type();
    ++it;
    if(*it == "[") {
      ++it;
      lval(s);
      ++it;
      constraint();
    } else {
      val(s);
      ++it;
      constraint();
    }
  }
}

void SalsaParser::func_mode() {
  if(it != tokens.end()) {
    if(*it == "count") {
      func = *it;
    } else if(*it == "rate") {
      func = *it;
      ++it;
      lmode();
    } else {
      funct();
      ++it;
      fmode();
    }
  } else {
    error(1);
  }
}

/* public methods */

int SalsaParser::parseNext() {
  MPI_Barrier(MPI_COMM_WORLD);

  sparse = sparse_glob;
  blocksize = blocksize_glob;
  format = format_glob;

  //cout << "Start parsing!" << endl;

  buffer[0]='\0';
  if(!file.eof()) {
    ++line_number;
    file.getline(buffer,256);
    tokens.clear();
    mc.clear();
    mode = "";
    func = "";
    string toTokens(buffer);
    string2tokens(toTokens, tokens);
    //cout << "String: " << buffer << endl;
    it = tokens.begin();
    if(it == tokens.end()) return 0;
    func_mode();
    ++it;
    density();
    output_format();
    block();

    list<string>::iterator tmp;
    stringstream s;
    tmp = it;
    while (tmp != tokens.end()) {
       s << *tmp << " ";
       tmp++;
    }
    constraints_ = s.str();
    constraint();
  }  else {
	return 0;
  }
  if(func=="count") {
    stat_func=::count;
  } else if(func=="length") {
    stat_func=length;
  } else if(func=="duration") {
    stat_func=duration;
  } else if(func=="rate") {
    stat_func=rate;
  } else {
    return 0;
  }

  if(stat_func != ::count) {
    if(mode=="min") {
      stat_mode=minimum;
    } else if(mode=="max") {
      stat_mode=maximum;
    } else if(mode=="avg") {
      stat_mode=avg;
    } else if(mode=="sum") {
      stat_mode=sum;
    } else {
      return 0;
    }
  }
  if(size % blocksize != 0) {
    error(29);
  }
  return 1;
}
