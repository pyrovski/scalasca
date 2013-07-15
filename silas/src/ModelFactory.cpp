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

#include <cstddef>
#include <string>

#include "ModelFactory.h"
#include "ReenactModel.h"

using namespace std;
using namespace silas;

Model* ModelFactory::create_model(const string& model)
{
    if (model == "Reenact")
        return new ReenactModel();
        
    return NULL;
}
