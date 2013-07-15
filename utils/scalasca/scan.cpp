/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/* SCALASCA Collection & Analysis Nexus:
   Parses commandline to identify MPI launcher and launch options which are
   augmented for measurement collection and adjusted for trace analysis.
   Uses the target MPI executable and number of processes as default experiment
   title, unless explicitly specified on commandline or via configuation.
   Verifies experiment configuration prior to (optional) execution.
   Spawns measurement collection and (when appropriate) trace analysis.
   Manages output from measurement and analysis and archives it in experiment.
**/

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#ifdef _SX
#   include <algorithm>
#   include <vector>
#endif

#include "epk_conf.h"
#include "epk_archive.h"

const int no_argument=0;
const int required_argument=1;
const int optional_argument=2;
struct option {
    const char* name;
    int has_arg;
    int* flag;
    int val;
};

extern char* optarg;
extern int optind, opterr, optopt;
/* prototype for function in libiberty */
extern "C" int getopt_long_only (int __argc, char *const *__argv,
      const char *__shortopts, const struct option *__longopts, int *__longind);

#ifdef _SX
/* additional libiberty prototypes required on NEC SX */
extern "C" int setenv(const char *name, const char *value, int overwrite);
extern "C" int unsetenv(const char *name);
#endif

using namespace std;

const string VERSION="1.4.3";
const string SCAN="S=C=A=N: ";
const string spc=" ";

unsigned verbose=0;
bool cobalt=false;

enum cnmodes { MODE_DEF, MODE_CO, MODE_SN, MODE_VN, MODE_SMP, MODE_DUAL, MODE_QUAD };
static const char* modes[7] = { "", "co", "sn", "vn", "smp", "dual", "quad" };

enum launch_opt { NO_ARG, NON_OPT, NNODES, NPROCS, NTHRDS, MPICFG, MPICWD, MPIENV,
                  TARGET, T_ARGS, T_PATH, CNMODE, SWITCH_D, CPUCMD, MICCMD, IGNORE };

struct launch_options_t {
    char* target;
    char* t_args;
    char* target2;
    char* t_args2;
    char* t_path;
    const char* cnmode;
    char* nprocs;
    char* nthrds;
    char* mpicwd;
    string mpienv;
    string other;
} launch_options = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "", "" };

int parse_launch (string launcher, int argc, char* argv[])
{
    static int modeopt = MODE_DEF; // only used for yod
    static struct option unified_options[] = {
        {"n", required_argument, 0, NPROCS},
#ifdef BG_MPIRUN
        {"p", required_argument, 0, CNMODE}, // ranks-per-node
#else
        {"p", required_argument, 0, IGNORE}, // partition
#endif
        {"np", required_argument, 0, NPROCS},
        {"sz", required_argument, 0, NPROCS}, // yod
        {"size", required_argument, 0, NPROCS}, // yod
        {"proccount", required_argument, 0, NPROCS}, // Cobalt
        // XXXX -nodes is an alias for -np using BG mpirun
        {"nn", required_argument, 0, NNODES}, // NECSX
        {"nodes", required_argument, 0, NNODES}, // SLURM,POE
        {"ntasks", required_argument, 0, NPROCS}, // SLURM
        {"t", required_argument, 0, NTHRDS}, // llrun@supermuc
        {"time", required_argument, 0, IGNORE}, // SLURM
        {"C", required_argument, 0, MPICWD}, // Sun
        {"D", required_argument, 0, MPICWD}, // SLURM
        {"chdir", required_argument, 0, MPICWD}, // SLURM
        {"wdir", required_argument, 0, MPICWD},
        {"cwd", required_argument, 0, MPICWD},
        {"env", required_argument, 0, MPIENV},
        {"envs", required_argument, 0, MPIENV}, // runjob
        {"configfile", required_argument, 0, MPICFG},
        {"A", required_argument, 0, TARGET}, // Sun: replaces target
        {"exe", required_argument, 0, TARGET}, // BG: includes full path
        {"host", required_argument, 0, IGNORE}, // IMPI convenience
        {"machinefile", required_argument, 0, IGNORE}, // MPICH convenience
        {"q", required_argument, 0, IGNORE}, // cobalt/qsub queue convenience
        {"path", required_argument, 0, T_PATH}, // mpiexec
        {"args", required_argument, 0, T_ARGS}, // BG
        {"mode", required_argument, 0, CNMODE}, // BG/Cobalt
        {"ranks-per-node", required_argument, 0, CNMODE}, // runjob
        {"npernode", required_argument, 0, CNMODE}, // PBS mpiexec
        {"pernode", no_argument, 0, CNMODE},        // PBS mpiexec
        {"nnp", required_argument, 0, CNMODE}, // NECSX
        {"N", required_argument, 0, CNMODE}, // aprun
        {"d", no_argument, 0, SWITCH_D}, // aprun/openmpi/mpt/etc
        {"SN", no_argument, &modeopt, MODE_SN}, // yod
        {"VN", no_argument, &modeopt, MODE_VN}, // yod
        {"nodelist", required_argument, 0, IGNORE}, // SLURM
        {"exclude", required_argument, 0, IGNORE}, // SLURM
        {"c", required_argument, 0, CPUCMD}, // ibrun.symm
        {"m", required_argument, 0, MICCMD}, // ibrun.symm
#ifdef BG_MPIRUN
        {"block", required_argument, 0, IGNORE}, // BG runjob convenience
        {"connect", required_argument, 0, IGNORE}, // BG mpirun convenience
        {"corner", required_argument, 0, IGNORE}, // BG runjob convenience
        {"exp-env", required_argument, 0, IGNORE}, // BG mpirun convenience
        {"mapfile", required_argument, 0, IGNORE}, // BG mpirun convenience
        {"mapping", required_argument, 0, IGNORE}, // BG runjob convenience
        {"partition", required_argument, 0, IGNORE}, // BG mpirun convenience
        {"shape", required_argument, 0, IGNORE}, // BG mpirun convenience
        // most mpirun commands implement -verbose with no argument, but not BlueGene
        {"verbose", required_argument, 0, IGNORE}, // BG mpirun convenience
#endif
#ifdef _AIX
        {"cmdfile", required_argument, 0, MPICFG}, // poe convenience
        {"pgmmodel", required_argument, 0, IGNORE}, // poe convenience
#endif
        {0, 0, 0, 0}
    };

    int nnodes=0; // only for NECSX & Cobalt
    int ch, opti = 0;
    opterr = 0; // disable automatic report of unrecognized options
    if (verbose>1) cerr << "argv[" << optind << "]=" << argv[optind] << endl;
    while ((ch = getopt_long_only(argc, argv, "-:", unified_options, &opti)) != -1) {
      if ((optind>=argc) && (launch_options.target==NULL) && (ch!=TARGET)) {
            if (verbose>1) cerr << "Got invalid optind=" << optind << endl;
            cerr << SCAN << "Launch command line parsing failed (due to bad version of libiberty?)" << endl
                 << "(Hint: try inserting \"-- \" immediately before target executable as workaround.)" << endl;
            exit(2);
        }
        switch(ch) {
            case 0:
                if (verbose>1) cerr << unified_options[opti].name << ": modeopt=" << modeopt << endl;
                switch(modeopt) {
                    case MODE_SN: launch_options.cnmode=modes[MODE_SN]; break;
                    case MODE_VN: launch_options.cnmode=modes[MODE_VN]; break;
                    default: break;
                }
                break;
            case 1:
                cerr << "Got non-option " << argv[optind-1] << endl;
                break;
            case SWITCH_D:
                // needs to be handled differently for aprun/mpt/etc
                if (launcher == "mpirun") { // MPT -d directory
                    launch_options.mpicwd=argv[optind];
                    if (verbose) cerr << "mpicwd=" << launch_options.mpicwd << endl;
                    launch_options.other += spc + argv[optind-1] + spc + argv[optind];
                    optind++;
                } else if ((launcher == "aprun") || (launcher == "mprun")) {
                    if (verbose) cerr << "Ignoring -d switch with arg \"" << argv[optind] << "\"" << endl;
                    launch_options.other += spc + argv[optind-1] + spc + argv[optind];
                    optind++;
                } else { // e.g., "orterun -d"
                    if (verbose) cerr << "Ignoring -d switch without arg" << endl;
                    launch_options.other += spc + argv[optind-1];
                }
                break;
            case CNMODE:
                if (verbose>1) cerr << "cnmode=" << (optarg?optarg:"pernode") << endl;
                if (!optarg) { // single process pernode
                    launch_options.cnmode = strdup("1p");
                    launch_options.other += spc + argv[optind-1];
                    break;
                }
                // cobalt/qsub script mode?
                if (string(optarg) == "script") {
                    cerr << SCAN << "Abort: qsub script mode not supported!" << endl;
                    cerr << "Hint: prefix mpirun/runjob commands within script instead." << endl;
                    exit(1);
                }
                // scan will replace specified mode with "script scan" so drop original here
                if (launcher != "qsub") 
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (string(optarg) == "VN") 
                    launch_options.cnmode=modes[MODE_VN];
                else if (string(optarg) == "CO") 
                    launch_options.cnmode=modes[MODE_CO];
                else if (string(optarg) == "SMP")
                    launch_options.cnmode=modes[MODE_SMP];
                else if (string(optarg) == "DUAL")
                    launch_options.cnmode=modes[MODE_DUAL];
                else if (string(optarg) == "QUAD")
                    launch_options.cnmode=modes[MODE_QUAD];
                else if (atoi(optarg) > 0) {
                    // got a numeric value, so add "per"
                    char* mode=(char*)malloc(2+strlen(optarg));
                    sprintf(mode,"%sp",optarg);
                    launch_options.cnmode=mode;
                } else if ((*optarg == 'c') && (atoi(optarg+1) > 0)) {
                    // convert cobalt numeric mode to "per"
                    char* mode=(char*)malloc(2+strlen(optarg));
                    sprintf(mode,"%sp",optarg+1);
                    launch_options.cnmode=mode;
                } else
                    launch_options.cnmode=optarg;
                break;
            case NNODES:
                nnodes = atoi(optarg);
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (verbose>1) cerr << "nnodes=" << optarg << endl;
                break;
            case NPROCS:
                if ((launcher == "qsub") && (string(argv[optind-2]) == "-n")) {
                    // "qsub -n" specifies number of nodes, not processes!
                    nnodes = atoi(optarg);
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                    if (verbose>1) cerr << "nnodes=" << optarg << endl;
                    break;
                }
                launch_options.nprocs=optarg;
                if (launcher != "qsub")
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (verbose>1) cerr << "nprocs=" << optarg << endl;
                break;
            case NTHRDS:
                if (launcher != "llrun") { // generally "-t" specifies time-limit
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                    break;
                }
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (launcher == "qsub") break; // "qsub -t" specifies job runtime limit, not threads!
                launch_options.nthrds=optarg;
                if (verbose>1) cerr << "nthrds=" << optarg << endl;
                break;
            case MPICFG:
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                cerr << SCAN << "Warning: Ignoring MPI configuration in file \""
                             << optarg << "\"" << endl;
                break;
            case MPICWD:
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (launcher == "llrun") break; // "llrun -C" specifies queue, not workdir!
                launch_options.mpicwd=optarg;
                if (verbose>1) cerr << "mpicwd=" << optarg << endl;
                break;
            case MPIENV:
                // MPI environment variable exports included in "other" launch options
                if (verbose>1) cerr << "mpienv=\"" << optarg << "\"" << endl;
                if (launcher != "qsub") // qsub --env moved to mpirun --env or runjob --envs
                    launch_options.other += spc + argv[optind-2] + " \"" + optarg + "\"";
                { // check whether OMP_NUM_THREADS specified
                bool bgq = epk_archive_exists("/bgsys/drivers/ppcfloor/hlcs");
                string envs(optarg);
                size_t sep, pos=0;
                do {
                    sep = envs.find(':',pos);
                    string env (envs.substr(pos, sep-pos));
                    // XXXX check for EPIK configuration variables
                    if (env.substr(0,16) == "OMP_NUM_THREADS=") {
                        launch_options.nthrds = strdup(env.substr(16).c_str());
                        if (verbose>1) cerr << "(env)nthrds=" << launch_options.nthrds << endl;
                    }
                    // cobalt qsub requires "--env VAR1=val1:VAR2=val2" specification
                    // reformat for mpirun as "-env VAR1=val1 -env VAR2=val2"
                    // reformat for runjob as "-envs VAR1=val1 -envs VAR2=val2"
                    if (launcher == "qsub") launch_options.mpienv += (bgq ? " --envs " : " -env ") + env;
                    pos = sep+1;
                } while (sep != string::npos);
                }
                break;
            case MICCMD:
                if (launcher != "ibrun.symm") { // something else to be ignored
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                } else {
                    string cmd(optarg);
                    size_t sep=cmd.find(spc);
                    if (sep != string::npos) {
                        launch_options.target2 = strdup(cmd.substr(0,sep).c_str());
                        launch_options.t_args2 = strdup(cmd.substr(sep+1).c_str());
                    } else launch_options.target2 = optarg;
                    // don't include target and args in launch_options.other
                    if (verbose>1) cerr << "target2=<" << launch_options.target2 
                        << "> t_args2=" << ((sep!=string::npos) ? launch_options.t_args2 : "(nil)") << endl;
                }
                break;
            case CPUCMD:
                if (launcher != "ibrun.symm") { // something else to be ignored
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                } else {
                    string cmd(optarg);
                    size_t sep=cmd.find(spc);
                    if (sep != string::npos) {
                        launch_options.target = strdup(cmd.substr(0,sep).c_str());
                        launch_options.t_args = strdup(cmd.substr(sep+1).c_str());
                    } else launch_options.target = optarg;
                    // don't include target and args in launch_options.other
                    if (verbose>1) cerr << "target=<" << launch_options.target 
                        << "> t_args=" << ((sep!=string::npos) ? launch_options.t_args : "(nil)") << endl;
                }
                break;
            case TARGET:
                if (launcher == "qsub") { // "qsub -A" specifies project not target!
                    launch_options.other += spc + argv[optind-2] + spc + optarg;
                    break;
                }
                launch_options.target=optarg;
                // don't include target in launch_options.other
                if (verbose>1) cerr << "target=" << optarg << endl;
                break;
            case T_PATH:
                launch_options.t_path=optarg;
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (verbose>1) cerr << "t_path=" << optarg << endl;
                break;
            case T_ARGS:
                launch_options.t_args=optarg;
                // don't include t_args in launch_options.other
                if (verbose>1) cerr << "t_args=\"" << optarg << "\"" << endl;
                break;
            case IGNORE: // arg with parameter to explicitly ignore
                launch_options.other += spc + argv[optind-2] + spc + optarg;
                if (verbose>1) cerr << "Ignoring launcher arg \"" << argv[optind-2]
                                    << " " << optarg << "\"" << endl;
                break;
            case ':':
                cerr << "Missing option parameter" << endl;
                break;
            case '?': // unrecognized or ambiguous option
                if (verbose>0) cerr << "Ignoring unrecognized launcher arg \"" << argv[optind-1] << "\"" << endl;
                launch_options.other += spc + argv[optind-1];
                break;
            default:
                cerr << "Unhandled launcher arg " << optopt << endl;
        }
        if (verbose>1 && argv[optind]) cerr << "argv[" << optind << "]=" << argv[optind] << endl;
    }
#if 0 // use escape=":" instead
    // skip ":" indicating end of runjob launcher options
    if (string(argv[optind]) == ":") optind++;
#endif
    // NECSX and Cobalt may need nprocs calculated from nnodes and cnmode
    if (!launch_options.nprocs && launch_options.cnmode && (nnodes>0)) {
        int nprocs = nnodes * atoi(launch_options.cnmode);
        launch_options.nprocs=(char*)calloc(16,1);
        sprintf(launch_options.nprocs,"%d",nprocs);
        if (verbose) cerr << "Set nprocs=" << string(launch_options.nprocs)
                          << " from nnodes=" << nnodes << " and ppn="
                          << string(launch_options.cnmode) << endl;
    }
    if (!launch_options.other.empty() && verbose)
        cerr << "Other launch options:" << launch_options.other << endl;
    return 0;
}

// locate executable on provided path
string which (string exe, string path)
{
    if (exe[0] == '/') return(exe); // given with absolute path
    size_t token_start = 0;
    while (token_start < path.length()) {
        size_t token_length=strcspn(&path.c_str()[token_start],":");
        string token = path.substr(token_start,token_length);
        token_start += token_length + 1;
        //cerr << "Path token: " << token << endl;
        token += string("/") + exe;
        if (access(token.c_str(), X_OK) == 0) {
            //cerr << "Located executable " << token << endl;
            return(token);
        }
    }
    if (verbose>1) cerr << "No " << exe << " in (" << path << ")" << endl;
    return "";
}

// check for symbol in executable
bool symbol_in_executable (const char* executable, const char* symbol)
{
    string nm_command;
    string nm_path = which("nm", getenv("PATH"));
    if (nm_path.empty()) {
        cerr << SCAN << "Error: \"nm\" not located on PATH!" << endl;
        exit(2);
    }
#ifdef _AIX
    nm_command += "OBJECT_MODE=32_64 ";
#endif
    nm_command += nm_path + spc + string(executable) + " 2>&1"
                          + " | grep -c " + string(symbol) + " > /dev/null";
    if (verbose>1) cerr << nm_command << endl;
    int retval = system(nm_command.c_str());
    if (retval == 2) cerr << SCAN << nm_command << endl
                          << "Warning: " << strerror(errno) << endl;
    return (retval == 0);
}

int main (int argc, char* argv[])
{
    string mic_config;
    int ch, retval, status;
    int last_scan_arg=0;
    time_t time1, time2;
    double dtime;
    string date;
    string envflag, envassign=spc; // specify environment variable on launch command
#ifdef BG_NOFREE
    bool nofree=true; // envvar for BlueGene partition control
#endif
    bool epk_inst_mpi, epk_inst_omp;
    bool preview=false;
    bool redirect=false; // redirect launcher stdout/stderr (for "mpiexec"?)
    bool olaunch=false; // if false, require launch command
    bool measure=true; // if false, analyze only
    // XXXX changing working directory can find a different EPIK.CONF
    bool summary=epk_str2bool(epk_get(EPK_SUMMARY));
    bool tracing=epk_str2bool(epk_get(EPK_TRACE));
    string metrics=epk_get(EPK_METRICS); // measurement metrics
    string filter=epk_get(EPK_FILTER); // measurement filterfile
    string archive;
    string blocker; // potential measurement lockfile
    string escape; // potential launcher escape
    const string USAGE = "Scalasca" + spc + VERSION + 
        ": measurement collection & analysis nexus\n"
        "usage: scan {options} [launchcmd [launchargs]] target [targetargs]\n"
        "      where {options} may include:\n"
        "  -h    Help: show this brief usage message and exit.\n"
        "  -v    Verbose: increase verbosity.\n"
        "  -n    Preview: show command(s) to be launched but don't execute.\n"
        "  -q    Quiescent: execution with neither summarization nor tracing.\n"
        "  -s    Summary: enable runtime summarization. [Default]\n"
        "  -t    Tracing: enable trace collection and analysis.\n"
        "  -a    Analyze: skip measurement to (re-)analyze an existing trace.\n"
        "  -e epik      : Experiment archive to generate and/or analyze.\n"
        "                 (overrides default experiment archive)\n"
        "  -f filtfile  : File specifying measurement filter.\n"
        "  -l lockfile  : File that blocks start of measurement.\n"
        "  -m metrics   : Metric specification for measurement.\n"
#if 0 // obsolete
        "  -O    Omit launch command (e.g., for pure OpenMP target).\n"
#endif
    ;
    if (argc == 1) { cerr << USAGE << endl; exit(0); }

    while ((ch = getopt(argc, argv, "+hvnqstae:f:l:m:O")) != EOF) {
        switch(ch) {
            case 'e':
                archive = optarg;
                if (verbose) cerr << "Got experiment archive \"" << archive << "\"" << endl;
                break;
            case 'f':
                filter = optarg;
                if (verbose) cerr << "Got measurement filter \"" << filter << "\"" << endl;
                break;
            case 'l':
                blocker = optarg;
                if (verbose) cerr << "Got measurement lockfile \"" << blocker << "\"" << endl;
                break;
            case 'm':
                metrics = optarg;
                if (verbose) cerr << "Got measurement metrics \"" << metrics << "\"" << endl;
                break;
            case 'n':
                preview=true;
                break;
            case 'O':
                olaunch=true;
                break;
            case 'q': // execution without summarization or tracing
                measure=true;
                summary=false;
                tracing=false;
                break;
            case 's': // summarization measurement
                measure=true;
                summary=true;
                break;
            case 't': // trace collection & analysis
                measure=true;
                tracing=true;
                break;
            case 'a': // analysis only
                measure=false;
                tracing=true;
                break;
            case 'v':
                if (verbose==1) { // report submitted command-line
                    cerr << argv[0];
                    for (int i=1; i<argc; i++) cerr << spc << argv[i];
                    cerr << endl;
                }
                verbose++;
                break;
            case 'h':
                cerr << USAGE << endl;
                exit(0);
                break;
            case '?': // unrecognized option
            case ':': // missing option argument
            default:
                cerr << USAGE << endl;
                exit(1);
        }
    }
    
    // verify filter
    if (!filter.empty() && access(filter.c_str(), R_OK) != 0) {
        cerr << SCAN << "Abort: Bad filter " << filter
                 << ": " << strerror(errno) << endl;
        exit(2);
    }

    if (getenv("SCAN_MPI_REDIRECT")) {
        redirect = epk_str2bool(getenv("SCAN_MPI_REDIRECT"));
        if (verbose) cerr << "Got SCAN_MPI_REDIRECT \"" << redirect << "\"" << endl;
    }

    if (getenv("SCAN_ESCAPE")) {
        escape = getenv("SCAN_ESCAPE");
        if (verbose) cerr << "Got SCAN_ESCAPE \"" << escape << "\"" << endl;
    }

    if (getenv("SCAN_SETENV")) {
        envflag = getenv("SCAN_SETENV");
        if (verbose) cerr << "Got SCAN_SETENV \"" << envflag << "\"" << endl;
        if (envflag[envflag.length()-1] == '=') {
            envflag.erase(envflag.length()-1);
            envassign='=';
        }
    }

    // cerr << "argc=" << argc << " optind=" << optind << endl;
    last_scan_arg = optind;

    if (optind == argc) {
        cerr << USAGE << "Error: Insufficient arguments." << endl;
        exit(1);
    }

    // finished parsing scan options
    cerr << SCAN << "Scalasca " << VERSION << spc;
    if (!measure)
        cerr << "trace analysis";
//  else if (summary && tracing)
//      cerr << "summarization with trace collection & analysis";
    else if (tracing)
        cerr << "trace collection and analysis";
    else if (summary)
        cerr << "runtime summarization";
    else
        cerr << "quiescent measurement execution";

    if (preview) cerr << " preview";
    cerr << endl;

    if (strcmp(argv[optind],escape.c_str())==0) {
       if (verbose) cerr << "Scanned escaped arg \"" << escape << "\"" << endl;
       optind++;
    }

    // next argument is potentially target or launcher
    if (access(argv[optind], X_OK) == 0) {
        bool epk_inst = symbol_in_executable(argv[optind], "esd_open");
        if (verbose) cerr << (epk_inst ? "" : "No ")
                << "EPIK instrumentation found in " << argv[optind] << endl;
        olaunch |= epk_inst; // omit launch check for instrumented target
    } else if (optind == (argc-1)) { // no access to target
        cerr << argv[optind] << ": " << strerror(errno) << endl;
        exit(1);
    }

    if (!olaunch && (optind == (argc-1))) { // no launcher and no instrumentation
        cerr << "No EPIK instrumentation found in " << argv[optind] << endl;
        exit(1);
    }

    string launcher, launcher_path;
    if (!olaunch) { // start parsing launch options
        unsigned launch_endind, launch_begind=optind++;

        launcher_path = which(argv[launch_begind], getenv("PATH"));
        if (launcher_path.empty()) {
            cerr << SCAN << "Error: " << argv[launch_begind] << " launcher not located!" << endl;
            exit(2);
        }

#ifdef SCAN_FOLLOW_LINKS
        // get actual path if symbolic link
        char pathbuf[PATH_MAX] = { '\0' };
        ssize_t siz = readlink(launcher_path.c_str(), pathbuf, PATH_MAX);
        if ((siz > 0) && (siz < PATH_MAX)) launcher_path = pathbuf;
#endif

        // get basename of launcher
        size_t sep = launcher_path.rfind('/');
        if (sep == string::npos) sep=0; else sep++;
        launcher = launcher_path.substr(sep);

        if ((launcher == "mpiexec") || (launcher == "mpiexec.py")
                                    || (launcher == "mpiexec.hydra")
                                    || (launcher == "mpiexec_mpt")
                                    || (launcher == "yod") // Catamount
                                    || (launcher == "mpirun.lsf") // LSF
                                    || (launcher == "cobalt-mpirun") // ALCF
                                    || (launcher == "orterun") // OpenMPI
                                    || (launcher == "aprun") // CrayXT
                                    || (launcher == "ibrun") // TACC 
                                    || (launcher == "ibrun.symm") // TACC
                                    || (launcher == "llrun") // JSC
                                    || (launcher == "poe") // AIX POE
                                    || (launcher == "srun") // SLURM
                                    || (launcher == "yhrun") // SLURM-derived YHRMS@TH-1A
                                    || (launcher == "ccc_mprun") // Curie
                                    || (launcher == "mprun") // Sun HPC Clustertools
                                    || (launcher == "mpirun")) {
           if (verbose) cerr << "Got launcher \"" << launcher << "\"" << endl;
        } else if (getenv("SCAN_MPI_LAUNCHER") && (launcher == getenv("SCAN_MPI_LAUNCHER"))) {
           if (verbose) cerr << "Got custom SCAN_MPI_LAUNCHER \"" << launcher << "\"" << endl;
        } else if (launcher == "runjob") {
           if (verbose) cerr << SCAN << "Got BG/Q launcher runjob!" << endl;
           escape=":";
           // set RUNJOB configuration from COBALT
           if (getenv("RUNJOB_BLOCK")) {
               cerr << "RUNJOB_BLOCK=" << getenv("RUNJOB_BLOCK") << endl;
           } else if (getenv("COBALT_PARTNAME")) {
              char* block = getenv("COBALT_PARTNAME");
              cerr << "COBALT_PARTNAME=" << block << " -> RUNJOB_BLOCK" << endl;
              setenv("RUNJOB_BLOCK",block,1);
           }
           if (getenv("RUNJOB_CORNER")) {
               cerr << "RUNJOB_CORNER=" << getenv("RUNJOB_CORNER") << endl;
           } else if (getenv("COBALT_CORNER")) {
              char* corner = getenv("COBALT_CORNER");
              cerr << "COBALT_CORNER=" << corner << " -> RUNJOB_CORNER" << endl;
              setenv("RUNJOB_CORNER",corner,1);
           }
           if (getenv("RUNJOB_SHAPE")) {
               cerr << "RUNJOB_SHAPE=" << getenv("RUNJOB_SHAPE") << endl;
           } else if (getenv("COBALT_SHAPE")) {
              char* shape = getenv("COBALT_SHAPE");
              cerr << "COBALT_SHAPE=" << shape << " -> RUNJOB_SHAPE" << endl;
              setenv("RUNJOB_SHAPE",shape,1);
           }
           // check environment for RUNJOB configuration
           if (getenv("RUNJOB_EXE")) {
               launch_options.target = getenv("RUNJOB_EXE");
               if (verbose) cerr << "RUNJOB_EXE target=" << launch_options.target << endl;
               unsetenv("RUNJOB_EXE");
           }
           if (getenv("RUNJOB_ARGS")) {
               launch_options.t_args = getenv("RUNJOB_ARGS");
               if (verbose) cerr << "RUNJOB_ARGS t_args=" << launch_options.t_args << endl;
               unsetenv("RUNJOB_ARGS");
           }
           if (getenv("RUNJOB_NP")) {
               launch_options.nprocs = getenv("RUNJOB_NP");
               if (verbose) cerr << "RUNJOB_NP nprocs=" << launch_options.nprocs << endl;
               unsetenv("RUNJOB_NP");
           }
           if (getenv("RUNJOB_RANKS_PER_NODE")) {
               char* ppn = getenv("RUNJOB_RANKS_PER_NODE");
               char* mode=(char*)malloc(8);
               sprintf(mode,"%sp",ppn);
               launch_options.cnmode=mode;
               if (verbose) cerr << "RUNJOB_RANKS_PER_NODE cnmode=" << launch_options.cnmode << endl;
               unsetenv("RUNJOB_RANKS_PER_NODE");
           }
           // XXXX RUNJOB_ENVS might contain settings
           if (getenv("RUNJOB_ENVS")) {
               cerr << "Warning: ignored RUNJOB_ENVS=\"" << getenv("RUNJOB_ENVS") << "\"" << endl;
           }
        } else if (launcher == "qsub") {
           if (verbose) cerr << SCAN << "Got Cobalt launcher qsub!" << endl;
           cobalt=true;
           if (getenv("COBALT_PARTNAME")) {
               char* block = getenv("COBALT_PARTNAME");
               if (verbose) cerr << "qsub COBALT_PARTNAME=" << block << endl;
               launch_options.other += " --block " + string(block);
           }
           if (getenv("COBALT_CORNER")) {
               char* corner = getenv("COBALT_CORNER");
               if (verbose) cerr << "qsub COBALT_CORNER=" << corner << endl;
               launch_options.other += " --corner " + string(corner);
           }
           if (getenv("COBALT_SHAPE")) {
               char* shape = getenv("COBALT_SHAPE");
               if (verbose) cerr << "qsub COBALT_SHAPE=" << shape << endl;
               launch_options.other += " --shape " + string(shape);
           }
        } else {
           cerr << SCAN << "Error: unrecognized launcher \"" << launcher_path << "\"" << endl;
           cerr << "(Hint: if this is actually a launcher for MPI applications," << endl
                << " try setting SCAN_MPI_LAUNCHER=" << launcher << ")" << endl;
           exit(2);
        }

        parse_launch(launcher, argc, argv);

        launch_endind = optind-1;
        if (verbose>1) cerr << "launch endind=" << launch_endind << endl;
    } else if (verbose)
        cerr << "Omitting launch command" << endl;

    if (getenv("SCAN_TARGET") && strlen(getenv("SCAN_TARGET"))) {
        // use explicitly specified target
        if (launch_options.target)
            cerr << "Target \"" << launch_options.target << "\" being overridden!" << endl;
        launch_options.target = getenv("SCAN_TARGET");
        if (verbose) cerr << "Got SCAN_TARGET \"" << launch_options.target << "\"" << endl;

        int argi=optind;
        while (argi < argc) {
            if (strcmp(argv[argi],launch_options.target)==0) {
                // add intervening args to launch_options
                while (optind < argi) launch_options.other += spc + string(argv[optind++]);
                if (verbose)
                    cerr << "Other launch options:" << launch_options.other << endl;
                break;
            }
            argi++;
        }
        if (argi==argc) {
            cerr << "Failed to find SCAN_TARGET \"" << launch_options.target
                 << "\" on command-line" << endl;
            exit(2);
        }
    }

    string t_args = launch_options.t_args ? launch_options.t_args : "";
    // retrieve target and args from remainder of commandline
    if (optind < argc) {
        if (strcmp(argv[optind],escape.c_str())==0) {
            if (verbose) cerr << "Scanned escaped arg \"" << escape << "\"" << endl;
            optind++;
        }
        launch_options.target = argv[optind++];
        if (verbose) cerr << "Target:" << launch_options.target << endl;
        while (optind < argc)  t_args += string(argv[optind++]) + spc;
        if (!t_args.empty()) t_args[t_args.length()-1]='\0'; // trailing space
        launch_options.t_args = (char*) t_args.c_str();
        if (verbose) cerr << "T_args:\"" << launch_options.t_args << "\"" << endl;
    }

    // target examined to determine measurement & analysis configurations
    if (1 /*measure || archive.empty()*/) {
        // check target set, exists & executable
        if (!launch_options.target) {
            cerr << SCAN << "Abort: Target executable not known!" << endl;
            exit(1);
        }

        string target; // target executable, with path if specified
        if (launch_options.t_path) target = string(launch_options.t_path) + "/";
        target += launch_options.target;
        // expand leading tilde with home directory
        if ((target[0]=='~') && getenv("HOME")) target.replace(0,1,getenv("HOME"));
        
        if (access(target.c_str(), X_OK) == -1) {
            cerr << SCAN << "Abort: Target executable " << target
                 << ": " << strerror(errno) << endl;
            if (!launcher.empty() && !launch_options.other.empty())
                cerr << "(Hint: if `" << target
                 << "' is a parameter of an (ignored) " << launcher
                 << " launch argument, rather than the intended target executable,"
                 << " then try quoting \"" 
                 << launch_options.other.substr(launch_options.other.rfind(" -")+1)
                 << spc << launch_options.target << "\".)" << endl;
            exit(1);
        }
        if (epk_archive_exists(target.c_str())) {
            cerr << SCAN << "Abort: Target executable `" << target
                 << "' is a directory!" << endl;
            exit(1);
        }

        // check target for EPIK instrumentation
        bool epk_inst = symbol_in_executable(target.c_str(), "esd_open");
        if (!epk_inst) { 
            cerr << SCAN << "Abort: No "
                 << "EPIK instrumentation found in target " << target << endl;
            exit(1);
        } else if (verbose)
            cerr << "EPIK instrumentation found in target " << target << endl;

        // examine target for MPI instrumentation
        epk_inst_mpi = symbol_in_executable(target.c_str(), "epk_mpi_register");
        if (verbose) cerr << (epk_inst_mpi ? "" : "No ")
                << "EPIK MPI instrumentation found in target " << target << endl;

        if (epk_inst_mpi && (!olaunch && launcher.empty())) {
            cerr << SCAN << "Abort: No launcher for MPI target " << target << endl;
            exit(1);
        }

        // omit launcher requirement when not MPI
        if (launcher.empty() && !epk_inst_mpi && !olaunch) {
            if (verbose) cerr << "Omitting MPI launch" << endl;
            olaunch=true;
        }

        // examine target for OpenMP instrumentation
        epk_inst_omp = symbol_in_executable(target.c_str(), "epk_omp_register");
        if (verbose) cerr << (epk_inst_omp ? "" : "No ")
                << "EPIK OMP instrumentation found in target " << target << endl;

        if (epk_inst_omp && !launch_options.nthrds) {
            launch_options.nthrds = (char*) epk_get(ESD_MAX_THREADS);
            if (strcmp(launch_options.nthrds, "OMP_NUM_THREADS") == 0) {
                launch_options.nthrds = getenv("OMP_NUM_THREADS");
                if (launch_options.nthrds && verbose)
                    cerr << "Got " << launch_options.nthrds 
                         << " threads from OMP_NUM_THREADS" << endl;
                else if (verbose)
                    cerr << "OMP_NUM_THREADS not set" << endl;
            } else if (launch_options.nthrds && verbose)
                cerr << "Got " << launch_options.nthrds 
                     << " threads from ESD_MAX_THREADS" << endl;
        }

        // examine target for HWC metric instrumentation
        if (!metrics.empty() && !symbol_in_executable(target.c_str(), "esd_metric_open")) {
            cerr << SCAN << "Abort: No metric instrumentation found in target " << target << endl;
            exit(1);
        }
    }

    // determine number of processes (if necessary)

    if ((epk_archive_exists("/bgl/BlueLight") || epk_archive_exists("/bgsys")) &&
                                      (cobalt || !launch_options.nprocs)) {
        if (!launch_options.cnmode) { // set default mode according to system
            if (launcher=="runjob") launch_options.cnmode=strdup("1p");
            else /* mpirun */       launch_options.cnmode=modes[MODE_SMP];
        }

        if (!launch_options.nprocs) {
            // determine nprocs from mode and number of nodes
            int nnodes = 0, nprocs = 0;
            if (getenv("LOADL_BG_SIZE")) {
                nnodes = atoi(getenv("LOADL_BG_SIZE"));
                if (verbose) cerr << "Got " << nnodes
                                  << " nodes from LOADL_BG_SIZE" << endl;
            }
            if (getenv("COBALT_PARTSIZE")) {
                nnodes = atoi(getenv("COBALT_PARTSIZE"));
                if (verbose) cerr << "Got " << nnodes
                                  << " nodes from COBALT_PARTSIZE" << endl;
            }
            if (atoi(launch_options.cnmode))
                nprocs = nnodes * atoi(launch_options.cnmode);
            else if (string(launch_options.cnmode) == string(modes[MODE_VN])) 
                nprocs = nnodes * 4;
            else if (string(launch_options.cnmode) == string(modes[MODE_QUAD]))
                nprocs = nnodes * 4;
            else if (string(launch_options.cnmode) == string(modes[MODE_DUAL]))
                nprocs = nnodes * 2;
            else /* MODE_SMP */
                nprocs = nnodes;

            launch_options.nprocs=(char*)calloc(16,1);
            sprintf(launch_options.nprocs,"%d",nprocs);
        }
        if (verbose) cerr << "Set BlueGene mode=" << string(launch_options.cnmode)
                          << " nprocs=" << launch_options.nprocs << endl;
    }

    if (epk_inst_mpi && (launcher == "ibrun.symm")) {
        char* tasks = getenv("SLURM_TASKS_PER_NODE");
        if (!tasks) {
            cerr << "SLURM_TASKS_PER_NODE not set!" << endl;  
        } else {
            int nodes, ppn;
            char* cnmode = (char*)calloc(8,1);
            sscanf(tasks,"%d(x%d)", &ppn, &nodes);
            launch_options.nprocs = (char*)calloc(16,1);
            sprintf(launch_options.nprocs, "%d", nodes*ppn);
            sprintf(cnmode, "%dp", ppn);
            launch_options.cnmode = cnmode;
            
            if (verbose) cerr << "Got " << launch_options.nprocs << " processes with "
                              << ppn << " per node from SLURM_TASKS_PER_NODE=" << tasks << endl;

            if (launch_options.target2) {
                const char* mic_ppn = (getenv("MIC_PPN") ? getenv("MIC_PPN") : "4"); // default:4
                const char* mic_thrds = getenv("MIC_OMP_NUM_THREADS"); // default:30
                int ranks = nodes * atoi(mic_ppn);
                if (ranks) {
                    char* mic_ranks = (char*)calloc(8,1);
                    sprintf(mic_ranks, "%d", ranks);
                    mic_config = "+mic" + string(mic_ppn) + "p" + string(mic_ranks);
                    if (mic_thrds) mic_config += "x" + string(mic_thrds);
                    if (verbose) cerr << "Got MIC config " << mic_config << endl;
                }
            } else  if (verbose) cerr << "No MIC config!" << endl;
        }
    }

    if (epk_inst_mpi && (launcher == "srun") && !launch_options.nprocs) {
        launch_options.nprocs = getenv("SLURM_NPROCS");
        if (verbose) cerr << "Got " << launch_options.nprocs 
                          << " processes from SLURM_NPROCS" << endl;
    }

    if (epk_inst_mpi && (launcher == "mpiexec") && !launch_options.nprocs) {
        // check LSF environment
        launch_options.nprocs = getenv("LSB_DJOB_NUMPROC");
        if (verbose) cerr << "Got " << launch_options.nprocs 
                          << " processes from LSB_DJOB_NUMPROC" << endl;
    }

    if (epk_inst_mpi && (launcher == "mpirun.lsf") && !launch_options.nprocs) {
        char* env=getenv("LSB_MCPU_HOSTS");
        if (!env)
            cerr << "Warning: LSB_MCPU_HOSTS not set!" << endl;
        else {
            env=strdup(env);
            if (verbose>1) cerr << "LSB_MCPU_HOSTS = " << env << endl;
            // parse alternating pairs of hostnames and process counts
            unsigned nprocs = 0;
            char* token = strtok(env, " ");
            do {
                nprocs += atoi(strtok(NULL, " "));
            } while ( (token = strtok(NULL," ")) != NULL );

            launch_options.nprocs=(char*)calloc(16,1);
            sprintf(launch_options.nprocs,"%d",nprocs);
            if (verbose) cerr << "Got " << launch_options.nprocs 
                              << " processes from LSB_MCPU_HOSTS" << endl;
            free(env);
        }
    }

    if (epk_inst_mpi && (launcher == "ibrun") && !launch_options.nprocs) {
        // determine SGE configuration
        const char *peway = getenv("PE");
        const char *nslots = getenv("NSLOTS");
        const char *my_nslots = getenv("MY_NSLOTS");
        if (my_nslots) {
            launch_options.nprocs = getenv("MY_NSLOTS");
            if (verbose) cerr << "Got " << launch_options.nprocs
                              << " processes from MY_NSLOTS" << endl;
        } else if (nslots && peway) {
            int slots=atoi(nslots);
            int way=atoi(peway);
            int nprocs = slots*way/16;
            if (verbose) cerr << "Got " << nprocs << " processes"
                              << " from NSLOTS(" << nslots
                              << ")*PE(" << peway << ")/16" << endl;
            if (nprocs > 0) {
                launch_options.nprocs=(char*)calloc(16,1);
                sprintf(launch_options.nprocs,"%d",nprocs);
            }
        }
    }

    if (epk_inst_mpi && (launcher == "poe")) {
        unsigned nprocs=0;
        if (getenv("LOADL_STEP_ID")) { // LoadLeveler batch job
#if 0
            // LOADL_PROCESSOR_LIST contains the list of allocated hosts per task
            // but it's not set if there are more than 128 tasks!
            string plist = getenv("LOADL_PROCESSOR_LIST");
            if (plist.empty()) {
                cerr << SCAN << "Warning: empty LOADL_PROCESSOR_LIST for poe launch" << endl;
            } else {
                for (unsigned i=0; i<plist.length(); i++) if (plist[i]==' ') nprocs++;
                if (verbose) cerr << "Got " << nprocs << " processes from poe LOADL_PROCESSOR_LIST" << endl;
            }
#else
            // newer versions of LoadLeveler have number of task instances (for non-master task)
            string llquery = "llq -l $LOADL_STEP_ID | grep -c \"Task Instance:\"";
            //string llquery = "llq -l $LOADL_STEP_ID | grep \"Num Task Inst:\" | cut -d: -f 2 | tail -n 1";
            int tokens;
            FILE* stream = popen(llquery.c_str(), "r");
            if (!stream)
                cerr << SCAN << "Error: " << llquery << endl << strerror(errno) << endl;
            else if ((tokens=fscanf(stream, "%d", &nprocs)) != 1)
                cerr << SCAN << "Error: llquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
            else if (verbose)
                cerr << "Got " << nprocs << " processes from llquery" << endl;
            if (stream) pclose(stream);
            if (nprocs > 0) nprocs--; // don't count master task

            if (nprocs <= 0) { // try again with former keyword specifying number of CPUs for step
                string llquery = "llq -l $LOADL_STEP_ID | grep \"Step Cpus:\" | cut -d: -f 2";
                stream = popen(llquery.c_str(), "r");
                if (!stream)
                    cerr << SCAN << "Error: " << llquery << endl << strerror(errno) << endl;
                else if ((tokens=fscanf(stream, "%d", &nprocs)) != 1)
                    cerr << SCAN << "Error: llquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
                else if (verbose)
                    cerr << "Got " << nprocs << " processes from llquery" << endl;
                if (stream) pclose(stream);
            }
#endif
        } else if (getenv("MP_PROCS")) { // interactive job
            nprocs = atoi(getenv("MP_PROCS"));
            if (verbose) cerr << "Got " << nprocs << " processes from MP_PROCS" << endl;
        }

        launch_options.nprocs=(char*)calloc(16,1);
        sprintf(launch_options.nprocs,"%d",nprocs);
    }

    if (epk_inst_mpi && (launcher=="mpiexec") && !launch_options.nprocs) {
        // PBS_NODEFILE lists compute nodes one per line
        if (getenv("PBS_NODEFILE")) {
            unsigned nnodes=0;
            int tokens;
            string wcquery = (launch_options.cnmode ? "sort -u" : "cat")
                           + string(" $PBS_NODEFILE | wc -l");
            FILE* stream = popen(wcquery.c_str(), "r");
            if (!stream)
                cerr << SCAN << "Error: " << wcquery << endl << strerror(errno) << endl;
            else if ((tokens=fscanf(stream, "%d", &nnodes)) != 1)
                cerr << SCAN << "Error: wcquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
            else if (verbose>1)
                cerr << "Got " << nnodes << " nodes from PBS_NODEFILE" << endl;
            if (nnodes > 0) {
                int ppn = launch_options.cnmode ? atoi(launch_options.cnmode) : 1;
                int nprocs = nnodes*ppn;
                launch_options.nprocs = (char*)calloc(16,1);
                sprintf(launch_options.nprocs,"%d",nprocs);
                if (verbose)
                    cerr << "Got " << nprocs << " processes from PBS_NODEFILE" << endl;
            }
            if (stream) pclose(stream);
        }
    }

    if (epk_inst_mpi && (launcher == "yod")) {
        /* envflag="-setenv"; */ envassign="=";
        // PBS_NNODES is the number of available compute nodes
        unsigned nnodes=0;
        const char* env = getenv("PBS_NNODES");
        if (env) {
            nnodes = atoi(env);
            if (verbose) cerr << "PBS_NNODES=" << nnodes << endl;
            if (nnodes <= 0) {
                cerr << SCAN << "Abort: invalid PBS_NNODES=" << env << " for yod launch" << endl;
                exit(1);
            }
        }
        // nprocs=all is the default, indicating all available processors
        unsigned nprocs=nnodes;
        if ((nnodes && !launch_options.nprocs) || (launch_options.nprocs && 
                 string(launch_options.nprocs) == "all")) {
            launch_options.nprocs = (char*)calloc(8,1);
            if (!launch_options.cnmode || 
                     (string(launch_options.cnmode) == string(modes[MODE_VN])))
                nprocs*=2; // XXXX dualcore may not always be the default
            sprintf(launch_options.nprocs, "%d", nprocs);
        }
        if (verbose) cerr << "Got " << nprocs << " processes on " << nnodes << " compute nodes" << endl;
    }

    if (epk_inst_mpi) {
        if (getenv("SCAN_MPI_RANKS")) { // explicit specification
            int ranks = epk_str2int(getenv("SCAN_MPI_RANKS"));
            cerr << "Using SCAN_MPI_RANKS=" << ranks << " processes!" << endl;
            launch_options.nprocs = getenv("SCAN_MPI_RANKS");
        }
        if (olaunch && !launch_options.nprocs) { // mark as omitted
            launch_options.nprocs = strdup("O"); 
        } else if (!launch_options.nprocs) { // check nprocs set and valid
            cerr << SCAN << "Warning: Number of processes not known!" << endl;
        } else if (atoi(launch_options.nprocs) <= 0) { // verify number
            cerr << SCAN << "Warning: Invalid number of processes: " << launch_options.nprocs << endl;
            launch_options.nprocs = strdup("X"); 
        }

        // check mpicwd exists, if specified
        if (launch_options.mpicwd && !epk_archive_exists(launch_options.mpicwd)) {
            cerr << SCAN << "Abort: Non-existant MPICWD \"" << launch_options.mpicwd << "\"" << endl;
            exit(1);
        }
    }

    // determine (provisional) archive location
    string epk_archive = epk_get(EPK_GDIR);

    string epk_title = epk_get(EPK_TITLE);
    if (!archive.empty()) { // override EPK_GDIR/TITLE with specified archive
        if (archive[archive.length()-1] == '/') archive[archive.length()-1] = '\0';
        if (verbose) cerr << "archive: " << archive << endl;
        string::size_type epik_start = archive.rfind("epik_");
        string::size_type path_split = archive.rfind("/");
        if ((epik_start == string::npos) ||
            ((epik_start > 0) && (path_split != epik_start-1))) {
            cerr << SCAN << "Abort: Invalid experiment archive name " << archive << endl;
            exit(1);
        }
        epk_title = archive.substr(epik_start+5); // title excluding prefix
        if (path_split != string::npos) { // archive includes path specification
            epk_archive = archive.substr(0,epik_start);
        } else {
            epk_archive = "."; // unspecified path
        }
        if (verbose) cerr << "EPK_GDIR=" << epk_archive.c_str() << endl;
        retval = setenv("EPK_GDIR", epk_archive.c_str(), 1);
        if (retval) cerr << "setenv: " << strerror(errno) << endl;
    } else if (epk_title == "a") { // default to override
        // determine EPK_TITLE from basename(target) + cnmode + nprocs
        string target(launch_options.target);
        size_t sep = target.rfind('/');
        if (sep == string::npos) sep=0; else sep++;
        size_t dot = target.find('.',sep);
        if (dot == string::npos) dot=target.length();
        epk_title = target.substr(sep,dot-sep) + "_";
        if (launch_options.cnmode) epk_title += launch_options.cnmode;
        if (launch_options.nprocs) epk_title += launch_options.nprocs;
        else /*if (olaunch)*/ epk_title += "O";
        if (launch_options.nthrds) epk_title += string("x") + launch_options.nthrds;
        else if (epk_inst_omp)     epk_title += "xO";

        if (!mic_config.empty()) epk_title += mic_config;

        // append suffix indicating type of measurement
        if (tracing) epk_title += "_trace"; // may include summary
        else if (summary) epk_title += "_sum";
        // append metrics
        if (!metrics.empty()) epk_title += string("_") + metrics;
        if (verbose) cerr << "EPK_GDIR=" << epk_archive << endl;
    }

    if (launch_options.mpicwd) { // update relative archive paths
        if (epk_archive == ".")
            epk_archive = string(launch_options.mpicwd);
        else if (epk_archive[0] != '/')
            epk_archive = string(launch_options.mpicwd) + "/" + epk_archive;
    }

    // check EPK_LDIR consistent with EPK_GDIR
    string epk_workdir = epk_get(EPK_LDIR);
    if (epk_workdir == ".") { // update default to match EPK_GDIR
        if (string(getenv("PWD")) != "/root") {
            if (verbose) cerr << "EPK_LDIR=" << epk_archive.c_str() << endl;
            retval = setenv("EPK_LDIR", epk_archive.c_str(), 1);
            if (retval) cerr << "setenv: " << strerror(errno) << endl;
        } else
            if (verbose) cerr << "Leaving EPK_LDIR=. when cobalt PWD=/root" << endl;
    } else if (epk_workdir != epk_archive) {
        cerr << "EPK_LDIR=" << epk_workdir << endl;
        cerr << "Warning: EPK_LDIR<>EPK_GDIR may result in tracing performance degradation!" << endl;
    }

    if (measure) {
        // verify accessiblity of directory to contain new archive
        if (access(epk_archive.c_str(), W_OK|X_OK) == -1) {
            cerr << SCAN << "Abort: unwritable directory " << epk_archive << endl;
            exit(1);
        }

        char* tracefsys = getenv("SCAN_TRACE_FILESYS");
        if (tracing && tracefsys) { // verify filesystem/device for trace collection
            if (verbose) cerr << "SCAN_TRACE_FILESYS=" << tracefsys << endl;
            dev_t tracefsys_dev, fsys_dev;
            struct stat sbuf;
            retval = stat(tracefsys, &sbuf);
            tracefsys_dev = sbuf.st_dev;
            if (retval != 0) {
                cerr << "Error: " << tracefsys << ": " << strerror(errno) << endl;
            }

            retval = stat(epk_archive.c_str(), &sbuf);
            fsys_dev = sbuf.st_dev;
            if (retval != 0) {
                cerr << "Error: " << epk_archive << ": " << strerror(errno) << endl;
            }

            if (verbose>1) cerr << "Got expt fsys dev " << fsys_dev 
                              << " and trace fsys dev " << tracefsys_dev << endl;
            if (fsys_dev != tracefsys_dev) {
                cerr << "Error: archive directory \"" << epk_archive 
                     << "\" not located on filesystem suitable for trace experiments "
                     << tracefsys << endl;
                exit(7);
            }
        }
    }

    // (re-)set environment variable to experiment title ?
    if (verbose) cerr << "EPK_TITLE=" << epk_title.c_str() << endl;
    retval = setenv("EPK_TITLE", epk_title.c_str(), 1);
    if (retval) cerr << "setenv: " << strerror(errno) << endl;

    // now append archive name to archive path
    epk_archive += string("/epik_") + epk_title;

    // verify archive location usable for new measurement and/or analysis
    if (measure) {
        if (epk_archive_exists(epk_archive.c_str())) {
            // remove existing archive if overwrite is set
            if (getenv("SCAN_OVERWRITE") && epk_str2bool(getenv("SCAN_OVERWRITE"))) {
                int pause = epk_str2int(getenv("SCAN_OVERWRITE"));
                cerr << SCAN << "Warning: SCAN_OVERWRITE of existing " << epk_archive << endl;
                sleep(pause);
                string remove = "/bin/rm -rf " + epk_archive;
                cerr << "[" << remove << "]" << endl;
                retval = system(remove.c_str());
                if (retval == -1) cerr << SCAN << "Error: remove " << strerror(errno) << endl;
                status = WEXITSTATUS(retval);
                if (status) cerr << SCAN << remove << " returned " << status << endl;
                // verify old archive removed
                if (epk_archive_exists(epk_archive.c_str())) {
                    cerr << SCAN << "Abort: failed to remove existing archive " << epk_archive << endl;
                    exit(1);
                }
            } else {
                cerr << SCAN << "Abort: measurement blocked by existing archive " << epk_archive << endl;
                exit(1);
            }
        }
    } else {
        if (!epk_archive_exists(epk_archive.c_str())) {
            cerr << SCAN << "Abort: archive to analyze not found " << epk_archive << endl;
            exit(1);
        }
        // verify accessiblity of existing archive for analysis
        if (access(epk_archive.c_str(), R_OK|W_OK|X_OK) == -1) {
            cerr << SCAN << "Abort: inaccessible archive " << epk_archive << endl;
            exit(1);
        }
    }

    cerr << SCAN << epk_archive << " experiment archive" << endl;

    string analcmd;
    string analyzer;

    if (!measure || tracing) { // locate trace analyzer
        string path;
        string scan = string(argv[0]);
        size_t sep = scan.rfind('/');
        if (getenv("SCALASCA_RTS")) path += string(getenv("SCALASCA_RTS")) + ":";
        if (getenv("SCALASCA_DIR")) path += string(getenv("SCALASCA_DIR")) + "/bin:";
        // add path to SCAN executable itself (if specified)
        if (sep != string::npos)    path += scan.substr(0,sep) + ":";
        path += getenv("PATH");
        // cerr << "PATH=" << path << endl;

        if (getenv("SCAN_TRACE_ANALYZER")) {
            analcmd = getenv("SCAN_TRACE_ANALYZER");
            if (verbose) cerr << "Got SCAN_TRACE_ANALYZER \"" << analcmd << "\"" << endl;
            if (analcmd == "none") {
                analyzer = "none";
                cerr << "Info: Automatic trace analysis will be skipped!" << endl;
            } else {
                analyzer = which(analcmd, path);
                if (analyzer.empty()) {
                    cerr << SCAN << "Abort: specified trace analyzer (" << analcmd << ") not found!" << endl;
                    exit(1);
                }
                // strip dirname and extension from analcmd
                if ((sep = analcmd.rfind('/')) != string::npos) analcmd.erase(0,sep);
                if ((sep = analcmd.find('.')) != string::npos) analcmd.erase(sep);
            }
        }

        if (analyzer.empty() && epk_inst_omp && epk_inst_mpi) {
            analcmd = "scout";
            string analexe = analcmd + ".hyb";
            analyzer = which(analexe, path);
            if (analyzer.empty())
                cerr << SCAN << "Warning: hybrid trace analyzer (" << analexe << ") not found!" << endl;
        }

        if (analyzer.empty() && epk_inst_mpi) {
            analcmd = "scout";
            string analexe = analcmd + ".mpi";
            analyzer = which(analexe, path);
            if (analyzer.empty())
                cerr << SCAN << "Warning: MPI trace analyzer (" << analexe << ") not found!" << endl;
            else if (epk_inst_omp)
                cerr << SCAN << "Warning: MPI trace analyzer will ignore OpenMP events in hybrid trace!"  << endl;
        }

        if (analyzer.empty() && epk_inst_omp && !epk_inst_mpi) {
            analcmd = "scout";
            string analexe = analcmd + ".omp";
            analyzer = which(analexe, path);
            if (analyzer.empty())
                cerr << SCAN << "Warning: OMP trace analyzer (" << analexe << ") not found!" << endl;
        }

        if (analyzer.empty()) {
            analcmd = "scout";
            string analexe = analcmd + ".ser";
            analyzer = which(analexe, path);
            if (analyzer.empty())
                cerr << SCAN << "Warning: serial trace analyzer (" << analexe << ") not found!" << endl;
        }

        if (analyzer.empty()) {
            analcmd = "expert";
            analyzer = which(analcmd, path);
            if (analyzer.empty())
                cerr << SCAN << "Warning: serial trace analyzer (" << analcmd << ") not found!" << endl;
        }

        if (analyzer.empty()) {
            cerr << SCAN << "Abort: no suitable trace analyzer found!" << endl;
            exit(1);
        }
        if (verbose) cerr << "Got trace analyzer \"" << analyzer << "\"" << endl;
    }

    if (measure) { // update defaults by setting environment variables
        retval = setenv("EPK_SUMMARY", (summary ? "1" : "0"), 1);
        if (retval) cerr << "setenv(EPK_SUMMARY): " << strerror(errno) << endl;
        retval = setenv("EPK_TRACE", (tracing ? "1" : "0"), 1);
        if (retval) cerr << "setenv(EPK_TRACE): " << strerror(errno) << endl;

        if (!filter.empty()) {
            if (verbose) cerr << "EPK_FILTER=" << filter.c_str() << endl;
            retval = setenv("EPK_FILTER", filter.c_str(), 1);
            if (retval) cerr << "setenv: " << strerror(errno) << endl;
        }
        if (!metrics.empty()) {
            if (verbose) cerr << "EPK_METRICS=" << metrics.c_str() << endl;
            retval = setenv("EPK_METRICS", metrics.c_str(), 1);
            if (retval) cerr << "setenv: " << strerror(errno) << endl;
        }
    }

    string launch = launcher_path;
    launch += launch_options.other;

    // formulate revised launch command
    string mpietc;
    if (epk_archive_exists("/bgl/BlueLight") || epk_archive_exists("/bgsys")) {
        bool bgq = epk_archive_exists("/bgsys/drivers/ppcfloor/hlcs");
        envflag="-env"; envassign="="; status=0;
        if (bgq) envflag="--envs";
        // unset envflag if using it results in conflicts
        if (launch_options.other.find("--env-all") != string::npos) envflag.erase();

        if (verbose) cerr << "Got BlueGene with launcher " << launcher << endl;
        if (cobalt) {
            string cobalt_launcher = (bgq ? "runjob" : "cobalt-mpirun");
            cerr << SCAN << "Got Cobalt BlueGene launcher " << launcher 
                         << " for " << cobalt_launcher << endl;

            char buf[256];
            ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
            if (len < 0) {
                cerr << SCAN << "Abort; failed readlink: " << strerror(errno) << endl;
                exit(5);
            } else if (len == sizeof(buf)) {
                cerr << SCAN << "Abort: readlink buf[" << sizeof(buf) << "] too small" << endl;
                exit(6);
            } else buf[len]='\0'; // terminate name

            // SCAN needs to be specified with full path (where scout is also found)!
            string scan_path(buf);
            string::size_type loc=scan_path.rfind("/fe/",len);
            if (loc!= string::npos) scan_path.erase(loc,3);

            string script = launch + mpietc + spc + "--mode script ";
            script += scan_path + spc;
            for (int iarg=1; iarg<last_scan_arg; iarg++) script += argv[iarg] + spc;
            script += cobalt_launcher + launch_options.mpienv + spc;

            if (bgq) {
                //if (!launch_options.mpicwd) script += "--cwd " + string(getenv("PWD")) + spc;
                if (launch_options.cnmode) {
                    string ppn = launch_options.cnmode;
                    if (ppn[ppn.length()-1] == 'p') ppn.erase(ppn.length()-1,1);
                    script += "--ranks-per-node " + ppn + spc;
                }
                script += "--np " + string(launch_options.nprocs) + spc;
#ifdef BG_RUNJOB_ALT
                script += ": ";
                script += string(launch_options.target);
                if (launch_options.t_args && strlen(launch_options.t_args))
                    script += spc + launch_options.t_args;
#else
                script += "--exe " + string(launch_options.target);
                if (launch_options.t_args && strlen(launch_options.t_args))
                    script += spc + "--args " + launch_options.t_args;
#endif
            } else {
                if (!launch_options.mpicwd) script += "-cwd " + string(getenv("PWD")) + spc;
                if (launch_options.cnmode) script += "-mode " + string(launch_options.cnmode) + spc;
                script += "-np " + string(launch_options.nprocs) + spc;
                script += launch_options.target;
                if (launch_options.t_args && strlen(launch_options.t_args))
                    script += spc + launch_options.t_args;
            }
            cerr << script << endl;
            if (!preview) {
                // submit script
                retval = system(script.c_str());
                if (retval == -1) cerr << SCAN << "Error: qsub " << strerror(errno) << endl;
                status = WEXITSTATUS(retval);
                if (status) cerr << SCAN << "qsub returned status " << status << endl;
            }
            cerr << SCAN << epk_archive << " job submission" 
                 << (preview ? " preview" : "") << spc 
                 << (status ? "probably failed!" : "complete.") << endl;
            exit(0); // all done for now
        } 
#if 0 // this seems over-restrictive and unhelpful, unless custom launcher also allowed
        else
        if ((launcher != "mpirun") && (launcher != "runjob") && (launcher != "cobalt-mpirun") && (launcher != "llrun")) {
            cerr << SCAN << "Abort: unexpected BlueGene launcher " << launcher << endl;
            exit(1);
        }
#endif

#ifdef BG_NOFREE
        // check whether to retain partition for trace analysis
        nofree = (getenv("MPIRUN_NOFREE") != NULL);
        if (nofree && verbose) cerr << "MPIRUN_NOFREE=" << getenv("MPIRUN_NOFREE") << endl;
        if (launcher == "llrun") nofree=true; // inapplicable
        if (measure && tracing && !nofree) {
            if (verbose) cerr << "Setting MPIRUN_NOFREE to retain partition for analysis" << endl;
            retval = setenv("MPIRUN_NOFREE", "1", 0); // no override
            if (retval) cerr << "setenv: " << strerror(errno) << endl;
        }
#endif
    }

    if (envflag.empty()) {
        // ParaStation MPI needs EPIK configuration explicitly passed
        if (launcher_path.find("/parastation/") != string::npos)
#ifdef PARASTATION_ALT
          // exporting all environment variables to all processes may lead to timeouts
          { mpietc += " --envall"; }
#else
          // the documented "-E ENV val" may be unreliable for more than one envvar
          { envflag="-E"; }
#endif
        // OpenMPI (mostly) needs EPIK configuration explicitly passed
        if (launcher == "orterun") { envflag="-x"; envassign="="; }
    }

    // incorporate updated environment in launch command
    if (!envflag.empty()) {
        // copy all (set) EPIK configuration variables from environment
        const char* key;
        for (unsigned i=0; (key = epk_key(i)) != NULL; i++) {
            if (getenv(key)) {
                string keystring = spc + envflag + spc + key + envassign + getenv(key);
                if (string(key) == "EPK_VERBOSE")
                    launch += keystring; // apply to both measure & analyze
                else
                    mpietc += keystring; // apply only for measurement
            }
        }
    }

#ifdef _SX
    // On NEC-SX, environment variables that should be passed by mpirun to
    // the MPI tasks need to be specified in MPIEXPORT
    {
        vector<string> exp_vars;

        // Tokenize current value of MPIEXPORT
        const char* env_value = getenv("MPIEXPORT");
        if (env_value) {
            char* tmp = strdup(env_value);
            char* pos = strtok(tmp, " ");
            while (pos) {
                exp_vars.push_back(string(pos));
                pos = strtok(NULL, " ");
            }
            free(tmp);
        }

        // Add EPIK configuration variables
        for (unsigned i=0; i<EPK_CONF_VARS; i++) {
            const char* key = epk_key(i);
            if (key && getenv(key) &&
                exp_vars.end() == find(exp_vars.begin(), exp_vars.end(),
                                       string(key)))
                exp_vars.push_back(string(key));
        }

        // Set/overwrite MPIEXPORT
        string mpiexport;
        vector<string>::iterator it = exp_vars.begin();
        while (exp_vars.end() != it) {
            mpiexport += *it + " ";
            ++it;
        }
        if (verbose) cerr << "Setting MPIEXPORT=" << mpiexport << endl;
        retval = setenv("MPIEXPORT", mpiexport.c_str(), 1);
        if (retval) cerr << "setenv: " << strerror(errno) << endl;
    }
#endif

    if (verbose && !olaunch) cerr << "Launch \"" << launch << "\"" << endl;
    if (verbose && measure) {
        cerr << "Target \"" << launch_options.target;
        if (launch_options.t_args && (string(launch_options.t_args) != ""))
            cerr << spc << launch_options.t_args;
        cerr << "\"" << endl;
        if (!mpietc.empty()) cerr << "MPIetc \"" << mpietc << "\"" << endl;
    }    

    if (measure) {
        string collect = olaunch ? "" : (launch + mpietc + spc);
        if (!escape.empty()) collect += escape + spc;

        if (launcher == "ibrun.symm") collect += "-c \"";
        collect += launch_options.target;
        if (launch_options.t_args && strlen(launch_options.t_args))
            collect += spc + launch_options.t_args;
        if (launcher == "ibrun.symm") {
            if (launch_options.target2) {
                collect += string("\" -m \"") + launch_options.target2;
                if (launch_options.t_args2)
                    collect += spc + launch_options.t_args2;
            }
            collect += "\"";
        }

        if (!blocker.empty()) while (access(blocker.c_str(), F_OK) == 0) {
            cerr << "Measurement start blocked by " << blocker << " ..." << endl;
            sleep(60);
        }

        if (preview) {
            cerr << SCAN << "Collect preview" << endl << collect << endl;
        } else { // actual experiment collection
            ofstream elog;
            string elogfile = epk_title + ".log"; // should be a "unique" name
            string eoutfile = epk_title + ".out"; // only used with redirect

            if (launcher != "cobalt-mpirun") {
                elog.open(elogfile.c_str(), ios::out | ios::trunc);
                if (!elog.good()) {
                    cerr << SCAN << "Abort: failed to open log file " << elogfile << endl;
                    exit(2);
                }
            }

            time(&time1);
            date = ctime(&time1);
            date.erase(date.length()-1);
            cerr << SCAN << date << ": Collect start" << endl << collect << endl;
            if (launcher != "cobalt-mpirun") {
                elog << SCAN << date << ": Collect start" << endl << collect << endl;
                elog.close();

                if (!redirect)
                    collect += " 2>&1 | tee -a " + elogfile;
                else {
                    collect += string(" 1>") + eoutfile + string(" 2>>") + elogfile;
                    cerr << "[launch output redirected to " + epk_archive + "]" << endl;
                }
            }

            retval = system(collect.c_str());
            if (retval == -1) cerr << SCAN << "Error: collect " << strerror(errno) << endl;
            status = WEXITSTATUS(retval);

            time(&time2);
            date = ctime(&time2);
            date.erase(date.length()-1);
            dtime = difftime(time2, time1);
            cerr << SCAN << date << ": Collect done (status=" << status
                 << ") " << dtime << "s" << endl;
            if (launcher != "cobalt-mpirun") {
                elog.open(elogfile.c_str(), ios::out | ios::app);
                if (!elog.good()) {
                    cerr << SCAN << "Error: failed to reopen log file " << elogfile << endl;
                } else {
                    elog << SCAN << date << ": Collect done (status=" << status
                         << ") " << dtime << "s" << endl;
                    elog.close();
                }
            }

            // wait for filesystem sync?
            if (getenv("SCAN_WAIT") && epk_str2bool(getenv("SCAN_WAIT"))) {
                int waits = epk_str2int(getenv("SCAN_WAIT"));
                if (verbose) cerr << "Got SCAN_WAIT=" << waits << endl;
                while (!epk_archive_exists(epk_archive.c_str()) && waits) {
                    cerr << SCAN << "Info: SCAN_WAIT for " << epk_archive << endl;
                    sleep(1);
                    waits--;
                }
            }

            // verify experiment archive created
            if (!epk_archive_exists(epk_archive.c_str())) {
                cerr << "Abort: missing experiment archive " << epk_archive << endl;
                exit(2);
            }

            if (launcher != "cobalt-mpirun") {
                // move log(s) into experiment archive
                // note: rename(2) doesn't work between filesystems
                string mv_command = "mv " + elogfile + " " + epk_archive + "/epik.log";
                retval = system(mv_command.c_str());
                if (retval) cerr << SCAN << mv_command << endl 
                                 << "Warning: " << strerror(errno) << endl;
                else if (verbose) cerr << "Archived " << elogfile << endl;
                if (redirect) {
                    mv_command = "mv " + eoutfile + " " + epk_archive + "/epik.out";
                    retval = system(mv_command.c_str());
                    if (retval) cerr << SCAN << mv_command << endl
                                     << "Warning: " << strerror(errno) << endl;
                    else if (verbose) cerr << "Archived " << eoutfile << endl;
                }
            } else {
                // copy cobalt stderr/stdout logs into experiment archive
                string cjob = string(launch_options.mpicwd) + "/" + getenv("COBALT_JOBID");
                string cp_command = "cp -p " + cjob + ".error " + epk_archive + "/epik.log";
                retval = system(cp_command.c_str());
                if (retval) cerr << SCAN << cp_command << endl 
                                 << "Warning: " << strerror(errno) << endl;
                else if (verbose) cerr << "Archived " << cjob + ".error" << endl;
                cp_command = "cp -p " + cjob + ".output " + epk_archive + "/epik.out";
                retval = system(cp_command.c_str());
                if (retval) cerr << SCAN << cp_command << endl 
                                 << "Warning: " << strerror(errno) << endl;
                else if (verbose) cerr << "Archived " << cjob + ".output" << endl;
            }

            // additional archivable info
            if ((launcher == "yod") || (launcher == "aprun")) {
                string arc_command;
                arc_command = which("xtprocadmin",getenv("PATH"));
                if (!arc_command.empty()) {
                    arc_command += " -A > " + epk_archive + "/xtnids";
                    arc_command += " 2>/dev/null";
                    retval = system(arc_command.c_str());
                    if (retval) { // retry with old command format
                        arc_command = "xtprocadmin > " + epk_archive + "/xtnids";
                        arc_command += " 2>/dev/null";
                        retval = system(arc_command.c_str());
                    }
                    if (WIFEXITED(retval) && (WEXITSTATUS(retval)!=EXIT_SUCCESS))
                        cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                    else if (verbose) cerr << "Archived xtnids" << endl;
                }

                arc_command = which("xtnodestat",getenv("PATH"));
                if (!arc_command.empty()) {
                    arc_command += " > " + epk_archive + "/xtstat";
                    arc_command += " 2>/dev/null";
                    retval = system(arc_command.c_str());
                    if (WIFEXITED(retval) && (WEXITSTATUS(retval)!=EXIT_SUCCESS))
                        cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                    else if (verbose) cerr << "Archived xtstat" << endl;
                }

                arc_command = which("xtshowmesh",getenv("PATH"));
                if (!arc_command.empty()) {
                    arc_command += " > " + epk_archive + "/xtmesh";
                    arc_command += " 2>/dev/null";
                    retval = system(arc_command.c_str());
                    if (WIFEXITED(retval) && (WEXITSTATUS(retval)!=EXIT_SUCCESS))
                        cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                    else if (verbose) cerr << "Archived xtmesh" << endl;
                }

                arc_command = which("xtshowcabs",getenv("PATH"));
                if (!arc_command.empty()) {
                    arc_command += " > " + epk_archive + "/xtcabs";
                    arc_command += " 2>/dev/null";
                    retval = system(arc_command.c_str());
                    if (WIFEXITED(retval) && (WEXITSTATUS(retval)!=EXIT_SUCCESS))
                        cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                    else if (verbose) cerr << "Archived xtcabs" << endl;
                }
            }

            // move lockfile into experiment archive (when it still exists)
            if (!blocker.empty() && (access(blocker.c_str(), F_OK) == 0)) {
                string mv_command = "mv " + blocker + " " + epk_archive;
                size_t sep = blocker.rfind('/');
                if (sep != string::npos) mv_command += blocker.substr(sep);
                retval = system(mv_command.c_str());
                if (retval) cerr << SCAN << mv_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                else if (verbose) cerr << "Archived " << blocker << endl;
            }
        }
    }

    if (!measure || (!preview && measure)) {
        // verify experiment measurement complete
        epk_archive_set_name(epk_archive.c_str());
        if (epk_archive_locked()) {
            cerr << "Abort: incomplete experiment " << epk_archive << endl;
            exit(2);
        } else if (verbose)
            cerr << "Archive " << epk_archive << " is not locked." << endl;
    }

    if (!measure || (!preview && measure && tracing)) {
        // verify that the experiment archive contains (analyzable) trace data
        bool verified=true;
        epk_archive_set_name(epk_archive.c_str());
        string datadir = epk_archive_directory(EPK_TYPE_ELG);
        if (!epk_archive_exists(datadir.c_str())) {
            cerr << "Error: missing trace data " << datadir << endl;
            verified=false;
        } else if (verbose) cerr << "Got trace data " << datadir << endl;
        // verify auxilliary files also exist
        struct stat sbuf;
        string edeffile = epk_archive + "/epik.esd";
        retval = stat(edeffile.c_str(), &sbuf);
        if (retval != 0) {
            cerr << "Error: " << edeffile << ": " << strerror(errno) << endl;
            verified=false;
        } else if (!(sbuf.st_mode & (S_IRUSR|S_IRGRP))) { // readable
            cerr << "Error: unreadable " << edeffile << endl;
            verified=false;
        } else if (sbuf.st_size == 0) {
            cerr << "Error: empty " << edeffile << endl;
            verified=false;
        } else if (verbose) cerr << "Got trace defs " << edeffile << endl;
        string emapfile = epk_archive + "/epik.map";
        retval = stat(emapfile.c_str(), &sbuf);
        if (retval != 0) {
            cerr << "Error: " << emapfile << ": " << strerror(errno) << endl;
            verified=false;
        } else if (!(sbuf.st_mode & (S_IRUSR|S_IRGRP))) { // readable
            cerr << "Error: unreadable " << emapfile << endl;
            verified=false;
        } else if (sbuf.st_size == 0) {
            cerr << "Error: empty " << emapfile << endl;
            verified=false;
        } else if (verbose) cerr << "Got defs map " << emapfile << endl;

        if (verified && epk_inst_mpi && (launcher!="ibrun.symm")) { // check number of process ranks
            int ranks = 0;
            // XXXX NEC SX requires be/elg_print therefore BINDIR prepended to PATH.
            // XXXX Alternatively, could explicitly open epik.esd and parse/count records.
            string scan = string(argv[0]);
            string print_path = which("elg_print", string(BINDIR) + ":"
                                                   + scan.substr(0,scan.rfind('/')) + ":"
                                                   + getenv("PATH"));
            if (print_path.empty()) {
                cerr << SCAN << "Warning: \"elg_print\" executable not found on PATH!" << endl;
            } else {
                string cmd = print_path + spc + edeffile + " 2>/dev/null | grep -c MAP_OFFSET";
                FILE *pipe = popen(cmd.c_str(),"r");
                if (!pipe)
                    cerr << SCAN << "Error: " << cmd << endl << strerror(errno) << endl;
                else {
                    fscanf(pipe, "%d", &ranks);
                    pclose(pipe);
                }
            }
            if (ranks == 0)
                cerr << "Error: counting ranks in " << edeffile << endl;
            else if (verbose) 
                cerr << "Got " << ranks << " ranks in " << edeffile << endl;
            if (ranks && launch_options.nprocs && 
                   (atoi(launch_options.nprocs) != ranks)) {
                cerr << "Warning: epik.esd rank count mis-match: "
                     << ranks << "<>" << launch_options.nprocs << endl;
                if (!getenv("SCAN_MPI_RANKS")) { // don't override!
                    launch_options.nprocs = (char*)calloc(16,1);
                    sprintf(launch_options.nprocs,"%d",ranks);
                    cerr << "Warning: setting SCAN_MPI_RANKS=" << ranks 
                         << " for trace analysis!" << endl;
                    retval = setenv("SCAN_MPI_RANKS", launch_options.nprocs, 0);
                    if (retval) cerr << "setenv: " << strerror(errno) << endl;
                }    
            }
            if (getenv("SCAN_MPI_RANKS")) { // override launch specification
                // XXXX flag might need to depend on MPI launcher
                launch += string(" -n ") + launch_options.nprocs;
            }
        }        
        // XXXX check readability of datadir
        // XXXX check existance/readability/size of required rank data
        // XXXX check writability of scout.cube
        // XXXX warn if existing scout.cube will be overwritten
        if (!verified) {
            cerr << "Abort: " << epk_archive << " archive unsuitable for trace analysis." <<endl;
            exit(4);
        } else if (verbose) cerr << "Verified integrity of " << epk_archive << endl;
    }

    if ((!measure || (measure && tracing)) && (analcmd != "none")) {
        if (analcmd == "expert" || (analyzer.find("scout.ser") != string::npos)) olaunch=true;
        string analyze = (olaunch ? "" : launch + spc);
        if (!escape.empty()) analyze += escape + spc;

        if (launcher == "ibrun.symm") analyze += "-c \"";
        analyze += analyzer;
        if (getenv("SCAN_ANALYZE_OPTS")) // include additional analyzer options
            analyze += spc + string(getenv("SCAN_ANALYZE_OPTS"));
        analyze += spc + epk_archive;
        if (launcher == "ibrun.symm") {
            if (launch_options.target2) {
                size_t pos = analyzer.rfind("/fe/");
                string analyzer_mic = analyzer; analyzer_mic.replace(pos, 4, "/be/");
                analyze += string("\" -m \"") + analyzer_mic;
                if (getenv("SCAN_ANALYZE_OPTS")) // include additional analyzer options
                    analyze += spc + string(getenv("SCAN_ANALYZE_OPTS"));
                analyze += spc + epk_archive;
            }
            analyze += "\"";
        }

        if (launcher == "poe") { // remove "-pgmmodel mpmd" and "-cmdfile <file>"
            string poe_flag = "-pgmmodel ";
	    size_t start = analyze.find(poe_flag);
            size_t end = analyze.find("pmd ",start);
            if (start != string::npos) {
                if (end != string::npos) {
                    if (verbose) cerr << "Eliminating incompatible POE option: " 
                                      << analyze.substr(start,end+4-start) << endl;
                    analyze.erase(start,end+4-start);
		} else {
                    cerr << "Warning: trace analyzer " << analcmd
                         << " likely incompatible with POE " << poe_flag << " option!" << endl;
		}
            }

            poe_flag = "-cmdfile ";
            start = analyze.find(poe_flag);
            if (start != string::npos) {
                end = analyze.find_first_of(' ',start+poe_flag.length()+1);
                if (end != string::npos) {
                    if (verbose) cerr << "Eliminating incompatible POE option: " 
                                      << analyze.substr(start,end-start) << endl;
                    analyze.erase(start,end-start);
		} else {
                    cerr << "Warning: trace analyzer " << analcmd
                         << " likely incompatible with POE " << poe_flag << " option!" << endl;
                }
            }
        }

        if (preview) {
            cerr << SCAN << "Analyze preview" << endl << analyze << endl;
        } else { // actual experiment analysis
            ofstream slog, serr;
            string slogfile = epk_archive + "/" + analcmd + ".log";
            string serrfile = epk_archive + "/" + analcmd + ".err";
            slog.open(slogfile.c_str(), ios::out | ios::app);
            if (!slog.good()) {
                cerr << SCAN << "Abort: failed to open log file " << slogfile << endl;
                exit(2);
            }
            time(&time1);
            date = ctime(&time1);
            date.erase(date.length()-1);
            cerr << SCAN << date << ": Analyze start" << endl << analyze << endl;
            slog << SCAN << date << ": Analyze start" << endl << analyze << endl;
            slog.close();

            string cobalt_mpirun = "cobalt-mpirun";
            size_t cobalt_pos = analyze.find(cobalt_mpirun);
            if (cobalt_pos != string::npos) {
                string redirection = string(" --stderr ") + serrfile
                                   + string(" --stdout ") + slogfile;
                analyze.insert(cobalt_pos+cobalt_mpirun.length(), redirection);
                cerr << "[" << analyze << "]" << endl;
            } else {
                analyze += string(" 2>>") + serrfile;
                if (!redirect)
                    analyze += string(" | tee -a ") + slogfile;
                else {
                    analyze += string(" 1>>") + slogfile; 
                    cerr << "[launch output redirected to " + slogfile + "]" << endl;
                }
            }
#ifndef EXPERT_MERGE
            if (analcmd == "expert") { // explicit merge while expert is unable to
                setenv("EPK_VERBOSE", "1", 0); // no override
                string merging = "elg_merge" + spc + epk_archive;
                if (verbose) cerr << merging << endl;
                merging += string(" 2>>") + serrfile + string(" | tee -a ") + slogfile;
                retval = system(merging.c_str());
                if (retval == -1) cerr << SCAN << "Error: merging " << strerror(errno) << endl;
                status = WEXITSTATUS(retval);
                if (status) cerr << SCAN << "merging returned " << status << endl;
                unsetenv("EPK_VERBOSE");
            }
#endif
#ifdef _AIX
            // multithreaded analysis can be very slow with default malloc
            if ((analyzer.find("scout.omp") != string::npos) || 
                (analyzer.find("scout.hyb") != string::npos)) {
                if (verbose) cerr << "Setting MALLOCOPTIONS=multiheap" << endl;
                retval = setenv("MALLOCOPTIONS", "multiheap", 1);
                if (retval) cerr << "setenv: " << strerror(errno) << endl;
            }
#endif
            retval = system(analyze.c_str());
            if (retval == -1) cerr << SCAN << "Error: analyze " << strerror(errno) << endl;
            status = WEXITSTATUS(retval);

            time(&time2);
            date = ctime(&time2);
            date.erase(date.length()-1);
            dtime = difftime (time2, time1);
            cerr << SCAN << date << ": Analyze done (status=" << status
                 << ") " << dtime << "s" << endl;
            slog.open(slogfile.c_str(), ios::out | ios::app);
            if (!slog.good()) {
                cerr << SCAN << "Error: failed to reopen log file " << slogfile << endl;
            } else {
                slog << SCAN << date << ": Analyze done (status=" << status
                     << ") " << dtime << "s" << endl;
                slog.close();
            }
            
#ifdef BG_NOFREE
// XXXX freeing doesn't seem to be necessary, 
// and indeed appears to introduce problems!
            if (measure && !nofree) { // BlueGene-specific partition control
                retval = unsetenv("MPIRUN_NOFREE"); // undo measurement setting
                if (retval) cerr << "unsetenv(MPIRUN_NOFREE): " << strerror(errno) << endl;
                string freeing = "mpirun -free wait";
                if (verbose) cerr << "Freeing partition: " << freeing << endl;
                retval = system(freeing.c_str());
                if (retval == -1) cerr << SCAN << "Error: freeing " << strerror(errno) << endl;
                status = WEXITSTATUS(retval);
                if (status) cerr << SCAN << "freeing returned " << status << endl;
            }
#endif

            // remove analysis errorfile if empty
            struct stat sbuf;
            stat(serrfile.c_str(), &sbuf); // ignore failure
            if (sbuf.st_size == 0) unlink(serrfile.c_str()); // ignore failure

            // verify trace analysis report created
            bool verified=true;
            string analfile = epk_archive + "/" + analcmd + ".cube";
#ifdef CUBE_COMPRESSED
            if (analcmd == "expert")
              analfile += ".gz";
#endif
            retval = stat(analfile.c_str(), &sbuf);
            if (retval != 0) {
                cerr << "Error: " << analfile << ": " << strerror(errno) << endl;
                verified=false;
            } else if (!(sbuf.st_mode & (S_IRUSR|S_IRGRP))) { // readable
                cerr << "Error: unreadable " << analfile << endl;
                verified=false;
            } else if (sbuf.st_size == 0) {
                cerr << "Error: empty " << analfile << endl;
                verified=false;
            } else if (verbose) cerr << "Got trace analysis report " << analfile << endl;

            if (!verified) {
                cerr << "Abort: " << epk_archive << " trace analysis failed!" << endl;
                cerr << serrfile << ":" << endl;
                string report = "cat " + serrfile;
                system(report.c_str());
                exit(4);
            } else {
                off_t kbytes;
                string cmd = "du -sk " + epk_archive + "/ELG";
                if (verbose) cerr << "[" << cmd << "]" << endl;
                FILE *pipe = popen(cmd.c_str(),"r");
                fscanf(pipe, "%lu", &kbytes);
                pclose(pipe);

                ostringstream bytes("");
                bytes << fixed << setprecision(3);
                if      (kbytes > (1<<30)) bytes << (float)kbytes/(1<<30) << "TB";
                else if (kbytes > (1<<20)) bytes << (float)kbytes/(1<<20) << "GB";
                else if (kbytes > 0)       bytes << (float)kbytes/(1<<10) << "MB";
                // if "du" returned zero, ignore size report
                if (kbytes) bytes << " of ";
                
                char* clean = getenv("SCAN_CLEAN");
                if (clean && epk_str2bool(clean)) {
                    if (verbose) cerr << "Got SCAN_CLEAN=" << clean << endl;
                    cmd = "/bin/rm -rf " + epk_archive + "/ELG";
                    if (verbose) cerr << "[" << cmd << "]" << endl;
                    retval = system(cmd.c_str());
                    status = WEXITSTATUS(retval);
                    if (status) cerr << cmd << ": " << strerror(status) << endl;
                    else cerr << "Info: " << bytes.str() << "analyzed trace data cleaned from " 
                              << epk_archive << "." << endl;
                } else {                
                    cerr << "Warning: " << bytes.str() << "analyzed trace data retained in " 
                         << epk_archive << "/ELG!" << endl;
                }
            }
        }
    }

    cerr << SCAN << epk_archive << (preview ? " preview" : "") << " complete." << endl;

    return 0;
}
