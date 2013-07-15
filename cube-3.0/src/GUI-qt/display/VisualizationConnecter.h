/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef VISUALIZATION_CONNECTER_H
#define VISUALIZATION_CONNECTER_H

#include <string>

class VisualizationConnecter
{
    public:
        virtual std::string InitiateAndOpenTrace() = 0;
        virtual std::string ZoomIntervall(double start, double end, int zoomNumber) = 0;
        virtual bool IsActive() const = 0;
        virtual ~VisualizationConnecter(){};
};

#endif

