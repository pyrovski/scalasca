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

#include <iostream>
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::ostream;
  using std::left;
#include <iomanip>
  using std::setw;
#include <fstream>
  using std::ofstream;
#include <string>
  using std::string;
#include <cstdlib>
  using std::exit;
#include <cctype>
  using std::toupper;

string toUpper(const string& str) {
  string up(str);
  for (size_t i=0; i<up.size(); ++i) up[i] = toupper(str[i]);
  return up;
}


void init_stream(ofstream& ofstr, const char* file, const char *cmt = "/") {
  ofstr.open(file);
  if ( ! ofstr ) {
    cerr << "Cannot open " << file << " file" << endl;
    exit(1);
  }
  ofstr << cmt << "* -- automatically generated -- do not change ---------"
                  "--------------------- */\n";
  ofstr << cmt << "* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"
                  "vvvvvvvvvvvvvvvvvvvvv */\n\n";
}

static int sid = 18;

void gen_name_define(ofstream& def1, ofstream& def2, const string& name,
                     const string& uname, const string& type = "EPK_FUNCTION") {
  def1 << "#define EPK__" << setw(30) << left << uname << setw(4) << ++sid;
  def1 << endl;

  def2 << "epk_shm_regid[EPK__" << uname << "] =\n";
  def2 << "  esd_def_region(\"" << name;
  def2 << "\", fid, ELG_NO_LNO, ELG_NO_LNO, \"SHMEM\", " << type << ");\n";
}

void gen_prolog(ostream& wrp, const string& uname, const string& extra = "") {
  wrp << "{\n";
  wrp << extra;
  wrp << "#ifndef EPK_CSITE_INST\n";
  wrp << "  esd_enter(epk_shm_regid[EPK__" << uname << "]);\n";
  wrp << "#endif\n";
}

void gen_epilog(ostream& wrp, const string& uname, const string& extra = "") {
  wrp << "#ifndef EPK_CSITE_INST\n";
  wrp << "  esd_exit(epk_shm_regid[EPK__" << uname << "]);\n";
  wrp << "#endif\n";
  wrp << extra;
  wrp << "}\n";
}

void gen_coll_prolog(ostream& wrp, const string& uname) {
  wrp << "{\n";
  wrp << "#ifndef EPK_CSITE_INST\n";
  wrp << "  esd_enter(epk_shm_regid[EPK__" << uname << "]);\n";
  wrp << "#endif\n";
  wrp << "  TRACE_OFF();\n";
}

void gen_coll_epilog(ostream& wrp, const string& uname, const char* root = "ELG_NO_ID, ") {
  /*??? sent, recvd*/
  wrp << "  TRACE_ON();\n";
  wrp << "  esd_collexit(epk_shm_regid[EPK__" << uname << "], " << root
      << "epk_shmem_create_com(PE_start, logPE_stride, PE_size), 0, 0);\n";
  wrp << "}\n";
}

void F_gen_coll_epilog(ostream& wrp, const string& uname, const char* root = "ELG_NO_ID, ") {
  /*??? sent, recvd*/
  wrp << "  TRACE_ON();\n";
  wrp << "  esd_collexit(epk_shm_regid[EPK__" << uname << "], " << root
      << "epk_shmem_create_com(*PE_start, *logPE_stride, *PE_size), 0, 0);\n";
  wrp << "}\n";
}

enum trans_t { TRANS_GET, TRANS_PUT };

void gen_transfer_wrapper(ostream& wrp, const string& name, const string& uname,
    trans_t t, const char* type, const char* extra1, const char* extra2,
    const string& size) {
  wrp << "\nvoid " << name << "(" << type << "* target, "
      << type << "* source," << extra1 << " int len, int pe)\n";
  gen_prolog(wrp, uname);
  if ( t == TRANS_GET ) {
    wrp << "  esd_get_1ts_remote(pe, EPK_NEXT_SHM_ID, len*" << size << ");\n";
    wrp << "  p" << name << "(target, source," << extra2 << " len, pe);\n";
    wrp << "  esd_get_1te(pe, EPK_CURR_SHM_ID);\n";
  } else if ( t == TRANS_PUT ) {
    wrp << "  esd_put_1ts(pe, EPK_NEXT_SHM_ID, len*" << size << ");\n";
    wrp << "  p" << name << "(target, source," << extra2 << " len, pe);\n";
    wrp << "  esd_put_1te_remote(pe, EPK_CURR_SHM_ID);\n";
  }
  gen_epilog(wrp, uname);
}

void F_gen_transfer_wrapper(ostream& wrp, const string& name,
    const string& uname, trans_t t, const char* extra1,
    const char* extra2, const string& size) {
  wrp << "\nvoid " << name << "(void* target, void* source,"
      << extra1 << " int* len, int* pe)\n";
  gen_prolog(wrp, uname);
  if ( t == TRANS_GET ) {
    wrp << "  esd_get_1ts_remote(*pe, EPK_NEXT_SHM_ID, (*len)*" << size << ");\n";
    wrp << "  p" << name << "(target, source," << extra2 << " len, pe);\n";
    wrp << "  esd_get_1te(*pe, EPK_CURR_SHM_ID);\n";
  } else if ( t == TRANS_PUT ) {
    wrp << "  esd_put_1ts(*pe, EPK_NEXT_SHM_ID, (*len)*" << size << ");\n";
    wrp << "  p" << name << "(target, source," << extra2 << " len, pe);\n";
    wrp << "  esd_put_1te_remote(*pe, EPK_CURR_SHM_ID);\n";
  }
  gen_epilog(wrp, uname);
}

void gen_sput_wrapper(ostream& wrp, const string& name, const string& uname,
    const char* type, const char* extra, const string& size) {
  wrp << "\nvoid " << name << "(" << type << "* target, ";
  if ( extra[0] ) wrp << type << " " << extra << ", ";
  wrp << "int pe)\n";
  gen_prolog(wrp, uname);
  wrp << "  esd_put_1ts(pe, EPK_NEXT_SHM_ID, " << size << ");\n";
  wrp << "  p" << name << "(target, ";
  if ( extra[0] ) wrp << extra << ", ";
  wrp << "pe);\n";
  wrp << "  esd_put_1te_remote(pe, EPK_CURR_SHM_ID);\n";
  gen_epilog(wrp, uname);
}

enum sget_t { SG_G, SG_FADD, SG_FINC, SG_SWAP, SG_CSWAP };

void gen_sget_wrapper(ostream& wrp, const string& name, const string& uname,
    sget_t mode, const char* type, const char* extra1, const char* extra2,
    const string& size) {
  wrp << "\n" << type << " " << name << "(" << type << "* target, ";
  if ( extra1[0] ) wrp << type << " " << extra1 << ", ";
  if ( extra2[0] ) wrp << type << " " << extra2 << ", ";
  wrp << "int pe)\n";
  gen_prolog(wrp, uname, ("  " + string(type) + " res;\n\n"));
  wrp << "  esd_get_1ts_remote(pe, EPK_NEXT_SHM_ID, " << size << ");\n";
  wrp << "  res = p" << name << "(target, ";
  if ( extra1[0] ) wrp << extra1 << ", ";
  if ( extra2[0] ) wrp << extra2 << ", ";
  wrp << "pe);\n";
  wrp << "  esd_get_1te(pe, EPK_CURR_SHM_ID);\n";
  if ( mode == SG_CSWAP ) {
    wrp << "  if ( res == cond ) {\n";
    wrp << "    esd_put_1ts(pe, EPK_NEXT_SHM_ID, " << size << ");\n";
    wrp << "    esd_put_1te_remote(pe, EPK_CURR_SHM_ID);\n";
    wrp << "  };\n";
  } else if ( mode != SG_G ) {
    wrp << "  esd_put_1ts(pe, EPK_NEXT_SHM_ID, " << size << ");\n";
    wrp << "  esd_put_1te_remote(pe, EPK_CURR_SHM_ID);\n";
  }
  gen_epilog(wrp, uname, "\n  return res;\n");
}

void gen_wait_wrapper(ostream& wrp, const string& name, const string& uname,
    const char* type, const char* extra) {
  wrp << "\nvoid " << name << "(" << type << "* ivar, ";
  if ( extra[0] ) wrp << "int " << extra << ", ";
  wrp << type << " value)\n";
  gen_prolog(wrp, uname);
  wrp << "  p" << name << "(ivar, ";
  if ( extra[0] ) wrp << extra << ", ";
  wrp << "value);\n";
  gen_epilog(wrp, uname);
}

void gen_reduce_wrapper(ostream& wrp, const string& name, const string& uname,
    const char* type, const string& size) {
  wrp << "\nvoid " << name << "(" << type << "* target, " << type
      << "* source, int nreduce, int PE_start, int logPE_stride, int PE_size, "
      << type << "* pWrk, long* pSync)\n";
  gen_coll_prolog(wrp, uname);
  wrp << "  p" << name << "(target, source, nreduce, PE_start, logPE_stride, PE_size);\n";
  gen_coll_epilog(wrp, uname);
}

void gen_collect_wrapper(ostream& wrp, const string& name, const string& uname,
    const char* cextra1, const char* cextra2, const char* size) {
  wrp << "\nvoid " << name << "(void* target, void* source, int nlong, "
      << cextra1 << "int PE_start, int logPE_stride, int PE_size,"
      << " long* pSync)\n";
  gen_coll_prolog(wrp, uname);
  wrp << "  p" << name << "(target, source, nlong, " << cextra2
      << "PE_start, logPE_stride, PE_size, pSync);\n";
  gen_coll_epilog(wrp, uname, (cextra2[0] ? cextra2 : "ELG_NO_ID, "));
}

void F_gen_collect_wrapper(ostream& wrp, const string& name, const string& uname,
    const char* cextra1, const char* cextra2, const char* size) {
  wrp << "\nvoid " << name << "(void* target, void* source, int* nlong, "
      << cextra1 << "int* PE_start, int* logPE_stride, int* PE_size,"
      << " long* pSync)\n";
  gen_coll_prolog(wrp, uname);
  wrp << "  p" << name << "(target, source, nlong, "
      << (cextra2[0] ? (cextra2+1) : "")
      << "PE_start, logPE_stride, PE_size, pSync);\n";
  F_gen_coll_epilog(wrp, uname, (cextra2[0] ? cextra2 : "ELG_NO_ID, "));
}

const int TYPE_MAX = 9;
const char* types1[TYPE_MAX] = {
  "short", "int", "long", "longlong",
  "float", "double", "longdouble", "complexf", "complexd"
};
const char* types2[TYPE_MAX] = {
  "short", "int", "long", "long long",
  "float", "double", "long double", "void", "void"
};
bool is_int_type[TYPE_MAX] =  {
  true, true, true, true, 
  false, false, false, false, false
};

bool is_cmplx_type[TYPE_MAX] =  {
  false, false, false, false,
  false, false, false, true, true
};

const int FTYPE_MAX = 6;
const char* ftypes[FTYPE_MAX] = {
  "character", "logical", "integer", "real", "double", "complex"
};
const char* fsizes[FTYPE_MAX] = { "1", "4", "4", "4", "8", "8" }; /* ??? */

const int PRX_MAX = 3;
const char* prefixes[PRX_MAX] = { "", "i",                  "ix" };
const char* extras1[PRX_MAX]  = { "", " int tst, int sst,", " int* idx," };
const char* extras2[PRX_MAX]  = { "", " tst, sst,",         " idx," };

const int MODE_MAX = 2;
const char* modes1[MODE_MAX] = { "get", "put" };
trans_t     modes2[MODE_MAX] = { TRANS_GET, TRANS_PUT };

const int SP_GET_MAX = 5;
const char* specialgets[SP_GET_MAX] = { "g", "fadd",  "finc", "swap", "cswap" };
const char* sget_extra1[SP_GET_MAX] = { "",  "value", "",     "value", "cond" };
const char* sget_extra2[SP_GET_MAX] = { "",  "",      "",     "",     "value" };
bool sget_allows_real[SP_GET_MAX] = { true,  false,   false,  true,    false };
sget_t sget_mode[SP_GET_MAX]    = { SG_G, SG_FADD, SG_FINC, SG_SWAP, SG_CSWAP };

const int SP_PUT_MAX = 3;
const char* specialputs[SP_PUT_MAX] = { "p", "add", "inc" };
const char* sput_extra[SP_PUT_MAX] = { "value", "value", "" };
bool sput_allows_real[SP_PUT_MAX] = { true, false, false };

const int WAIT_MAX = 2;
const char* wnames[WAIT_MAX] = { "wait", "wait_until" };
const char* wextra[WAIT_MAX] = { "",     "cmp" };

const int REDUC_MAX = 7;
const char* reducs[REDUC_MAX] = {
  "min", "max", "and", "or", "xor", "sum", "prod"
};
bool reduc_allows_real[REDUC_MAX] = {
  true, true, false, false, false, true, true
};

const int BSIZE_MAX = 6;
const char* szname[BSIZE_MAX]  = { "", "32", "64", "128", "4", "8" };
const char* szbytes[BSIZE_MAX] = { "8", "4",  "8",  "16", "4", "8" };
bool sz_c_only[BSIZE_MAX] = { true, true, true, true, false, false };

const int COLL_MAX = 3;
const char* collects[COLL_MAX] = { "broadcast",      "collect", "fcollect" };
const char* cextra1[COLL_MAX]  = { "int PE_root, ",  "",        "" };
const char* cextra2[COLL_MAX]  = { "PE_root, ",      "",        "" };
const char* fextra1[COLL_MAX]  = { "int* PE_root, ", "",        "" };
const char* fextra2[COLL_MAX]  = { "*PE_root, ",     "",        "" };
const char* colltype[COLL_MAX] = { "EPK_FUNCTION_COLL_ONE2ALL",
                                   "EPK_FUNCTION_COLL_ALL2ONE",
                                   "EPK_FUNCTION_COLL_ALL2ONE" };

int main() {
  ofstream def1, def2, wrp, fwrp, cols("SHMEM_COLLS");
  init_stream(def1, "epk_shmem.c.reg1.gen");
  init_stream(def2, "epk_shmem.c.reg2.gen");
  init_stream(wrp,  "epk_shmem.c.wrapper.gen");
  init_stream(fwrp, "epk_fshmem.c.wrapper.gen");

  cols << "\"shmem_init\",\n";
  cols << "\"shmem_finalize\",\n";
  cols << "\"barrier\",\n";
  cols << "\"shmem_barrier_all\",\n";
  cols << "\"shmalloc\",\n";
  cols << "\"shfree\",\n";

  for (int t = 0; t<TYPE_MAX; ++t) {
    string size = (types2[t][0] == 'v') ? 
               ((types1[t][7] == 'f') ? "2*sizeof(float)" : "2*sizeof(double)")
               : "sizeof(" + string(types2[t]) + ")";

    // shmem_<TYPE>_<PRX>get/put
    for (int p = 0; p<PRX_MAX; ++p) {
      for (int m = 0; m<MODE_MAX; ++m) {
        string name = string("shmem_")
               + types1[t] + "_" + prefixes[p] + modes1[m];
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname);
        gen_transfer_wrapper(wrp, name, uname, modes2[m], types2[t],
                             extras1[p], extras2[p], size);
      }
    }

    // shmem_<TYPE>_<SPUT>
    for (int p = 0; p<SP_PUT_MAX; ++p) {
      if ( is_int_type[t] || (sput_allows_real[p] && !is_cmplx_type[t]) ) {
        string name = string("shmem_") + types1[t] + "_" + specialputs[p];
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname);
        gen_sput_wrapper(wrp, name, uname, types2[t], sput_extra[p], size);
      }
    }

    // shmem_<TYPE>_<SGET>
    for (int g = 0; g<SP_GET_MAX; ++g) {
      if ( is_int_type[t] || (sget_allows_real[g] && !is_cmplx_type[t]) ) {
        string name = string("shmem_") + types1[t] + "_" + specialgets[g];
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname);
        gen_sget_wrapper(wrp, name, uname, sget_mode[g], types2[t],
                         sget_extra1[g], sget_extra2[g], size);
      }
    }

    // shmem_<TYPE>_<WAIT>
    for (int w = 0; w<WAIT_MAX; ++w) {
      if ( is_int_type[t] ) {
        string name = string("shmem_") + types1[t] + "_" + wnames[w];
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname);
        gen_wait_wrapper(wrp, name, uname, types2[t], wextra[w]);
      }
    }

    // shmem_<TYPE>_<REDUC>_to_all
    for (int r = 0; r<REDUC_MAX; ++r) {
      if ( is_int_type[t] || reduc_allows_real[r] ) {
        string name = string("shmem_") + types1[t] + "_" + reducs[r]
                    + "_to_all";
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname, "EPK_FUNCTION_COLL_ALL2ALL");
        gen_reduce_wrapper(wrp, name, uname, types2[t], size);
        cols << "\"" << name << "\",\n";
      }
    }
  }

  for (int s = 0; s<BSIZE_MAX; ++s) {
    // shmem_<PRX>get/put<BSIZE>
    if ( sz_c_only[s] ) {
      for (int p = 0; p<PRX_MAX; ++p) {
        for (int m = 0; m<MODE_MAX; ++m) {
          string name = string("shmem_") + prefixes[p] + modes1[m] + szname[s];
          string uname(toUpper(name));
          gen_name_define(def1, def2, name, uname);
          gen_transfer_wrapper(wrp, name, uname, modes2[m], "void",
                               extras1[p], extras2[p], szbytes[s]);
        }
      }

      // shmem_<COLL><BSIZE>
      for (int c = 0; c<COLL_MAX; ++c) {
        string name = string("shmem_") + collects[c] + szname[s];
        string uname(toUpper(name));
        gen_name_define(def1, def2, name, uname, colltype[c]);
        gen_collect_wrapper(wrp, name, uname, cextra1[c], cextra2[c],
                            szbytes[s]);
        cols << "\"" << name << "\",\n";
      }
    }
  }

  /*
   * F O R T R A N
   */

  for (int t = 0; t<FTYPE_MAX; ++t) {
    // FORTRAN shmem_<TYPE>_<PRX>get/put
    for (int m = 0; m<MODE_MAX; ++m) {
      string name = string("shmem_") + ftypes[t] + "_" + modes1[m];
      string uname(toUpper(name));
      if ( ftypes[t] != "double" ) gen_name_define(def1, def2, name, uname);
      F_gen_transfer_wrapper(fwrp, name, uname, modes2[m], "", "", fsizes[t]);
    }
  }

  for (int s = 0; s<BSIZE_MAX; ++s) {
    // shmem_<PRX>get/put<BSIZE>
    for (int m = 0; m<MODE_MAX; ++m) {
      string name = string("shmem_") + modes1[m] + szname[s];
      string uname(toUpper(name));
      if ( ! sz_c_only[s] ) gen_name_define(def1, def2, name, uname);
      F_gen_transfer_wrapper(fwrp, name, uname, modes2[m], "", "", szbytes[s]);
    }

    // shmem_<COLL><BSIZE>
    for (int c = 0; c<COLL_MAX; ++c) {
      string name = string("shmem_") + collects[c] + szname[s];
      string uname(toUpper(name));
      if ( ! sz_c_only[s] )
	gen_name_define(def1, def2, name, uname, colltype[c]);
      F_gen_collect_wrapper(fwrp, name, uname, fextra1[c], fextra2[c],
                            szbytes[s]);
      cols << "\"" << name << "\",\n";
    }
  }

  def1 << "\n#define EPK__SHMEM_MAX " << ++sid << "\n";
}
