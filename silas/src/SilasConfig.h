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

#ifndef SILAS_CONFIG_H
#define SILAS_CONFIG_H

#include <vector>
#include <string>


/**
 * @file
 * 
 * @author Marc-Andre Hermanns <m.a.hermanns@fz-juelich.de>
 */

namespace silas
{
    /** 
     * A container class for the simulator configuration
     */
    class Config
    {
        public:
            /**
             * Constructor for the configuration
             */
            Config();
            /**
             * Standard destructor for the configuration
             */
            virtual ~Config();
            /**
             * Check if this configuration is of a certain type;
             * @param type String identifier of a certain config type
             */
            virtual bool is_type(std::string type);
            /**
             * Set the input trace name
             * @param name Filename of the input trace
             */
            void set_input_trace(std::string name);
            /**
             * Get the input trace name
             * @return Filename of the input trace
             */
            std::string get_input_trace();
            /**
             * Set the output trace name
             * @param name Filename of the output trace
             */
            void set_output_trace(std::string name);
            /**
             * Get the output trace name
             * @return Filename of the output trace
             */
            std::string get_output_trace();

        private:
            /**
             * Input trace name
             */
            std::string m_input_trace_name;
            std::string m_output_trace_name;
    };
}

#endif

