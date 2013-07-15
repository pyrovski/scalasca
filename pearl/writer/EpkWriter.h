/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_EPKWRITER_H
#define PEARL_EPKWRITER_H


#include <string>

#include <elg_rw.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    EpkWriter.h
 * @ingroup PEARL_writer
 * @brief   Declaration of the class EpkWriter.
 *
 * This header file provides the declaration of the class EpkWriter.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CallbackData;
class CallbackManager;
class Event;
class GlobalDefs;
class LocalTrace;


class EpkWriter
{
  public:
    /// @name Constructors & destructor
    /// @{

    EpkWriter();
    ~EpkWriter();

    /// @}
    /// @name Writing experiment data
    /// @{

    void write(const std::string& archive,
               const LocalTrace&  trace,
               const GlobalDefs&  defs);

    /// @}


  protected:
    /// File handle for the definition file
    ElgOut* m_esdFile;

    /// File handle for the trace file
    ElgOut* m_elgFile;

    /// %Callback manager instance for the replay
    CallbackManager* m_cbmanager;

    /// MPI rank of the calling process
    int m_rank;


  private:
    /// @name Replay callback routines
    /// @{

    void enter(const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void exit (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void recv (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void send (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void fork (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void join (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void alock(const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);
    void rlock(const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* cdata);

    void recv_request  
              (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* data);
    void send_complete 
              (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* data);
    void request_tested
              (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* data);
    void cancelled     
              (const CallbackManager& cbmanager, int user_event,
               const Event& event, CallbackData* data);

    /// @}
    /// @name Internal I/O routines
    /// @{

    void prepare_archive  (const std::string& archive);
    void write_traces     (const GlobalDefs&  defs,
                           const LocalTrace&  trace);
    void write_definitions(const GlobalDefs&  defs);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_EPKWRITER_H */
