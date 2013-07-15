/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "salsa.h"

#include "SalsaCallback.h"
#include "MessageChecker.h"
#include "SalsaParser.h"
#include "SalsaStringUtils.h"
#include "LocalTrace.h"
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <cstring>

#include "pearl.h"
#include "pearl_replay.h"
#include "epk_archive.h"
#include "Error.h"


#ifdef USE_SIONLIB
#define SION_MPI
#include "sion.h"
#endif
#define FNAMELEN 255

#define SALSA_LITTLE_ENDIAN 0
#define SALSA_BIG_ENDIAN    1

#define DEBUG

using namespace std;
using namespace pearl;
//using std::ofstream;

#ifdef DEBUG
double start_time;
double run_time;
#endif

string func;
string mode;
bool sparse_glob = false;
bool sparse = sparse_glob;
long blocksize_glob = 0;
long blocksize = blocksize_glob;
formats format_glob=ascii;
formats format = format_glob;
string constraints;
funcs stat_func;
modes stat_mode;

MessageChecker mc;

int line_number=0;

// Endianess checker
int endian() {
        int i = 1;
        char *p = (char *)&i;

        if (p[0] == 1) {
                return SALSA_LITTLE_ENDIAN;
        }
        return SALSA_BIG_ENDIAN;
}

inline string get_app_name(char *epik_dir) {
   int idx = 0;
   string str(epik_dir);
   if(str.compare(0,5,"epik_") == 0) {
	   idx = 5;
   }
   return str.substr(idx, str.find_first_of('_', idx+1)-idx);
}

inline void write_meta_data_reduced(string dir_name, int size, int blocksize,
                double &global_min, double &global_max) {
        ofstream metadatafile;
        string meta_name = dir_name + "/" + "meta.reduced.data";

        metadatafile.open(meta_name.c_str());

        metadatafile << "Directory:  " << dir_name << endl;
        metadatafile << "Size:  " << size << endl;
        metadatafile << "Blocksize:  " << blocksize << endl;
        metadatafile << "Function: " << func << endl;
        metadatafile << "Mode: " << mode << endl;
        //metadatafile << "Format: ascii" << endl;
        metadatafile << "Format: binary" << endl;
        //metadatafile << "Density: sparse" << endl;
        metadatafile << "Density: dense" << endl;
        metadatafile << "Global_Min: " << global_min << endl;
        metadatafile << "Global_Max: " << global_max << endl;
        if(endian() == SALSA_LITTLE_ENDIAN) {
                metadatafile << "Endianess: little" << endl;
        } else {
                metadatafile << "Endianess: big" << endl;
        }
        metadatafile << "Constraints: " << constraints << endl;
}

inline void write_meta_data(string dir_name, char *epik_dir, int size, int blocksize,
                double &global_min, double &global_max) {
        ofstream metadatafile;
        string meta_name = dir_name + "/" + "meta.data";

        metadatafile.open(meta_name.c_str());

        string app_name= get_app_name(epik_dir);
        metadatafile << "Application:  " << app_name << endl;
        metadatafile << "Size:  " << size << endl;
        metadatafile << "Blocksize:  " << blocksize << endl;
        metadatafile << "Function: " << func << endl;
        metadatafile << "Mode: " << mode << endl;
        switch(format) {
        case ascii:
                metadatafile << "Format: ascii" << endl;
                break;
        case binary:
                metadatafile << "Format: binary" << endl;
                break;
        case sion:
                metadatafile << "Format: sion" << endl;
                break;
//	case netcdf:
//		metadatafile << "Format: netcdf" << endl;
//		break;
        }
        if(sparse) {
                metadatafile << "Density: sparse" << endl;
        } else {
                metadatafile << "Density: dense" << endl;
        }
        metadatafile << "Global_Min: " << global_min << endl;
        metadatafile << "Global_Max: " << global_max << endl;
        if(endian() == SALSA_LITTLE_ENDIAN) {
                metadatafile << "Endianess: little" << endl;
        } else {
                metadatafile << "Endianess: big" << endl;
        }
        metadatafile << "Constraints: " << constraints << endl;
}


#include <sys/stat.h>
static int create_dir(const char *path, mode_t mode)
{
    struct stat st;
    int status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist */
        if (mkdir(path, mode) != 0)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        status = -1;
    }
    return(status);
}

static int mkdirs(const char *path, mode_t mode)
{
    char           *pp;
    char           *sp;
    int             status = 0;
    char           *copypath = strdup(path);

    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
            *sp = '\0';
            status = create_dir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = create_dir(path, mode);
    free(copypath);
    return (status);
}

#include <time.h>
#include <iomanip>
inline string gen_dir_name(char *epik_dir, int size) {

      string appname= get_app_name(epik_dir);

      string dirname;
      stringstream s;
      s << "salsa_" << appname << "/" << size << "_" << func;
      if (mode.length() > 0) s << "_"<< mode;

      time_t rawtime;
      time ( &rawtime );
      struct tm * ti;
      ti = localtime ( &rawtime );
      s << "_" <<  setw(2) << setfill('0') << ti->tm_mday
            << "." << setw(2) << setfill('0') << ti->tm_mon + 1
            << "_" << setw(2) << setfill('0') << ti->tm_hour
            << "." << setw(2) << setfill('0') << ti->tm_min
            << "." << setw(2) << setfill('0') << ti->tm_sec;

      dirname = s.str();

      //cerr << "output directory: "<< dirname << endl;
      return dirname;
}

/**
  generates a set of reduced matrices, which are saved in subdirectories of current matrix
  the matrix is reduced to a size smaller or equal 4096
  */
void output_reduced(double *recv_buf, int rank, int size, int x_block_size,
            funcs stat_func, char *epik_dir, double &global_min, double &global_max,
            MPI_Comm block_comm, MPI_Comm local_root_comm)
{
   if (size <= 4096) return; // too small to save reduced matrix set

   int factor = 2;
   int reduction = 1;
   while (size > 4096 && factor <= 7)  { // compute possible reduction factor to reduce to size <= 4096
      if (size % factor == 0) {
         size /= factor;
         reduction *= factor;
      }
      else {
         factor++;
      }
   }
   if (reduction == 1 || size == 1) return;

   // generate directory name
   string dir_name = gen_dir_name(epik_dir, size);
   ofstream outputfile;

   double *buf_max, *buf_min, *buf_avg, *buf_sum, *buf_nze;
   buf_max = new double[size * x_block_size * sizeof(double)];
   buf_min = new double[size * x_block_size * sizeof(double)];
   buf_avg = new double[size * x_block_size * sizeof(double)];
   buf_sum = new double[size * x_block_size * sizeof(double)];
   buf_nze = new double[size * x_block_size * sizeof(double)];

   memset(buf_max, 0, size * x_block_size * sizeof(double));
   memset(buf_sum, 0, size * x_block_size * sizeof(double));
   memset(buf_avg, 0, size * x_block_size * sizeof(double));
   memset(buf_nze, 0, size * x_block_size * sizeof(double));
   for (int i=0; i<size*x_block_size; ++i)
      buf_min[i] = global_max;

   // reduce block
   int rsize = size / reduction;
   for (int y = 0; y < x_block_size; y++) {
      for (int x=0; x < size; ++x) {
         int idx = (y/reduction)*rsize+(x/reduction);
         int blockidx = y*size+x;
         buf_max[idx] = std::max(recv_buf[blockidx], buf_max[idx]);
         buf_sum[idx] += recv_buf[blockidx];
         buf_avg[idx] += recv_buf[blockidx];
         buf_min[idx] =  recv_buf[blockidx]!=0 ?
                         std::min(recv_buf[blockidx], buf_min[idx]) :
                         buf_min[idx];
         buf_nze[idx] += recv_buf[blockidx]!=0 ? 1 : 0;
      }
   }
   // write reduced matrices
   string types[]={"min","max","avg","nze","sum"};
   double *arrays[]={buf_min, buf_max, buf_avg, buf_nze, buf_sum};
   for (int nr=0; nr<5; nr++) {
      string r_dir_name= dir_name + "/reduction_" + types[nr];
      //cerr << "dirname " << r_dir_name<< endl;
      mkdirs(r_dir_name.c_str(), S_IRWXU | S_IRGRP | S_IROTH);

      string fname= r_dir_name + "/" + "block" + val2string(rank/reduction) + ".out";

      double *array = arrays[nr];

      outputfile.open(fname.c_str(), ios::out|ios::binary);
      int length = (x_block_size / reduction) * (size/reduction);
      outputfile.write((char*) array, length*sizeof(double));
      outputfile.close();

      /* ascii
              outputfile.open(fname.c_str(),ios::out);
              T *array = arrays[nr];
              for(int i=0; i< x_block_size / reduction; i++) {
                      for(int j=0; j< size / reduction; j++) {
                              outputfile << array[i*size/reduction+j] << " ";
                      }
                      outputfile << endl;
              }
              outputfile.close(); */

      if (rank == 0) {
         write_meta_data_reduced(r_dir_name, rsize, x_block_size/reduction, global_min, global_max);
      }
   }
   delete[] buf_min;
   delete[] buf_max;
   delete[] buf_avg;
   delete[] buf_sum;
   delete[] buf_nze;
}

void sparse_output_reduced(double *recv_buf_nze, int *recv_buf_pos, int *n_nzes,
                   int sum, int rank, int size, int x_block_size, funcs stat_func,
                   char* epik_dir, double &global_min, double &global_max, MPI_Comm block_comm,
                   MPI_Comm local_root_comm)
{
   double *buf = new double[size * x_block_size];
   memset(buf, 0, size * x_block_size * sizeof(double));

   int l=0;
   for(int j=0; j<x_block_size; ++j) {
      for(int k=0; k<n_nzes[j]; ++k) {
         int idx = recv_buf_pos[l]+size*j;
         buf[idx] = recv_buf_nze[l];
         l++;
      }
   }
   output_reduced(buf, rank, size, x_block_size,
               stat_func, epik_dir, global_min, global_max,
               block_comm, local_root_comm);
   delete[] buf;
}

void output(double *recv_buf, int rank, int size, int x_block_size,
                        funcs stat_func, char *epik_dir, double &global_min, double &global_max,
                        MPI_Comm block_comm, MPI_Comm local_root_comm) {

        MPI_Barrier(local_root_comm);

#ifdef DEBUG
        if (rank==0) {
          run_time = MPI_Wtime()-start_time;
          cout << func << " " << mode << ": Time without output= ";
          cout << run_time << endl;
        }
#endif

        // generate directory name
        string dir_name = gen_dir_name(epik_dir, size);

        // make directory
        mkdirs(dir_name.c_str(),S_IRWXU | S_IRGRP | S_IROTH);
        if (rank == 0) cout << "writing output to " << dir_name << endl;

        if (format == sion)
        {
#ifdef USE_SIONLIB
           int           sid;
           char          fname[FNAMELEN];
           sion_int64    chunksize;
           FILE          *fileptr;

           chunksize  = size * x_block_size * sizeof(double);

           /* inital parameters */
           strcpy(fname, "salsafile.sion");
           int numFiles   = 1;
           MPI_Comm lComm = local_root_comm;
           sion_int32 fsblksize  = chunksize; // blocksize of filesystem. -1 = use default
           int globalrank = rank;
           //const int MAX=8;
           //int headersize = 2*sizeof(sion_int32) + (2*MAX+1)*sizeof(char)+2*sizeof(double);
           //if (rank == 0) chunksize += headersize;
           string sfile = dir_name+"/data.sion";
           sid = sion_paropen_mpi((char *) sfile.c_str(), "bw", &numFiles, lComm, &lComm,
                                  &chunksize, &fsblksize, &globalrank, &fileptr, NULL);
            /*if (rank == 0) { // write header
              char str[MAX]; str[MAX-1]=0;
              fwrite(&headersize , sizeof(int), 1, fileptr);
              fwrite(&size , sizeof(int), 1, fileptr);
              strncpy(str, func.c_str(), MAX-1);
              fwrite(str , sizeof(char), MAX, fileptr);
              strncpy(str, mode.c_str(), MAX-1);
              fwrite(mode.c_str() , sizeof(char), MAX, fileptr);
              fwrite(&sparse, sizeof(char), 1, fileptr);
              fwrite(&global_min, sizeof(double), 1, fileptr); // global min
              fwrite(&global_max, sizeof(double), 1, fileptr); // global max
           }*/

           //sion_writedata(recv_buf, size * x_block_size * sizeof(double), sid, fileptr);
           sion_fwrite(recv_buf, sizeof(double), size * x_block_size, sid);

           sion_parclose_mpi(sid);
#else
           cerr << "Error: salsa has been created without sion support";
#endif
        }
        else { // not sion format

           // generate directory name
           string dir_name = gen_dir_name(epik_dir, size);

           // make directory
           mkdirs(dir_name.c_str(),S_IRWXU | S_IRGRP | S_IROTH);

           // generate filename
           string filename = dir_name + "/" + "block" + val2string(rank) + ".out";

           fflush(stdout);
           ofstream outputfile;

           switch(format) {
                case ascii:
                        // open file
                        outputfile.open(filename.c_str(),ios::out);
                        for(int i=0; i<x_block_size; i++) {
                                for(int j=0; j<size; j++) {
                                        outputfile << recv_buf[i*size+j] << " ";
                                }
                                outputfile << endl;
                        }
                        // close file
                        outputfile.close();
                        break;
                case binary:
                        // open file
                        outputfile.open(filename.c_str(),ios::out|ios::binary);
                        // write header
                        //outputfile.write("SALSA BD",8);
                        // write standard matrix
                        for(int j=0; j<x_block_size; j++) {
                                //for(int i=0;i<size;i++) {
                                        outputfile.write((const char*)(&recv_buf[j*size]),size*sizeof(double));
                                //}
                        }
                        // close file
                        outputfile.close();
                        break;
                default:
                        break;
           }
        } //if

        // write metadata file
        if(rank == 0) {
           write_meta_data(dir_name, epik_dir, size, x_block_size, global_min, global_max);
        }
}


void sparse_output(double *recv_buf_nze, int *recv_buf_pos, int *n_nzes,
                  int sum, int rank, int size, int x_block_size, funcs stat_func,
                  char* epik_dir, double &global_min, double &global_max, MPI_Comm block_comm,
                  MPI_Comm local_root_comm) {

        MPI_Barrier(local_root_comm);

#ifdef DEBUG
        if(rank==0) {
          run_time = MPI_Wtime()-start_time;
          cout << func << " " << mode << ": Time without output= ";
          cout << run_time << endl;
        }
#endif

        // generate directory name
        string dir_name = gen_dir_name(epik_dir, size);
        if (rank == 0) cout << "writing output to " << dir_name << endl;

        // make directory
        mkdirs(dir_name.c_str(),S_IRWXU | S_IRGRP | S_IROTH);

        if (format == sion)
        {
#ifdef USE_SIONLIB
           int           sid;
           char          fname[FNAMELEN];
           FILE          *fileptr;

           int nonzeros = size * size / 4; // estimated non zero elements in matrix
           sion_int64  chunksize  = x_block_size*sizeof(int) + nonzeros*sizeof(int) + nonzeros*sizeof(double);

           /* inital parameters */
           strcpy(fname, "salsafile.sion");
           int numFiles   = 1;
           MPI_Comm lComm = local_root_comm;
           sion_int32 fsblksize  = chunksize; // blocksize of filesystem. -1 = use default => todo
           int globalrank = rank;
           string sfile = dir_name+"/data.sion";
           sid = sion_paropen_mpi((char *) sfile.c_str(), "bw", &numFiles, lComm, &lComm,
                                  &chunksize, &fsblksize, &globalrank, &fileptr, NULL);

           sion_fwrite(n_nzes, sizeof(int), x_block_size, sid);
           sion_fwrite(recv_buf_pos, sizeof(int), sum, sid);
           sion_fwrite(recv_buf_nze, sizeof(double), sum, sid);
           sion_parclose_mpi(sid);
#else
           cerr << "Error: salsa has been created without sion support";
#endif
        }
        else
        {
           // generate filename
           string filename = dir_name + "/" + "block" + val2string(rank) + ".out";

           fflush(stdout);
           ofstream outputfile;

           switch(format) {
             case ascii:
              // open file
              outputfile.open(filename.c_str(),ios::out);
              for(int i = 0; i < x_block_size-1; i++) {
                 outputfile << n_nzes[i] << " ";
              }
              outputfile << n_nzes[x_block_size-1] << endl;
              for(int i = 0; i < sum -1; i++) {
                 outputfile << recv_buf_pos[i] << " ";
              }
              outputfile << recv_buf_pos[sum-1] << endl;
              for(int i = 0; i < sum -1; i++) {
                 outputfile << recv_buf_nze[i] << " ";
              }
              outputfile << recv_buf_nze[sum-1] << endl;
              // close file
              outputfile.close();
              break;
            case binary:
              // open file
              outputfile.open(filename.c_str(),ios::out|ios::binary);
              outputfile.write((const char*)(&n_nzes[0]), x_block_size*sizeof(int));
              outputfile.write((const char*)(&recv_buf_pos[0]), sum*sizeof(int));
              outputfile.write((const char*)(&recv_buf_nze[0]), sum*sizeof(double));
              // close file
              outputfile.close();
              break;
             default:
               break;
           }

           cout << "Creating file: " << filename << "     finished" << endl;
        }
        // write metadata file
        if(rank == 0) {
                write_meta_data(dir_name, epik_dir, size, x_block_size, global_min, global_max);
        }
}



void printhelp(int rank) {
	if(rank==0) {
        cerr << endl;
        cerr << "***************************************************************";
        cerr << endl;
        cerr << "Usage:  salsa [options]" << endl;
        cerr << endl;
        cerr << "Options: " << endl;
        cerr << "  -e, --experiment:     path to epik experiment      (required)";
        cerr << endl;
        cerr << "  -c, --commandfile:    command file                 (required)";
        cerr << endl;
        cerr << "  -d, --density:        [sparse/dense]               (optional)";
        cerr << endl;
        cerr << "  -f, --format:         [ascii/binary/sion]          (optional)";
        cerr << endl;
        cerr << "  -b, --blocksize:      blocksize                    (optional)";
        cerr << endl;
        cerr << "  -h, --help:           prints (this) help message!";
        cerr << endl;
        cerr << endl << endl;
        cerr << "command file includes function, mode and constraints";
        cerr << endl;
        cerr << "function: [count, length, duration, rate]" << endl;
        cerr << "mode: [min, max, avg, sum]" << endl;
        cerr << "***************************************************************";
        cerr << endl << endl;
	}
	MPI_Finalize();
	exit(1);
}

//int opt_check(ifstream &file, char* buffer, funcs* stat_func,
//		             modes* stat_mode, int size) {
//
//}


void create_matrix(double *vector, int rank, int size, int x_block_size,
                funcs stat_func, char* epik_dir, MPI_Comm block_comm,
                MPI_Comm local_root_comm, int block_rank) {
        double local_min, local_max, global_min, global_max;

        // Define receive buffer on all Ranks
        double *recv_buf = NULL;
        // Allocate receive buffer on collecting processes
        if(block_rank==0) {
                recv_buf= new double[size*x_block_size];
        }

        // Root collects all vectors
        MPI_Gather(vector, size,
                   MPI_DOUBLE, recv_buf, size,
                   MPI_DOUBLE, 0, block_comm);

        // Determine global minimum and maximum of the matrix
        if(block_rank==0) {
                local_min = *min_element(&recv_buf[0],&recv_buf[size*x_block_size]);
                local_max = *max_element(&recv_buf[0],&recv_buf[size*x_block_size]);

                MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE,
                           MPI_MIN, 0, local_root_comm);
                MPI_Reduce(&local_max, &global_max, 1, MPI_DOUBLE,
                           MPI_MAX, 0, local_root_comm);

                output(recv_buf, rank, size, x_block_size, stat_func,
                                epik_dir, global_min, global_max,
                                block_comm, local_root_comm);

                output_reduced(recv_buf, rank, size, x_block_size, stat_func,
                                epik_dir, global_min, global_max,
                                block_comm, local_root_comm);

                delete[] recv_buf;
        }
}

void create_sparse_matrix(double *vector, int rank, int size, int x_block_size,
                funcs stat_func, char* epik_dir, MPI_Comm block_comm,
                MPI_Comm local_root_comm, int block_rank) {
        double local_min, local_max, global_min, global_max;
        double *recv_buf_nze = NULL;
        int *recv_buf_pos = NULL;
        // vector for non-zero elements
        double *nze;
        // vector for positions of non-zero elements
        int *pos_nze;
        // number of non-zero elements
        int n_nze = 0;
        // buffer for numbers of non-zero elements -> only used on local_root
        int *n_nzes;
        // displacement vector
        int *displ;
        // sum of numbers of nzes on loacal_roots
        int sum = 0;

        //determine the number of non-zero elements
        //n_nze = size - std::count(&vector[0],&vector[size],0);
        //n_nze = std::count_if(&vector[0],&vector[size],bind2nd(greater<T>(),0));
        for(int i = 0; i < size; ++i) {
                if(vector[i] > 0) ++n_nze;
        }
        // Allocate vectors
  n_nzes = new int[x_block_size];
        displ = new int[x_block_size];
        nze = new double[n_nze];
        pos_nze = new int[n_nze];
        // Collecting non-zero elements
        int j = 0;
        for(int i = 0; i < size; ++i) {
                if(vector[i] != 0) {
                        nze[j] = vector[i];
                        pos_nze[j] = i;
                        ++j;
                }
        }
        // Collect numbers of nzes
        MPI_Gather(&n_nze, 1, MPI_INT, n_nzes, 1, MPI_INT, 0, block_comm);
        // Allocate receive buffer on collecting processes
        if(block_rank==0) {
                displ[0] = 0;
                sum = n_nzes[0];
                for(int i = 1; i < x_block_size; ++i) {
                        sum += n_nzes[i];
                        displ[i] = displ[i-1] + n_nzes[i-1];
                }
                recv_buf_nze= new double[sum];
                recv_buf_pos= new int[sum];
        }
        // Collect elements vectors at local root
        MPI_Gatherv(nze, n_nze, MPI_DOUBLE, recv_buf_nze,
                    n_nzes, displ, MPI_DOUBLE, 0, block_comm);
        // Collect positions vectors at local root
        MPI_Gatherv(pos_nze, n_nze, MPI_INT, recv_buf_pos,
                        n_nzes, displ, MPI_INT, 0, block_comm);

        //Determine global matrix min/max
        if(block_rank==0) {
                local_max = *max_element(&recv_buf_nze[0], &recv_buf_nze[sum]);
                if(sum < size*blocksize) {
                        local_min = 0;
                } else {
                        local_min = *min_element(&recv_buf_nze[0], &recv_buf_nze[sum]);
                }
                MPI_Reduce(&local_min, &global_min,
                           1, MPI_DOUBLE, MPI_MIN, 0, local_root_comm);
                MPI_Reduce(&local_max, &global_max,
                           1, MPI_DOUBLE, MPI_MAX, 0, local_root_comm);


                sparse_output(recv_buf_nze, recv_buf_pos, n_nzes, sum,
                                rank, size, x_block_size, stat_func, epik_dir,
                                global_min, global_max, block_comm, local_root_comm);
                sparse_output_reduced(recv_buf_nze, recv_buf_pos, n_nzes, sum,
                                rank, size, x_block_size, stat_func, epik_dir,
                                global_min, global_max, block_comm, local_root_comm);
                delete[] recv_buf_nze;
                delete[] recv_buf_pos;
        }
        delete[] n_nzes;
        delete[] displ;
        delete[] nze;
        delete[] pos_nze;
}

int main(int argc, char** argv) {
  int rank, size, block_rank, local_root_rank;
  MPI_Comm world, block_comm, local_root_comm;

  MPI_Init(&argc, &argv);
  world = MPI_COMM_WORLD;
  MPI_Comm_rank(world, &rank);
  MPI_Comm_size(world, &size);
  PEARL_mpi_init();
  char *archive;
  char *experiment_name = NULL;

  GlobalDefs* defs = NULL;
  LocalTrace* trace = NULL;
  string inputfile;

  blocksize_glob = size;
  blocksize = size;

  // checking command line parameters
  if(argc<5) {
        printhelp(rank);
  }

  for(int i = 1; i < argc; ++i) {
        if(strcmp(argv[i],"-e")== 0 || strcmp(argv[i],"--experiment")== 0) {
        	++i;
            experiment_name = argv[i];
        } else if(strcmp(argv[i],"-h")== 0 || strcmp(argv[i],"--help")== 0) {
                printhelp(rank);
        } else if(strcmp(argv[i],"-d")== 0 || strcmp(argv[i],"--density")== 0) {
                ++i;
                if(strcmp(argv[i],"sparse")== 0) {
                        sparse_glob = true;
                } else if(strcmp(argv[i],"dense")== 0) {
                        sparse_glob = false;
                } else {
                	printhelp(rank);
                }
        } else if(strcmp(argv[i],"-f")== 0 || strcmp(argv[i],"--format")== 0) {
                ++i;
                if(strcmp(argv[i],"ascii")== 0) {
                        format_glob = ascii;
                } else if(strcmp(argv[i],"binary")== 0) {
                        format_glob = binary;
                } else if(strcmp(argv[i],"sion")== 0) {
                        format_glob = sion;
//  		} else if(strcmp(argv[i],"netcdf")== 0) {
//  			format_glob = netcdf;
                } else {
                	printhelp(rank);
                }
        } else if(strcmp(argv[i],"-b")== 0 || strcmp(argv[i],"--blocksize")== 0) {
                if(!string2val(argv[++i], blocksize_glob)) {
                        printhelp(rank);
                } else if(size % blocksize_glob != 0) {
                        if(rank == 0) {
                                cerr << "Error: Size must be a multiple of Blocksize!" << endl;
                        }
                        delete trace;
                        delete defs;
                        MPI_Finalize();
                        exit(EXIT_FAILURE);

                }
        } else if(strcmp(argv[i],"-c")== 0 ||
                               strcmp(argv[i],"--commandfile")== 0) {
                inputfile = argv[++i];
        } else {
        	printhelp(rank);
        }
  }

  try {
	  archive = epk_archive_set_name(experiment_name);
      if(archive == NULL) {
    	  throw FatalError("Not an EPIK archive!");
      }
      defs = new GlobalDefs(archive);
      trace = new LocalTrace(*defs, archive, (long int)rank);
  } catch (const Error& error) {
      if(rank == 0) {
              cerr << "Error opening experiment!" << endl;
      }
      MPI_Finalize();
      exit(EXIT_FAILURE);
  }

  try {
	  // Initialize PEARL
	  PEARL_verify_calltree(*defs, *trace);
	  PEARL_mpi_unify_calltree(*defs);
	  PEARL_preprocess_trace(*defs, *trace);
  } catch (const Error& error) {
	  if(rank == 0) {
		  cerr << "Error initializing PEARL!" << endl;
	  }
	  MPI_Finalize();
	  exit(EXIT_FAILURE);
  }


  // Initialize Parser
  SalsaParser parser(inputfile, sparse_glob, blocksize_glob, format_glob, size);

  while(parser.parseNext()==1) {

    mc = parser.getMessageChecker();
    blocksize = parser.getBlocksize();
    stat_func = parser.getFunc();
    func = parser.getFuncString();
    stat_mode = parser.getMode();
    mode = parser.getModeString();
    format = parser.getFormat();
    sparse = parser.isSparse();
    constraints = parser.getContraints();
    line_number++;

    CallbackManager* manager = new CallbackManager();
    CallbackDataneu defsneu;
    defsneu.defs=defs;

#ifdef DEBUG
    // Start timer
    if(rank == 0) {
      start_time = MPI_Wtime();
    }
#endif

    SalsaCallback* callbacks;

    callbacks = new SalsaCallback(rank, size, stat_func, stat_mode, mc);
    manager->register_callback(MPI_SEND,
    		PEARL_create_callback(callbacks,&SalsaCallback::send));
    manager->register_callback(MPI_RECV,
    		PEARL_create_callback(callbacks,&SalsaCallback::recv));

    // Replay with registered callbacks
    PEARL_forward_replay(*trace, *manager, &defsneu);

    // Split MPI_COMM_WORLD into block communicators
    int x_block_size = size<blocksize ? size : blocksize;
    MPI_Comm_split(world, rank/x_block_size, rank, &block_comm);
    // Determine rank in the new communicator
    MPI_Comm_rank(block_comm, &block_rank);

    // Generate new communicator consisting of all root-processes
    MPI_Comm_split(world, block_rank, 0, &local_root_comm);
    MPI_Comm_rank(local_root_comm, &local_root_rank);

    double *result = callbacks->get_results();
    if(sparse) {
    	create_sparse_matrix(result, rank, size, x_block_size, stat_func,
    		experiment_name, block_comm, local_root_comm, block_rank);
    } else {
    	create_matrix(result, rank, size, x_block_size, stat_func,
    		experiment_name, block_comm, local_root_comm, block_rank);
    }

    MPI_Barrier(world);

    MPI_Comm_free(&block_comm);
    MPI_Comm_free(&local_root_comm);

    delete callbacks;

#ifdef DEBUG
    // Stop timer and print overall time
    if(rank==0) {
      run_time = MPI_Wtime() - start_time;
      cout << func << " " << mode << ": Complete time= " << run_time << endl;
    }
#endif
    sparse = sparse_glob;
    blocksize = blocksize_glob;
    format = format_glob;
  }

  delete trace;
  delete defs;

  MPI_Finalize();

  return EXIT_SUCCESS;
}
