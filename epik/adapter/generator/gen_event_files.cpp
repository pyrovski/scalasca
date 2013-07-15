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

/* EPILOG currently doesn't record all MPI event parameters in traces
   and until it does, the reader and subsequent handler routines need
   to be customised not to expect all of the MPI event parameters:
   MPI_COLLEXIT events are missing the "rid" exiting routine identifier,
   MPI_RECV events are missing the number of bytes "recvd" parameter.
#define EPILOG_ALL_PARAMS 1
*/

#include <iostream>
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::ostream;
  using std::istream;
  using std::left;
#include <iomanip>
  using std::setw;
#include <fstream>
  using std::ifstream;
  using std::ofstream;
#include <string>
  using std::string;
  using std::getline;
#include <vector>
  using std::vector;
#include <cstdlib>
  using std::exit;
#include <cctype>
  using std::tolower;

struct Field {
  string type;
  string fname;
  string descr;
};

class Event {
public:
  Event(const string& n, int nf, const string& c)
       : name(n), sect(c), fields(nf) {}
  string name;
  string sect;
  vector<Field> fields;
};

void skip_line(istream& es) {
  es.ignore(999999, '\n');
}

void read_spec(vector<Event*>& events) {
  ifstream es("EVENT.SPEC");

  if (!es) {
    cerr << "Cannot open EVENT.SPEC file" << endl;
    exit(1);
  }

  string line, section, name;
  int nfields;

  while ( getline(es, line) ) {
    if ( line[0] == '@' ) {
      // -- new section
      section = line.substr(2);
    } else if ( line[0] != '#' && line[0] != '\0' ) {
      // -- new event
      name = line;
      es >> nfields;
      skip_line(es);
      Event* event = new Event(name, nfields, section);
      for (int i=0; i<nfields; ++i) {
        Field field;
        getline(es, event->fields[i].type);
        getline(es, event->fields[i].fname);
        getline(es, event->fields[i].descr);
      }
      events.push_back(event);
      section = "";
    }
  }
}

bool gen_func_head(ostream& ostr, const string& func, const string& prefix,
                   vector<Field>& fields, const string& suffix,
                   const string& mode = "") {
  bool has_metrics = false;

  // in "gen" and "trc" mode ignore first two fields
  int start = ( mode == "gen" || mode == "trc" ? 2 : 0 );

  // print func and optional prefix
  int len = func.size() + 1 + prefix.size();
  ostr << func << "(" << prefix;
  if ( prefix[0] && (fields.size()-start) > 0 ) {
    ostr << ",";
    ++len;
  }

  // print fields
  bool notfirst = prefix[0];
  int special = ( mode == "trc" && fields[fields.size()-1].fname == "metv[]" );
#ifndef EPILOG_ALL_PARAMS
  if ( func.find("_MPI_RECV") != string::npos &&
       func.find("ElgGen_write_") == string::npos )
      special = 1;
#endif
  for (size_t f=start; f<fields.size(); ++f) {
    Field& F = fields[f];

#ifndef EPILOG_ALL_PARAMS
    if ( F.fname == "recvd" && func.find("_MPI_RECV") != string::npos &&
                               func.find("ElgGen_write_") == string::npos )
        continue;
    if ( F.fname == "rid" && func.find("_MPI_COLLEXIT") != string::npos &&
                             func.find("ElgGen_write_") == string::npos )
        continue;
#endif

    // special handling for metrics
    string extra;
    if ( F.fname == "metv[]" ) {
      extra = "elg_ui1 metc, ";
      has_metrics = true;
      if ( mode == "trc" ) continue;
    }

    // break lines longer than 80 chars
    int arglen = F.type.size() + F.fname.size() + 3 + extra.size();
    if ( (len + arglen) > 80 ) {
      ostr << endl << "       ";
      len = 7;
    } else if ( notfirst ) {
      ostr << " ";
    } else {
      notfirst = true;
    }

    // print field and separator
    if ( ( mode == "gen" || mode == "trc" ) && F.fname.find("lid") == 1 )
      ostr << extra << F.type << " " << F.fname[0] << "pid";
    else
      ostr << extra << F.type << " " << F.fname;
    if ( f != (fields.size()-1-special) || suffix[0] )
      ostr << ",";
    len += arglen;
  }

  // print optional suffix
  if ( suffix[0] ) {
    if ( (len + suffix.size() + 3) > 80 ) ostr << endl << "      ";
    ostr << " " << suffix << ")";
  } else {
    ostr << ")";
  }

  return has_metrics;
}

void process_fields(ostream& ostr, const char* prefix, vector<Field>& fields,
                    const char* suffix, const char* suffixLast = 0) {
  for (size_t f=0; f<fields.size(); ++f) {
    ostr << prefix << fields[f].fname;
    if ( f == fields.size()-1 && suffixLast ) ostr << suffixLast;
    else ostr << suffix;
    ostr << endl;
  }
}

void gen_gen_write(ostream& ostr, const char* prefix, const string& event,
                   vector<Field>& fields, bool no_metrics) {
  ostr << prefix << "  ElgGen_write_" << event
       << "(ELGTHRD_GEN(thrdv[ELG_MY_THREAD])";
  for (size_t f=2; f<fields.size(); ++f) {
    Field& F = fields[f];
    if ( F.fname.find("lid") == 1 ) {
      ostr << ", " << F.fname[0] << "pid";
    } else if ( F.fname == "metv[]" ) {
      if ( no_metrics ) ostr << ", 0, NULL";
      else ostr << ",\n             num_metrics, ELGTHRD_VALV(thrdv[ELG_MY_THREAD])";
    } else {
      ostr << ", " << F.fname;
    }
  }
  ostr << ");\n";
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
                  "vvvvvvvvvvvvvvvvvvvvv */\n";
}

string toLower(const string& str) {
  string low(str);
  for (size_t i=0; i<low.size(); ++i) low[i] = tolower(str[i]);
  return low;
}

string gen_tex(const string& str) {
  string res;
  for (size_t i=0; i<str.size(); ++i) {
    if ( str[i] == '_' ) res.push_back('\\');
    res.push_back(str[i]);
  }
  return res;
}

void process(vector<Event*>& events) {
  ofstream genh, genc, rwh, rwc, trch, trcc, rcbh, rcbc, rcbd, prt, anal, tex;
  init_stream(genh, "elg_gen.h.gen");
  init_stream(genc, "elg_gen.c.gen");
  init_stream(rwh,  "elg_rw.h.gen");
  init_stream(rwc,  "elg_rw.c.gen");
  init_stream(trch, "elg_trc.h.gen");
  init_stream(trcc, "elg_trc.c.gen");
  init_stream(rcbh, "elg_readcb.h.gen");
  init_stream(rcbc, "elg_readcb.c.gen");
  init_stream(rcbd, "elg_readcb_dummies.c.gen");
  init_stream(prt,  "elg_print.c.gen");
#if 0
  init_stream(anal, "elg_anal.c.gen");
#endif
  init_stream(tex, "epilog.gen.tex", "%");

  for (size_t n=0; n<events.size(); ++n) {
    Event* E = events[n];
    if ( E->sect[0] ) cout << "\n/* -- " << E->sect << " -- */" << endl;
    cout << E->name << endl;

    if ( E->name.find("_1TO") == string::npos ) {
    // ignore for 1TO events

    // --- elg_gen.h ------------------------------------------------------
    if ( E->sect[0] ) genh << "\n/* -- " << E->sect << " -- */" << endl;
    genh << endl;
    gen_func_head(genh, "EXTERN void ElgGen_write_" + E->name, "ElgGen* gen",
                  E->fields, "", "gen");
    genh << ";" << endl;

    // --- elg_gen.c ------------------------------------------------------
    if ( E->sect[0] ) genc << "\n/* -- " << E->sect << " -- */" << endl;
    genc << endl;
    gen_func_head(genc, "void ElgGen_write_" + E->name, "ElgGen* gen",
                  E->fields, "", "gen");
    genc << " {\n";
    genc << "  elg_ui1 type   = ELG_" << E->name << ";\n";
    genc << "  elg_d8  time   = elg_pform_wtime();\n";
    genc << "  elg_ui1 length =\n";
    for (size_t f=1; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      if ( F.fname.find("lid") == 1 ) {
        genc << "    sizeof(" << F.fname[0] << "pid";
      } else if ( F.fname == "metv[]" ) {
        genc << "    (metc ? metc * sizeof(elg_ui8) : 0";
#ifndef EPILOG_ALL_PARAMS
      } else if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) {
        genc << "  /*sizeof(" << F.fname << ")*/ (0";
      } else if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) {
        genc << "  /*sizeof(" << F.fname << ")*/ (0";
#endif
      } else {
        genc << "    sizeof(" << F.fname;
      }
      if ( f == E->fields.size()-1 ) genc << ");"; else genc << ") +";
      genc << endl;
    }
    genc << endl;
    genc << "  ELGGEN_CHECK(gen);\n";
    if ( E->name == "ENTER_TRACING" )
      genc << "  /* NB: No ELGGEN_ALLOC since space reserved at buffer creation */\n";
    else
      genc << "  ELGGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);\n";
    genc << endl;
    genc << "  ELGGEN_COPY_NUM(gen, length);\n";
    genc << "  ELGGEN_COPY_NUM(gen, type);\n";
    for (size_t f=1; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      if ( F.fname.find("lid") == 1 ) {
        genc << "  ELGGEN_COPY_NUM(gen, " << F.fname[0] << "pid);" << endl;
      } else if ( F.fname == "metv[]" ) {
        genc << "  { int i;\n";
        genc << "    for (i = 0; i < metc; i++)\n";
        genc << "      {\n";
        genc << "        ELGGEN_COPY_NUM(gen, metv[i]);\n";
        genc << "      }\n";
        genc << "  }\n";
#ifndef EPILOG_ALL_PARAMS
      } else if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) {
        genc << "/*ELGGEN_COPY_NUM(gen, " << F.fname << ");*/" << endl;
      } else if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) {
        genc << "/*ELGGEN_COPY_NUM(gen, " << F.fname << ");*/" << endl;
#endif
      } else {
        genc << "  ELGGEN_COPY_NUM(gen, " << F.fname << ");" << endl;
      }
    }
    genc << "}" << endl;

    // --- elg_trc.h ------------------------------------------------------
    if ( E->sect[0] ) trch << "\n/* -- " << E->sect << " -- */" << endl;
    trch << endl;
    gen_func_head(trch, "EXTERN void elg_" + toLower(E->name), "", E->fields,
                  "", "trc");
    trch << ";" << endl;

    // --- elg_trc.c ------------------------------------------------------
    if ( E->sect[0] ) trcc << "\n/* -- " << E->sect << " -- */" << endl;
    trcc << endl;
    bool has_metrics = gen_func_head(trcc, "void elg_" + toLower(E->name),
                                     "", E->fields, "", "trc");
    trcc << " {\n";
    trcc << "  elg_check_thrd_id(ELG_MY_THREAD);\n";
    if ( has_metrics ) {
      trcc << "#if defined(ELG_METR)\n";
      trcc << "  if ( num_metrics > 0 )\n";
      trcc << "    {\n";
      trcc << "      elg_metric_read(ELGTHRD_METV(thrdv[ELG_MY_THREAD]),\n";
      trcc << "                      ELGTHRD_VALV(thrdv[ELG_MY_THREAD]));\n";
      gen_gen_write(trcc, "    ", E->name, E->fields, false);
      trcc << "    }\n";
      trcc << "  else\n";
      trcc << "    {\n";
      gen_gen_write(trcc, "    ", E->name, E->fields, true);
      trcc << "    }\n";
      trcc << "#else\n";
    }
    if ( E->name == "OMP_FORK" || E->name == "OMP_JOIN" ) {
      trcc << "#if (defined (ELG_OMPI) || defined (ELG_OMP))\n";
      trcc << "  if (!omp_in_parallel())\n";
      gen_gen_write(trcc, "  ", E->name, E->fields, true);
      trcc << "#endif\n";
    } else {
      gen_gen_write(trcc, "", E->name, E->fields, true);
    }
    if ( has_metrics )  trcc << "#endif\n";
    trcc << "}" << endl;

    } // end ignore for 1TO events

    // -- do not include remote events below
    // if ( E->name.find("_REMOTE") != string::npos ) continue;

    // --- elg_rw.h -------------------------------------------------------
    if ( E->sect[0] ) rwh << "\n/* -- " << E->sect << " -- */" << endl;
    rwh << endl;
    gen_func_head(rwh, "EXTERN int ElgOut_write_" + E->name, "ElgOut* out",
                  E->fields, "");
    rwh << ";" << endl;

    // --- elg_rw.c -------------------------------------------------------
    if ( E->sect[0] ) rwc << "\n/* -- " << E->sect << " -- */" << endl;
    rwc << endl;
    gen_func_head(rwc, "int ElgOut_write_" + E->name, "ElgOut* out", E->fields,
                  "");
    rwc << " {\n";
    rwc << "  ElgRec* rec = out->rec;\n";
    rwc << "  ElgRec_reset_type(rec, ELG_" << E->name << ");\n";
    for (size_t f=0; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "lid" ) {
        rwc << "  if (lid != ELG_NO_ID)\n";
        rwc << "    ELGREC_WRITE_VAR(rec, lid);\n";
      } else if ( F.fname == "metv[]" ) {
        rwc << "  { int i;\n";
        rwc << "    for (i = 0; i < metc; i++)\n";
        rwc << "      {\n";
        rwc << "        ELGREC_WRITE_VAR(rec, metv[i]);\n";
        rwc << "      }\n";
        rwc << "  }\n";
      } else {
        rwc << "  ELGREC_WRITE_VAR(rec, " << F.fname << ");\n";
      }
    }
    rwc << "  rec->length = rec->pos - rec->body;\n";
    rwc << "  ElgOut_write_record(out,rec);\n";
    rwc << "  return rec->length + sizeof(rec->type) + sizeof(rec->length);\n";
    rwc << "}" << endl;

    // --- elg_readcb.h ---------------------------------------------------
    if ( E->sect[0] ) rcbh << "\n/* -- " << E->sect << " -- */" << endl;
    rcbh << endl;
    rcbh << "EXTERN void elg_readcb_" << E->name << "(\n" << left;
    for (size_t f=0; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "metv[]" ) {
        rcbh << "                      elg_ui1 metc,      ";
        rcbh << "/* number of metrics                */\n";
      }
      rcbh << "                      " << setw(7) << F.type;
      rcbh << " " << F.fname << ",";
      rcbh << setw(10-F.fname.size()) << " ";
      rcbh << "/* " << setw(32) << F.descr << " */\n";
    }
    rcbh << "                      void* userdata);" << endl;

    // --- elg_readcb.c ---------------------------------------------------
    bool hasMetrics = false;
    if ( E->sect[0] ) rcbc << "\n/* -- " << E->sect << " -- */" << endl;
    rcbc << endl;
    rcbc << "    case ELG_" << E->name << " :\n";
    rcbc << "      {\n";
    for (size_t f=0; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "metv[]" ) {
        rcbc << "        int i;\n";
        hasMetrics = true;
      } else
        rcbc << "        " << F.type << " " << F.fname << ";\n";
    }
    rcbc << "\n" << left;
    if (hasMetrics) {
      rcbc << "        if ( handle->metc == 255) {\n";
      rcbc << "          handle->metc = ( length";
      int len = 33;
      for (size_t f=1; f<E->fields.size(); ++f) {
        Field& F = E->fields[f];
        if ( F.fname == "metv[]" )
          continue;
        int arglen = F.fname.size() + 11;
        if ( len + arglen > 80 ) {
          rcbc << "\n                                 ";
          len = 33;
        }
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
        rcbc << " - sizeof(" << F.fname << ")";
        len += arglen;
      }
      rcbc << " )\n";
      rcbc << "                           / sizeof(elg_ui8);\n";
      rcbc << "          if (handle->metc > 0) {\n";
      rcbc << "            handle->metv = realloc(handle->metv, handle->metc * sizeof(elg_ui8));\n";
      rcbc << "            if (handle->metv == NULL)\n";
      rcbc << "              elg_error_msg(\"Can't allocate metric data buffer\");\n";
      rcbc << "          }\n";
      rcbc << "        }\n";
    }
    rcbc << "        if ( length == sizeof(lid)";
    int len = 34;
    for (size_t f=1; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      int arglen = ( F.fname == "metv[]" ? 35 : F.fname.size() + 11 );
      if ( (len + arglen) > 80 ) {
        rcbc << "\n                                  ";
        len = 34;
      }
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "metv[]" )
        rcbc << " + (handle->metc * sizeof(elg_ui8))";
      else
        rcbc << " + sizeof(" << F.fname << ")";
      len += arglen;
    }
    rcbc << " )\n";
    rcbc << "          lid  = ElgRec_read_ui4(rec);\n";
    rcbc << "        else\n";
    rcbc << "          lid  = handle->single_lid;\n";
    for (size_t f=1; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "metv[]" ) {
        rcbc << "        for (i = 0; i < handle->metc; i++)\n";
        rcbc << "          handle->metv[i] = ElgRec_read_ui8(rec);\n";
      } else {
        rcbc << "        " << setw(6) << F.fname << " = ElgRec_read_";
        rcbc << F.type.substr(4) << "(rec);\n";
      }
    }
    rcbc << "\n";
    rcbc << "        elg_readcb_" << E->name << "(lid, time,";
    len = E->name.size() + 30;
    for (size_t f=2; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      int arglen = ( F.fname == "metv[]" ? 28 : F.fname.size() + 2 );
      if ( (len + arglen) > 80 ) {
        rcbc << "\n                  ";
        len = 18;
      }
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      if ( F.fname == "metv[]" )
        rcbc << " handle->metc, handle->metv,";
      else
        rcbc << " " << F.fname << ",";
      len += arglen;
    }
    if ( len > 69 )
      rcbc << "\n                   userdata);\n";
    else
      rcbc << " userdata);\n";
    rcbc << "        break;\n";
    rcbc << "      }\n";

    // --- elg_readcb_dummies.c -------------------------------------------
    if ( E->sect[0] ) rcbd << "\n/* -- " << E->sect << " -- */" << endl;
    rcbd << endl;
    gen_func_head(rcbd, "void elg_readcb_" + E->name, "", E->fields,
                  "void* userdata");
    rcbd << " {\n";
    rcbd << "}" << endl;

    // --- elg_print.c ----------------------------------------------------
    if ( E->sect[0] ) prt << "\n/* -- " << E->sect << " -- */" << endl;
    prt << endl;
    bool has_metrics = gen_func_head(prt, "void elg_readcb_" + E->name,
                                     "", E->fields, "void* userdata");
    prt << " {" << left << endl;
    prt << "  printf(\"%-20s\", \"<" << E->name << ">\");\n";
    for (size_t f=0; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      if ( F.fname == "metv[]" ) {
        continue;
      }
#ifndef EPILOG_ALL_PARAMS
      if ( ( F.fname == "recvd" ) && ( E->name == "MPI_RECV" ) ) continue;
      if ( ( F.fname == "rid" ) && ( E->name == "MPI_COLLEXIT" ) ) continue;
#endif
      prt << "  printf(\"  " << setw(6) << F.fname << " : ";
      if ( F.type == "elg_ui4" || F.type == "elg_ui1" )
        prt << "%3u";
      else if ( F.type == "elg_d8" )
        prt << "%10.6f";
      if ( f == E->fields.size()-1 && !has_metrics )
        prt << "\", ";
#ifndef EPILOG_ALL_PARAMS
      else if ( E->name == "MPI_RECV"  && f == E->fields.size()-2 && !has_metrics )
        prt << "\", ";
#endif
      else
        prt << ",\", ";
      prt << F.fname << ");\n";
    }
    if  ( has_metrics ) {
      prt << "  printf(\"  metv   : ( \");\n";
      prt << "  { int i;\n";
      prt << "    for (i = 0; i < metc; i++)\n";
      prt << "      printf(\"%3lld \", metv[i]);\n";
      prt << "  }\n";
      prt << "  printf(\")\");\n";
    }
    prt << "  printf(\"\\n\");\n";
    prt << "}" << endl;

#if 0
    // --- elg_anal.c -----------------------------------------------------
    if ( E->sect[0] ) anal << "\n/* -- " << E->sect << " -- */" << endl;
    anal << endl;
    gen_func_head(anal, "void elg_readcb_" + E->name, "", E->fields,
                  "void* userdata");
    anal << " {" << endl;
    bool is_exit = ( E->name.find("EXIT") != string::npos );

    if ( is_exit ) anal << "  StackNode *n;\n\n";
    for (size_t f=0; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      if ( F.fname.find("lid") != string::npos ) {
        anal << "  ";
        if ( F.fname == "rlid" ) anal << "if ( rlid != ELG_NO_ID ) ";
        anal << F.fname << " = locmap[" << F.fname << "];\n";
      }
    }
    if ( E->name == "ENTER" )
      anal << "  stk_push(lid, rid, time);\n";
    else if ( E->name == "ENTER_CS" )
      anal << "  stk_push(lid, csitetab[csid].erid, time);\n";
    else if ( is_exit )
      anal << "  n = stk_pop(lid);\n";
    anal << "}" << endl;
#endif

    // --- TEX tables -----------------------------------------------------
    if ( E->sect[0] ) tex << "\n\\subsection{" << E->sect << "}\n" << endl;
    tex << "\n\\subsubsection{" << gen_tex("ELG_" + E->name) << "}\n" << endl;
    tex << "\\begin{tabular}[h]{|l|l|l|}\n";
    tex << "\\hline\n";
    tex << "\\multicolumn{3}{|l|}{\\tt " << gen_tex("ELG_" + E->name)
        << "} \\\\\n";
    tex << "\\hline\n";
    tex << "{\\tt elg\\_ui4} & {\\tt lid} & location identifier \\\\\n";
    tex << "{\\tt elg\\_d8} & {\\tt time} & time stamp \\\\\n";
    if ( E->fields.size() > 2 ) tex << "\\hline\n";
    for (size_t f=2; f<E->fields.size(); ++f) {
      Field& F = E->fields[f];
      if ( F.fname == "metv[]" )
        tex << "{\\tt elg\\_ui8 | elg\\_d8";
      else
        tex << "{\\tt " << gen_tex(F.type);
      tex << "} & {\\tt " << F.fname << "} & " << F.descr << " \\\\\n";
    }
    tex << "\\hline\n";
    tex << "\\end{tabular}\n" << endl;
  }
  genh << endl;
  trch << endl;
  rwh  << endl;
  rcbh << endl;
}

int main() {
  vector<Event*> events;

  read_spec(events);
  process(events);
}
