/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef PARAVER_CONNECTER_H
#define PARAVER_CONNECTER_H

#include "VisualizationConnecter.h"

#include <string>

class ParaverConnecter : public VisualizationConnecter
{
    public:
        ParaverConnecter(std::string const &file, std::string const &configFile,
                         bool verbose);
        virtual std::string InitiateAndOpenTrace();
        virtual std::string ZoomIntervall(double start, double end, int zoomStep);
        virtual bool IsActive() const;
        virtual ~ParaverConnecter();
    private:
        int childID;
        std::string fileName;
        std::string configFileName;
        std::string commFileName;
        bool verbose;
};

#endif

