/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <exception>
#include <memory>
#include <new>
#include <string>

#include <unistd.h>
#include <sys/stat.h>

#include <mpi.h>

#include "elg_error.h"
#include "epk_conf.h"
#include "epk_archive.h"

#include "Error.h"
#include "GlobalDefs.h"
#include "LocalTrace.h"
#include "EpkWriter.h"
#include "pearl.h"
#include "pearl_types.h"

#include "Timer.h"
#include "Simulator.h"
#include "Hypothesis.h"
#include "Model.h"
#include "ReenactModel.h"
#include "SilasConfig.h"

// partial hypotheses
#include "HypBalance.h"
#include "HypCutMessage.h"
#include "HypScale.h"

using namespace std;
using namespace pearl;
using namespace silas;

//--- Forward declarations --------------------------------------------------

void simulate_trace(LocalTrace& trace);

//--- Constants -------------------------------------------------------------

const char* copyright = "Copyright (c) 1998-2013 Forschungszentrum Juelich GmbH";


//--- Global variables ------------------------------------------------------

int rank;

/* Using reenact model for simulation */
Model*          model      = NULL;
/* Using an empty hypothesis will result in identity simulation */
Hypothesis*            hypothesis = new Hypothesis(); 
/* Hypothesis Part to simulate load balance */
HypBalance* balancer   = new HypBalance();
/* Hypothesis Part to cut zero sized messages */
HypCutMessage* message_surgeon = new HypCutMessage();
/* Region scaler */
HypScale* rescaler = new HypScale();

//--- Function prototypes ---------------------------------------------------

int main(int argc, char** argv);

extern "C" void cleanup();
void log(const char* fmt, ...);
void global_error(bool error, const string& message);

GlobalDefs* read_definitions();
LocalTrace* read_trace(const GlobalDefs& defs);
void preprocess_trace(GlobalDefs& defs, LocalTrace& trace);
void simulate_trace(LocalTrace& trace);
void write_trace(GlobalDefs& defs, LocalTrace& trace, const char* filename);


extern void parse_config(const std::string& filename);

/*
 *---------------------------------------------------------------------------
 *
 * SILAS - SImulation of LArge Scale applications
 *
 *---------------------------------------------------------------------------
 */

void show_help(int exitcode)
{
    log("Usage: silas [options]\n\n"
        "Options:\n"
        "    -c <config>   Configuration file to use for simulation\n"
        "    -h            Show this help and exit\n"
        "    -i            Input trace directory\n"
        "    -o            Output trace directory\n"
        );

    exit(exitcode);
}

void parse_commandline(int argc, char** argv, silas::Config* config)
{
    for (int i = 1; i < argc; i++)
    {
        /* check for options */
        if (argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'c':
                {
                    /* parse the configuration file given in the next
                     * commandline option */
                    if (i < argc)
                        parse_config(argv[++i]);
                    break;
                }
                case 'h':
                {
                    /* show help and exit */
                    show_help(EXIT_SUCCESS);
                    break;
                }
                case 'i':
                {
                    if (i < argc)
                        config->set_input_trace(argv[++i]);
                    break;
                }
                case 'o':
                {
                    if (i <argc)
                        config->set_output_trace(argv[++i]);
                    break;
                }
                default:
                {
                    /* unknown option */
                    cerr << "Unknown option '-" << argv[i][1] << "'\n";
                    show_help(EXIT_FAILURE);
                }
            }
        }
        else
        {
            
        }
    }
}

int main(int argc, char** argv)
{
    // Initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    PEARL_mpi_init();
    elg_error_pid(rank);
    atexit(cleanup);
    Timer total_time;
    Config* config = new Config();

    /* display copyright notice */
    log("SILAS   %s\n\n", copyright);

    parse_commandline(argc, argv, config);

    /* validate and open epik trace archive */
    global_error(!epk_archive_set_name(config->get_input_trace().c_str()), string("Archive \"")
            + argv[1] + "\" undefined!");
    global_error(!epk_archive_exists(NULL), string("Archive \"")
            + epk_archive_get_name() + "\" does not exist!");
    log("Using experiment archive %s\n\n", epk_archive_get_name());
    
    /* read event definitions */
    std::auto_ptr<GlobalDefs> defs(read_definitions());
    /* read local traces to memory */
    std::auto_ptr<LocalTrace> trace(read_trace(*defs));

    /* preprocess trace for simulation: unification, etc. */
    preprocess_trace(*defs, *trace);
    /* trigger simulation */
    simulate_trace(*trace);

    /* write trace to disk */
    write_trace(*defs, *trace, config->get_output_trace().c_str());
  
    log("\nTotal processing time: %s\n\n", total_time.value_str().c_str());

    delete config;
}


//--- Utility functions -----------------------------------------------------

/**
 * Providing a graceful exit by cleaning up the parallel environment,
 * e.g. calling MPI_Finalize;
 */
void cleanup()
{
  MPI_Finalize();
}

/**
 * Logging function
 */
void log(const char* fmt, ...)
{
    va_list ap;

    // Display log message on master node
    va_start(ap, fmt);
    if (rank == 0) 
    {
        vprintf(fmt, ap);
        fflush(NULL);
    }
    va_end(ap);
}

/**
 * Checking whether this error is a global error or not
 * @param error Flag indicating an error
 * @param message Message string indicating the cause of the error
 */
void global_error(bool error, const string& message)
{
    string msg    = "SILAS: " + message;
    int    local  = error ? 1 : 0;
    int    global = 0;
    int    size;

    // Check whether it is a global error or not
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Allreduce(&local, &global, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // No error at all
    if (0 == global)
        return;

    // Display error message and exit
    if (global == size)
        elg_cntl_msg(msg.c_str());
    if ((global != size && error) || (global == size && rank == 0))
        elg_error_msg(msg.c_str());
    exit(EXIT_FAILURE);
}

/**
 * Read global definitions
 * return Pointer to GlobalDefs object containing the global definitions
 */
GlobalDefs* read_definitions()
{
    log("Reading definitions file   ... ");
    Timer read_time;

    // Load definition file only on rank 0
    string   message;
    bool     error  = false;
    uint8_t* buffer = NULL;
    long     size;
    if (rank == 0) 
    {
        try 
        {
            char* name = epk_archive_filename(EPK_TYPE_ESD, 
                    epk_archive_get_name());

            // Determine file size
            struct stat info;
            if (stat(name, &info) != 0)
                throw FatalError("Unable to determine trace definition file size.");
            size = info.st_size;

            // Allocate buffer
            buffer = new uint8_t[size];

            // Read definition file
            FILE* fp = fopen(name, "rb");
            if (!fp || fread(buffer, size, 1, fp) != 1)
                throw FatalError("Error reading trace definition file.");
            fclose(fp);

            free(name);
        }
        catch (const exception& ex) 
        {
            message = ex.what();
            error   = true;
        }
    }
    // Check for errors
    global_error(error, message);

    // Distribute definition data & initialize GlobalDefs object
    GlobalDefs* result = NULL;
    try 
    {
        MPI_Bcast(&size, 1, MPI_LONG, 0, MPI_COMM_WORLD);
        if (rank != 0)
            buffer = new uint8_t[size];
        MPI_Bcast(buffer, size, MPI_BYTE, 0, MPI_COMM_WORLD);

        result = new GlobalDefs(buffer, size);
        delete[] buffer;
    }
    catch (const exception& ex) 
    {
        message = ex.what();
        error   = true;
    }

    // Check for errors
    global_error(error, message);
    log("done (%s).\n", read_time.value_str().c_str());

    return result;
}

/**
 * Read event trace into memory
 * @param defs Reference to global definitions
 * @return Pointer to LocalTrace object containing the event stream
 */
LocalTrace* read_trace(const GlobalDefs& defs)
{
    log("Reading event trace files  ... ");
    Timer read_time;

    // Read file
    string      message;
    bool        error  = false;
    LocalTrace* result = NULL;
    try 
    {
        result = new LocalTrace(defs, epk_archive_get_name(), rank);
    }
    catch (const exception& ex) 
    {
        message = ex.what();
        error   = true;
    }

    // Check for errors
    global_error(error, message);
    log("done (%s).\n", read_time.value_str().c_str());

    return result;
}

/**
 * Preprocessing of trace data, e.g. call tree unification.
 * @param defs Reference on global definitions
 * @param trace Reference on event trace
 */
void preprocess_trace(GlobalDefs& defs, LocalTrace& trace)
{
    log("Preprocessing local traces ... ");
    Timer preprocess_time;


    string message;
    bool   error = false;

    // Verify call trees & check for errors
    try 
    {
        PEARL_verify_calltree(defs, trace);
    }
    catch (const exception& ex) 
    {
        message = ex.what();
        error   = true;
    }
    global_error(error, message);

    // Unify call trees & check for errors
    try 
    {
        PEARL_mpi_unify_calltree(defs);
    }
    catch (const exception& ex) 
    {
        message = ex.what();
        error   = true;
    }
    global_error(error, message);

    // Preprocess trace data & check for errors
    try 
    {
        PEARL_preprocess_trace(defs, trace);
    }
    catch (const exception& ex) 
    {
        message = ex.what();
        error   = true;
    }
    global_error(error, message);

    log("done (%s).\n", preprocess_time.value_str().c_str());
}

/**
 * Start simulation process
 * @param trace Reference to event trace
 */
void simulate_trace(LocalTrace& trace)
{
    /* Creating simulator object with model and hypothesis */
    Simulator simulator(trace, model, hypothesis);
    /* Time object to time overall simulation time */
    Timer     simulation_time;
   
    hypothesis->add_part(balancer);
    hypothesis->add_part(message_surgeon);
    hypothesis->add_part(rescaler);
    
    /* start simulation */
    string  message;
    bool    error = false;
    try
    {
        simulator.run();
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }
    global_error(error, message);
}

/**
 * Writing the predicted event trace to disk
 * @param defs Reference to global definitions
 * @param trace Reference to event trace
 * @param filename Name of the experiment archive
 */
void write_trace(GlobalDefs& defs, LocalTrace& trace, const char* filename)
{
    Timer   writing_time;
    string  message;
    bool    error = false;
    
    /* write modified trace */
    log("Writing predicted trace    ... ");
    writing_time.start();
    try 
    {
        /* write trace */
        EpkWriter* writer = new EpkWriter();
        writer->write(filename, trace, defs);
    }
    catch (const exception& ex)
    {
        message = ex.what();
        error   = true;
    }
    global_error(error, message);
    log("done (%s).\n", writing_time.value_str().c_str());
}
