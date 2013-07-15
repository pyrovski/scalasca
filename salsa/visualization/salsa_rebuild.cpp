#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Comm_Matrix.hpp"

#define DEBUG1

using namespace std;

int main(int argc, char **argv) {

  int size, blocksize;
  ifstream metafile;
  ifstream tmpfile;
  string path;
  string filename;
  string dirname;
  string mode, format, density;
  string dummy;
  int rsize;
  string rmode;

  if(argc < 2) {
  	cerr << "To few arguments!" << endl;
  	exit(1);
  }

  tmpfile.open(argv[1]);
  tmpfile >> path;
  tmpfile >> rmode;
  tmpfile >> rsize;
  tmpfile.close();
  filename = path+"/meta.data";

  metafile.open(filename.c_str());
  metafile >> dummy >> dirname;
  metafile >> dummy >> size;
  metafile >> dummy >> blocksize;
  metafile >> dummy >> mode;
  metafile >> dummy >> format;
  metafile >> dummy >> density;

#ifdef DEBUG
  cerr << "Datei: " << dirname << endl;
  cerr << "Size: " << size << endl;
  cerr << "Blocksize: " << blocksize << endl;
  cerr << "Mode: " << mode << endl;
  cerr << "Format: " << format << endl;
  cerr << "Density: " << density << endl;
#endif

  if(mode == "count" || mode == "length") {
  	Comm_Matrix<uint64_t> cm(path, rsize, rmode);
  	cm.print();
  } else if(mode == "rate" || mode == "duration") {
  	Comm_Matrix<double> cm(path, rsize, rmode);
  	cm.print();
  }

}
