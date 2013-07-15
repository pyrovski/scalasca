/****************************************************************************
 **  SCALASCA    http://www.scalasca.org/                                   **
 *****************************************************************************
 **  Copyright (c) 1998-2013                                                **
 **  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
 **                                                                         **
 **  See the file COPYRIGHT in the package base directory for details       **
 ****************************************************************************/

#ifndef COMM_MATRIX_HPP_
#define COMM_MATRIX_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#define SALSA_LITTLE_ENDIAN 0
#define SALSA_BIG_ENDIAN    1

using namespace std;

template<typename T>
string val2string(T i) {
  ostringstream num;
  num << i;
  return num.str();
}

int endian() {
	int i = 1;
	char *p = (char *)&i;

	if (p[0] == 1) {
		return SALSA_LITTLE_ENDIAN;
	}
	return SALSA_BIG_ENDIAN;
}

inline void byte_swap(void* buffer, size_t length) {
  /* length must be an even number */

  char tmp;
  size_t i;

  if (length > 1) {
    for (i = 0; i < length / 2; i++) {
				tmp = ((char*) buffer)[i];
				((char*) buffer)[i] = ((char*) buffer)[length - 1 - i];
				((char*) buffer)[length - 1 - i] = tmp;
      }
  }
}

/**
 * Class representing a communication matrix created by SALSA
 *
 * @tparam T 	Choose T according to the SALSA mode:\n
 * 						count, length  => T = uint64_t \n
 * 						duration, rate => T = double
 */
template<typename T>
class Comm_Matrix {
private:
	string dirname;
	string mode;
	string density;
	string format;
	int size;
	int blocksize;
	int reduced_size;
	T max;
	T min;
	T *matrix;
	bool reduced;
	bool swap;


	void error(int error_code) {
		switch(error_code) {
		case 1:
			cerr << "Error reading meta data file!" << endl;
			break;
		case 2:
			cerr << "Error! The dimension of the reduced matrix is not valid.";
			cerr << endl;
			cerr << "The block size must be a multiple of size/reduced size!";
			cerr << endl;
			break;
		case 3:
			cerr << "Error in meta data file! Unknown density" << endl;
			break;
		case 4:
			cerr << "Error in meta data file! Unknown format" << endl;
			break;
		default:
			cerr << "Unknown error!" << endl;
		}
		exit(1);
	}

	void print_reduced() {
		for(int i=0; i<reduced_size*reduced_size; ++i) {
			cout << matrix[i] << " ";
			if((i+1) % reduced_size == 0) {
				cout << endl;
			}
		}
	}

	void print_full() {
		for(int i=0; i<size*size; ++i) {
			cout << matrix[i] << " ";
			if((i+1) % size == 0) {
				cout << endl;
			}
		}
	}

	void ascii_sparse() {
		ifstream blockfile;
		string blockname;
		int n_nze[blocksize];
		int sum;

		// Read matrix from file(s)
		for(int i=0; i<size; i+=blocksize) {
			blockname = dirname + "/block" + val2string(i) + ".out";
			blockfile.open(blockname.c_str());
			for(int j=0; j<blocksize; ++j) {
				blockfile >> n_nze[j];
			}
			sum = 0;
			for(int j=0; j<blocksize; ++j) {
				sum+=n_nze[j];
			}
			int pos_nze[sum];
			for(int j=0; j<sum; ++j) {
				blockfile >> pos_nze[j];
			}
			int l=0;
			for(int j=0; j<blocksize; ++j) {
				for(int k=0; k<n_nze[j]; ++k) {
					blockfile >> matrix[pos_nze[l++]+(size*(j+i))];
				}
			}
			blockfile.close();
		}
	}

	void ascii_dense() {
		ifstream blockfile;
		string blockname;

		for(int i=0; i<size; i+=blocksize) {
			blockname = dirname + "/block" + val2string(i) + ".out";
			blockfile.open(blockname.c_str());
			for(int j=0; j<blocksize; ++j) {
				for(int k=0; k<size; ++k) {
					blockfile >> matrix[size*(i+j)+k];
				}
			}
			blockfile.close();
		}
	}

	void binary_sparse() {
		ifstream blockfile;
		string blockname;
		int n_nze[blocksize];
		int sum;

		for(int i=0; i<size; i+=blocksize) {
			blockname = dirname + "/block" + val2string(i) + ".out";
			blockfile.open(blockname.c_str(), ios::binary);
			blockfile.read((char*)(&n_nze[0]),blocksize*sizeof(int));
			// Change Endianess if necessary
			if(swap) {
				for(int j=0; j<blocksize; ++j) {
					byte_swap(&n_nze[j],sizeof(int));
				}
			}
			sum = 0;
			for(int j=0; j<blocksize; ++j) {
				sum+=n_nze[j];
			}
			int pos_nze[sum];
			blockfile.read((char*)(&pos_nze[0]),sum*sizeof(int));
			// Change Endianess if necessary
			if(swap) {
				for(int j=0; j<sum; ++j) {
					byte_swap(&pos_nze[j], sizeof(int));
				}
			}
			int l=0;
			for(int j=0; j<blocksize; ++j) {
				for(int k=0; k<n_nze[j]; ++k) {
					int idx = pos_nze[l++]+size*(i+j);
					blockfile.read((char*)(&matrix[idx]),sizeof(T));
					// Change Endianess if necessary
					if(swap) {
						byte_swap(&matrix[idx], sizeof(T));
					}
				}
			}
			blockfile.close();
		}
	}

	void binary_dense() {
		ifstream blockfile;
		string blockname;


		// Read Matrix from file(s)
		for(int i=0; i<size; i+=blocksize) {
			blockname = dirname + "/block" + val2string(i) + ".out";
			blockfile.open(blockname.c_str(), ios::binary);
			if(!blockfile) {
				cerr << "Error opening file!" << endl;
				exit(1);
			}
			for(int j=0; j<blocksize; ++j) {
					blockfile.read((char*)(&matrix[size*(i+j)]),size*sizeof(T));
			}
			blockfile.close();
		}
		// Change Endianess if necessary
		if(swap) {
			for(int i=0; i<size*size; ++i) {
				byte_swap(&matrix[i], sizeof(T));
			}
		}
	}

	void read_full_matrix() {
		if(format == "ascii") {
			if(density == "dense") {
				ascii_dense();
			} else if(density == "sparse") {
				ascii_sparse();
			} else {
				error(3);
			}
		} else if(format == "binary") {
			if(density == "dense") {
				binary_dense();
			} else if(density == "sparse") {
				binary_sparse();
			} else {
				error(3);
			}
		} else {
			error(4);
		}
	}


	void get_block(int idx, T *block) {
		ifstream blockfile;
		string blockname;

		blockname = dirname + "/block" + val2string(idx) + ".out";

		if(format == "binary") {
			blockfile.open(blockname.c_str(), ios::binary);
			if(density == "dense") {
				for(int j=0; j<blocksize; ++j) {
					blockfile.read((char*)(&block[j*size]),size*sizeof(T));
				}
				// Change Endianess if necessary
				if(swap) {
					for(int l=0; l<blocksize*size; ++l) {
						byte_swap(&block[l], sizeof(T));
					}
				}
			} else if(density == "sparse") {
				int sum;
				int n_nze[blocksize];

				// Read in binary sparse matrix block
				blockfile.read((char*)(&n_nze[0]),blocksize*sizeof(int));
				// Change Endianess if necessary
				if(swap) {
					for(int l=0; l<blocksize; ++l) {
						byte_swap(&n_nze[l], sizeof(int));
					}
				}
				sum = 0;
				for(int j=0; j<blocksize; ++j) {
					sum+=n_nze[j];
				}
				int pos_nze[sum];
				blockfile.read((char*)(&pos_nze[0]),sum*sizeof(int));
				// Change Endianess if necessary
				if(swap) {
					for(int l=0; l<sum; ++l) {
						byte_swap(&pos_nze[l], sizeof(int));
					}
				}
				int l=0;
				for(int j=0; j<blocksize; ++j) {
					for(int k=0; k<n_nze[j]; ++k) {
						int idx = pos_nze[l++]+size*(j);
						blockfile.read((char*)(&block[idx]),sizeof(T));
						// Change Endianess if necessary
						if(swap) {
							byte_swap(&block[idx],sizeof(T));
						}
					}
				}
			} else {
				error(3);
			}
		} else if(format == "ascii") {
			blockfile.open(blockname.c_str());
			if(density == "dense") {
				for(int k=0; k<blocksize; ++k) {
					for(int j=0; j<size; ++j) {
						blockfile >> block[k*size+j];
					}
				}
			} else if(density == "sparse") {
				int sum;
				int n_nze[blocksize];
				// Read in ascii sparse matrix block
				for(int j=0; j<blocksize; ++j) {
					blockfile >> n_nze[j];
				}
				sum = 0;
				for(int j=0; j<blocksize; ++j) {
					sum+=n_nze[j];
				}
				int pos_nze[sum];
				for(int j=0; j<sum; ++j) {
					blockfile >> pos_nze[j];
				}
				int l=0;
				for(int j=0; j<blocksize; ++j) {
					for(int k=0; k<n_nze[j]; ++k) {
						blockfile >> block[pos_nze[l++]+(size*(j))];
					}
				}
			} else {
				error(3);
			}
		} else {
			error(4);
		}
		blockfile.close();
	}

	void read_reduced_matrix(int rsize, const string& mode) {

		T *block = new T[size*blocksize];
		int step_x = size / rsize;
		int idx = 0;

		for(int i=0; i<size; i+=blocksize) {
			memset(block, 0, size*blocksize*sizeof(T));
			get_block(i, block);
//			cout << "Block " << i << endl;
//			for(int k=0; k<blocksize; ++k) {
//				for(int j=0; j<size; ++j) {
//					cout << block[k*size+j] << " ";
//				}
//				cout << endl;
//			}
			for(int k=0; k<blocksize; k+=step_x) {
				for(int j=0; j<size; j+=step_x) {
					if(mode == "max") {
						T max = 0;
						for(int l=0; l<step_x; ++l) {
							for(int m=0; m<step_x; ++m) {
								max=std::max(block[(k+m)*size+(j+l)],max);
							}
						}
						matrix[idx++] = max;
					} else if(mode == "min") {
						T min = max;
						bool nze = false;
						for(int l=0; l<step_x; ++l) {
							for(int m=0; m<step_x; ++m) {
								min = block[(k+m)*size+(j+l)]!=0 ?
										  std::min(block[(k+m)*size+(j+l)],min) :
										  min;
								if(block[(k+m)*size+(j+l)]!=0) nze = true;
							}
						}
						matrix[idx++] = nze ? min : 0;

					} else if(mode == "avg") {
						T sum = 0;
						for(int l=0; l<step_x; ++l) {
							for(int m=0; m<step_x; ++m) {
								sum += block[(k+m)*size+(j+l)];
							}
						}
						matrix[idx++] = sum/(step_x*step_x);
					} else if(mode == "random") {
						matrix[idx++] = block[(k+rand()%step_x)*size+(j+rand()%step_x)];
					} else if(mode == "nze")	{
						T nze = 0;
						for(int l=0; l<step_x; ++l) {
							for(int m=0; m<step_x; ++m) {
								if(block[(k+m)*size+(j+l)]!=0) {
									nze++;
								}
							}
						}
						matrix[idx++] = nze;
					} else if(mode == "sum") {
						T sum = 0;
						for(int l=0; l<step_x; ++l) {
							for(int m=0; m<step_x; ++m) {
								sum += block[(k+m)*size+(j+l)];
							}
						}
						matrix[idx++] = sum;
					}
				}
			}
		}
		delete[] block;
	}

	void read_meta_data(const char *dir) {
		ifstream metafile;
		string dummy;
		string endianess;
		string filename = dir;
		filename += "/meta.data";

		metafile.open(filename.c_str());
		if(!metafile) {
			error(1);
		}

		metafile >> dummy >> dirname;
		metafile >> dummy >> size;
		metafile >> dummy >> blocksize;
		metafile >> dummy >> mode;
		metafile >> dummy >> format;
		metafile >> dummy >> density;
		metafile >> dummy >> min;
		metafile >> dummy >> max;
		metafile >> dummy >> endianess;
		dirname = dir; // necessary if not called from results top directory

		metafile.close();

		if((endianess == "little" && endian() == SALSA_LITTLE_ENDIAN)||
				(endianess == "big" && endian() == SALSA_BIG_ENDIAN)) {
			swap = false;
		} else {
			swap = true;
		}

//		cout << "Datei: " << dirname << endl;
//		cout << "Size: " << size << endl;
//		cout << "Blocksize: " << blocksize << endl;
//		cout << "Mode: " << mode << endl;
//		cout << "Format: " << format << endl;
//		cout << "Density: " << density << endl;
//		cout << "Min: " << min << endl;
//		cout << "Max: " << max << endl;
	}

	void init_reduced(int rsize, const string& mode) {
		if(blocksize % (size/rsize) != 0) {
			error(2);
		}
		reduced = true;
		reduced_size = rsize;
		matrix = new T[rsize * rsize];
		if(density == "sparse") {
			memset(matrix, 0, rsize*rsize*sizeof(T));
		}
		srand((unsigned)time(0));
		read_reduced_matrix(rsize, mode);
	}

	void init_full() {
		reduced = false;
		matrix = new T[size*size];
		if(density == "sparse") {
			memset(matrix, 0, size*size*sizeof(T));
		}
		read_full_matrix();
	}

public:
//	Comm_Matrix(const char *dir) {
//		read_meta_data(dir);
//		init_full();
//	}

	/**
	 * Generates the complete communication matrix.
	 * @param dir The directory containing the SALSA result.
	 */
	Comm_Matrix(const string &dir) {
		read_meta_data(dir.c_str());
		init_full();
	}

//	Comm_Matrix(const char *dir, int rsize, const string &mode) {
//		read_meta_data(dir);
//		init_reduced(rsize, mode);
//	}

	/**
	 * Generates a reduced communication matrix.
	 * @param dir The directory containing the SALSA result.
	 * @param rsize The dimension of the reduced matrix.
	 * @param mode The reduction mode. \n
	 * Possible values are: max, min, avg, random, nze, and sum
	 */
	Comm_Matrix(const string &dir, int rsize, const string &mode) {
		read_meta_data(dir.c_str());
		if(rsize < size) {
			init_reduced(rsize, mode);
		} else {
			init_full();
		}
	}

	/**
	 * Frees the memory used for storing the matrix.
	 */
	virtual ~Comm_Matrix() {
		delete[] matrix;
	}

	/**
	 * Prints the matrix to standard output.
	 */
	void print() {
		if(reduced) {
			print_reduced();
		} else {
			print_full();
		}
	}

	/**
	 * Enables element-wise access to the matrix.
	 * @param x : x index
	 * @param y : y index
	 * @return The matrix element at position [x,y]
	 */
	T operator()(int x, int y) {
		if(reduced) {
			return matrix[x*reduced_size+y];
		}
		return matrix[x*size+y];
	}

	/**
	 * @return The size of the matrix.
	 */
	int get_size() {
		if(reduced) {
			return reduced_size;
		}
		return size;
	}
};

#endif /* COMM_MATRIX_HPP_ */
