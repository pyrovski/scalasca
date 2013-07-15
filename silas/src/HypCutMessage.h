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

#ifndef SILAS_HYP_CUT_MESSAGE_H
#define SILAS_HYP_CUT_MESSAGE_H

#include <map>
#include "CallbackManager.h"
#include "ReenactCallbackData.h"

namespace silas
{

    /**
     * MessageSurgeon the timespan of atomic regions
     */
    class HypCutMessage : public HypothesisPart
    {
        typedef std::map<std::string, int> registration_t;
        public:
            /// @name Constructors & Destructors
            /// @{
            
            HypCutMessage();
            virtual ~HypCutMessage();
            
            /// @}
            /**
             * Register the needed callbacks to enable the management of
             * user defined modifications on specific events.
             */
            virtual void register_callbacks(const int run, pearl::CallbackManager* cb_manager);
            
            void register_message_size(const std::string& rel, int rval);
            void register_message_tag(const std::string& rel, int rval);
        private:
            /**
             * callback to check and perform removal of sends
             */
            void remove_message(const pearl::Event& exit, 
                                silas::CallbackData* data);
            void check_call_removal(const pearl::Event& exit,
                                silas::CallbackData* data);
            void cb_noop(const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);
            void cb_check_send(const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);
            /**
             * callback to check and perform removal of receives
             */
            void cb_check_recv(const pearl::CallbackManager& cbmanager,
                    int user_event, const pearl::Event& event, 
                    pearl::CallbackData* cdata);

            /// @name Comparison functions
            /// @{
            bool cmp(const std::string& rel, int a, int b);
            bool ne(int a, int b) { return a != b; }
            bool eq(int a, int b) { return a == b; }
            bool lt(int a, int b) { return a < b; }
            bool le(int a, int b) { return a <= b; }
            bool gt(int a, int b) { return a > b; }
            bool ge(int a, int b) { return a >= b; }
            bool is_registered(const pearl::Event& event);
            /// @}

            registration_t m_reg_msg_size;
            registration_t m_reg_msg_tag;
    };
}

#endif



