/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "ParaverConnecter.h"

#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <string.h>

using namespace std;

ParaverConnecter::ParaverConnecter(string const &file,
    string const &configFile, bool verb)
    : childID(0), fileName(file), configFileName(configFile), verbose(verb)
{
}

string ParaverConnecter::InitiateAndOpenTrace()
{
    childID = fork();
    if(childID == -1)
    {
        return "ParaverConnecter: Could not fork.";
    }
    if(childID == 0)
    {
        char *name = new char[fileName.length() + 1];
        strcpy(name, fileName.c_str());
        char *configName = new char[configFileName.length() + 1];
        strcpy(configName, configFileName.c_str());
        char *arguments[] = { (char*) "paraver", name, configName, 0};
        execvp("wxparaver", arguments);
        cerr << "Paraver could not be started. Error: "
             << strerror(errno) << endl;
        delete [] configName;
        delete[] name;
        exit(1);
    }
    else
    {
        return "";
    }
}

string ParaverConnecter::ZoomIntervall(double start, double end, int zoomStep)
{
    if(verbose)
    {
        cout << "Zooming paraver instance with programm id " << childID
             << " to interval [" << start << " seconds, "
             << end << " seconds]" << endl;
    }
    if(zoomStep > 1)
    {
        sleep(2);
    }
    if(commFileName.length() == 0)
    {
        if(getenv("HOME") == 0)
        {
            return "No home dir defined. Needed for the paraver config file.";
        }
        string homeDir = getenv("HOME");
	commFileName = homeDir + "/paraload.sig";
    }
    ofstream theFile(commFileName.c_str());
    theFile << configFileName << endl;
    //to get from seconds to nanoseconds
    double const paraMultiplier = 1e9;
    theFile << static_cast<long long>(start * paraMultiplier) << ":"
            << static_cast<long long>(end * paraMultiplier) << endl;
    theFile.close();
    if(zoomStep == 1)
    {
        //Send paraver the signal SIGUSR1 to make it read the configuration file
	//and open a new window
        kill(childID, SIGUSR1);
    }
    else if(zoomStep > 1)
    {
        //Send paraver the signal SIGUSR2 to make it read the configuration file        //and zoom in the previous window
        kill(childID, SIGUSR2);
    }
    return "";
}

bool ParaverConnecter::IsActive() const
{
    //FIXME: Subject to change when a method is provided to check if paraver
    //has completetly loaded the trace
    return true;
}

ParaverConnecter::~ParaverConnecter()
{
    if(commFileName.length())
    {
        unlink(commFileName.c_str());
    }
    kill(childID, SIGTERM);
}

