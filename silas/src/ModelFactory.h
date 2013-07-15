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

#ifndef SILAS_MODEL_FACTORY_H
#define SILAS_MODEL_FACTORY_H

#include <string>
#include "Model.h"

namespace silas {

    class ModelFactory
    {
        public:
            static silas::Model* create_model(const std::string& model);
    };
}

#endif
