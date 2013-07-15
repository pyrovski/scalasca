/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "VampirConnecter.h"

#include <iostream>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

using namespace std;

bool VampirConnecter::busNameRegistered = false;

VampirConnecter::VampirConnecter(std::string const &theBusName,
      std::string const &serverName, dbus_uint32_t portNumber,
      std::string const &fileName, bool verb)
    : connection(0), pending(0), busName(theBusName),
      objectName("/com/gwt/vampir"), interfaceName("com.gwt.vampir"),
      server(serverName), port(portNumber), file(fileName),
      isActive(false), verbose(verb)
{
    {
        size_t const pointPosition = file.find_last_of('.');
        string const extension = file.substr(pointPosition);
        if(extension == ".elg" || extension == ".esd")
        {
            fileType = ELG;
        }
        else if(extension == ".otf")
        {
            fileType = OTF;
        }
        else
        {
            //should never happen
            fileType = undefined;
        }
    }
    DBusError error;
    dbus_error_init(&error);
    //connect to the bus and check for errors
    connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    //Prevent the application from being closed when the connection closes
    dbus_connection_set_exit_on_disconnect(connection,  false);
    CheckError(error);
    if(connection == 0)
    {
        throw VampirConnecterException("Connection failed");
    }
    if(!busNameRegistered)
    {
        int ret = dbus_bus_request_name(connection,
                      "com.gwt.CUBE-VampirConnecter",
                      DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
        CheckError(error);
        if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
        {
            throw VampirConnecterException("No primary owner!");
        }
        busNameRegistered = true;
    }
    dbus_error_free(&error);
}

string VampirConnecter::InitiateAndOpenTrace()
{
    //tell dbus that we are interested in signals from vampir
    //(in order to check for the 'traceFileLoaded' signal below)
    string rule = "type='signal',interface='" + interfaceName + "'";
    DBusError err;
    dbus_error_init(&err);
    dbus_bus_add_match(connection, rule.c_str(), &err);
    if(dbus_error_is_set(&err))
    {
        dbus_error_free(&err);
        return "DBUSerror while waiting for vampir.";
    }
    else
    {
        dbus_error_free(&err);
    }

    //check the opening method
    //if no servername and port was set, we open a local file
    if(server == "" && port == 0)
    {
        if(!OpenLocalTraceFile(file))
        {
            return "Local trace file could not be opened.";
        }
    }
    else if(!OpenRemoteTraceFile(file, server, port))
    {
        return "Remote trace file could not be opened.";
    }

    //wait a second to give vampir some time for initializing all default start displays
    sleep(1);

    if(!OpenDisplay(Timeline))
    {
        return "Timeline display could not be opened.";
    }

    isActive = true;
    return "";
}

string VampirConnecter::ZoomIntervall(double start, double end, int step)
{
    //TODO - we do not need the step parameter anymore! vampir has a own undo zoom feature.
    //so we must not save the steps to undo any zoom intervall

    if(verbose)
    {
        cout << "Zooming vampir on bus name " << busName << " to intervall ["
             << start << " seconds, " << end << " seconds]" << endl;
    }
    if(!ExistsVampirWithBusName(busName))
    {
        //Ignore closed instances of vampir
        if(verbose)
        {
            cout << "Ignoring closed instance of vampir on bus name "
                 << busName << endl;
        }
        return "";
    }
    if (step == 1)
    {
      if(!OpenDisplay(Timeline))
      {
          return "Timeline display could not be opened.";
      }
    }
    if(!ZoomDisplay(Timeline, start, end))
    {
        return "Remote zoom failed.";
    }
    return "";
}

void VampirConnecter::CheckError(DBusError &error)
{
    if(dbus_error_is_set(&error))
    {
        string message = error.message;
        dbus_error_free(&error);
        throw VampirConnecterException(message);
    }
}

void VampirConnecter::InitiateCommunication(string const &methodName)
{
    message = dbus_message_new_method_call(busName.c_str(), objectName.c_str(),
                   interfaceName.c_str(), methodName.c_str());
    if(message == 0)
    {
        throw VampirConnecterException(
              "Connect message null while initiating communication for method "
              + methodName);
    }
    messageIterator = DBusMessageIter();
    dbus_message_iter_init_append(message, &messageIterator);
}

void VampirConnecter::CompleteCommunicationGeneric(bool block)
{
    if(!dbus_connection_send_with_reply(connection, message, &pending, -1))
    {
        throw VampirConnecterException(
              "Out of memory while trying to send DBus message");
    }
    if(pending == 0)
    {
        throw VampirConnecterException(
              "Pending call null while trying to send DBus message");
    }
    dbus_connection_flush(connection);
    dbus_message_unref(message);
    message = 0;

    //block until reply is received
    if(block)
    {
        dbus_pending_call_block(pending);
        message = dbus_pending_call_steal_reply(pending);
        if(message == 0)
        {
            throw VampirConnecterException("Connection reply is null");
        }
        dbus_pending_call_unref(pending);
        pending = 0;
    }
}

bool VampirConnecter::CompleteCommunication(bool block, bool )
{
    CompleteCommunicationGeneric(block);

    if(block)
    {
        bool reply;
        if(!dbus_message_iter_init(message, &messageIterator))
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication: "
                   << "dbus_message_iter_init failed. Returning false." << endl;
            return false;
        }
        else if(dbus_message_iter_get_arg_type(&messageIterator) != DBUS_TYPE_BOOLEAN)
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication: Message argument "
                   << "type is not bool. Returning false." << endl;
            return false;
        }
        else
        {
            dbus_message_iter_get_basic(&messageIterator, &reply);
            if(verbose)
              cout << "In CompleteCommunication: reply = "
                   << boolalpha << reply << endl;
            if(reply)
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool, bool) : "
                       << "returning true" << endl;
                return true;
            }
            else
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool, bool) : "
                       << "returning false" << endl;
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool VampirConnecter::CompleteCommunication(bool block, dbus_uint32_t_list **successDBusUint32List)
{
    CompleteCommunicationGeneric(block);

    if(block)
    {
        DBusMessageIter index;
        if(!dbus_message_iter_init(message, &messageIterator))
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool uint32_list):"
                   << "dbus_message_iter_init failed. Returning false." << endl;
            return false;
        }
        else if(dbus_message_iter_get_arg_type(&messageIterator) != DBUS_TYPE_ARRAY)
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool uint32_list): Message argument "
                   << "type is not array. Returning false." << endl;
            return false;
        }
        else
        {
            int counter = 0;

            dbus_message_iter_recurse(&messageIterator, &index);
            if(DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type(&index))
            {
                (*successDBusUint32List) = new dbus_uint32_t_list;
                (*successDBusUint32List)->next = NULL;
                dbus_message_iter_get_basic(&index, &(*successDBusUint32List)->data);

                while(dbus_message_iter_next(&index)){
                    if(DBUS_TYPE_UINT32 != dbus_message_iter_get_arg_type(&index)){
                        dbus_message_unref(message);
                        if(verbose)
                          cout << "In CompleteCommunication (bool uint32_list):"
                               << "wrong argument in list at position " << counter <<  ". Returning false." << endl;
                        return false;
                    }
                    else{
                        successDBusUint32List = &(*successDBusUint32List)->next;

                        (*successDBusUint32List) = new dbus_uint32_t_list;
                        (*successDBusUint32List)->next = NULL;
                        dbus_message_iter_get_basic(&index, &(*successDBusUint32List)->data);
                    }
                    counter++;
                }
            }
            else{
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool uint32_list):"
                       << "wrong argument in list at position " << counter <<  ". Returning false." << endl;
                return false;
            }
        }
    }

    return true;
}

bool VampirConnecter::CompleteCommunication(bool block, char **successCharStringValue)
{
    CompleteCommunicationGeneric(block);
    if(block)
    {
        if(!dbus_message_iter_init(message, &messageIterator))
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool char*): "
                   << "dbus_message_iter_init failed. Returning false." << endl;
            return false;
        }
        else if(dbus_message_iter_get_arg_type(&messageIterator) != DBUS_TYPE_STRING)
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool char*): Message argument "
                   << "type is not char*."<< endl;
            return false;
        }
        else
        {
            char *buffer;
            dbus_message_iter_get_basic(&messageIterator, &buffer);
            *successCharStringValue = (char*)calloc(strlen(buffer) + 1,sizeof(char));
            strcpy(*successCharStringValue,buffer);

            if(verbose)
              cout << "In CompleteCommunication (bool char*): reply = "
                   << *successCharStringValue << endl;

            dbus_message_unref(message);
            return true;
        }
    }
    //If we dont wait for the answer
    return true;
}

bool VampirConnecter::CompleteCommunication(bool block,
     string const &successMessageBeginning)
{
    CompleteCommunicationGeneric(block);
    if(block)
    {
        int const maxMessageLength = 128;
        char *buffer = new char[maxMessageLength];
        if(!dbus_message_iter_init(message, &messageIterator))
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool string): "
                   << "dbus_message_iter_init failed. Returning false." << endl;
            return false;
        }
        else if(dbus_message_iter_get_arg_type(&messageIterator)
                != DBUS_TYPE_STRING)
        {
            dbus_message_unref(message);
            if(verbose)
            cout << "In CompleteCommunication (bool string): Message argument "
                 << "type is not string."<< endl;
            return false;
        }
        else
        {
            dbus_message_iter_get_basic(&messageIterator, &buffer);
            string receivedMessage = buffer;
            if(verbose)
              cout << "In CompleteCommunication (bool string): receivedMessage = "
                   << receivedMessage << endl;
            if(receivedMessage.find(successMessageBeginning) == 0)
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool, string) : "
                       << "returning true" << endl;
                return true;
            }
            else
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool, string) : "
                       << "returning false" << endl;
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool VampirConnecter::CompleteCommunication(bool block, dbus_uint32_t *successUint32Value)
{
    CompleteCommunicationGeneric(block);
    if(block)
    {
        if(!dbus_message_iter_init(message, &messageIterator))
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool uint32): "
                   << "dbus_message_iter_init failed. Returning false." << endl;
            return false;
        }
        else if(dbus_message_iter_get_arg_type(&messageIterator) != DBUS_TYPE_UINT32)
        {
            dbus_message_unref(message);
            if(verbose)
              cout << "In CompleteCommunication (bool uint32): Message argument "
                   << "type is not uint32."<< endl;
            return false;
        }
        else
        {
            dbus_message_iter_get_basic(&messageIterator, successUint32Value);
            if(verbose)
              cout << "In CompleteCommunication (bool uint32): reply = "
                   << *successUint32Value << endl;
            if(*successUint32Value > 0)
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool uint32): "
                       << "returning true" << endl;
                return true;
            }
            else
            {
                dbus_message_unref(message);
                if(verbose)
                  cout << "In CompleteCommunication (bool uint32): "
                       << "returning false" << endl;
                return false;
            }
        }
    }
    //If we dont wait for the answer
    return true;
}

bool VampirConnecter::CompleteFileOpening(std::string const &fileName)
{
    trace_file_session session;

    //for the future, you can open more than one file with qtvampir
    if(CompleteCommunication(true, (dbus_uint32_t*)&session.sessionId))
    {
        openedFiles[fileName] = session;

        //check for the 'traceFileLoaded' signal from vampir
        //before opening the timeline
        dbus_uint32_t statusId = 1;
        DBusMessage *replyMessage = 0;
        DBusMessageIter replyArgs;
        while(true)
        {
            dbus_connection_read_write(connection, 50);
            replyMessage = dbus_connection_pop_message(connection);

            //check signal for successfully loading
            if(replyMessage != 0 &&
               dbus_message_is_signal(replyMessage, interfaceName.c_str(),
                                      "traceFileLoaded"))
            {
                if(verbose)
                {
                    cout << "Vampir instance at bus name " << busName
                         << " has loaded file " << fileName
                         << " in session id " << session.sessionId << endl;
                }
                //FIXME:
                sleep(1);
                return true;
            }
            //check signal for loading errors
            else if(replyMessage != 0 &&
                    dbus_message_is_signal(replyMessage, interfaceName.c_str(),
                                           "status"))
            {
                //check status
                //a status id which is greater than 0 is not ok and was error is occurred
                if(dbus_message_iter_init(replyMessage,&replyArgs)){
                    if(DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type(&replyArgs))
                    {
                        dbus_message_iter_get_basic(&replyArgs,&statusId);
                        if(statusId > 0)
                        {
                            if(verbose)
                            {
                                cout << "Vampir instance at bus name " << busName
                                     << " threw error status id " << statusId << endl;
                            }
                            return false;
                        }
                    }
                }
            }

            //do nothing and wait for right signal
            if(replyMessage != 0)
            {
                dbus_message_unref(replyMessage);
            }
            replyMessage = 0;
        }
    }

    return false;
}

void VampirConnecter::AddMessage(string const &messageString)
{
    char *buffer = new char[messageString.length() + 1];
    strcpy(buffer, messageString.c_str());
    if(!dbus_message_iter_append_basic(&messageIterator,
                                       DBUS_TYPE_STRING, &buffer))
    {
        throw VampirConnecterException(
              "Out of memory while trying to append a string");
    }
    delete[] buffer;
}

void VampirConnecter::AddMessage(dbus_uint32_t messageNumber)
{
    if(!dbus_message_iter_append_basic(&messageIterator,
                                       DBUS_TYPE_UINT32, &messageNumber))
    {
        throw VampirConnecterException(
              "Out of memory while trying to append a dbus_uint32_t");
    }
}

void VampirConnecter::AddMessage(double messageNumber)
{
    if(!dbus_message_iter_append_basic(&messageIterator,
                                       DBUS_TYPE_DOUBLE, &messageNumber))
    {
        throw VampirConnecterException(
              "Out of memory while trying to append a double");
    }
}

bool VampirConnecter::OpenLocalTraceFile(std::string const &fileName)
{
    if(verbose)
    {
        cout << "Vampir instance at bus name " << busName
             << " tries to open local file " << fileName << endl;
    }
    InitiateCommunication("openLocalTrace");
    AddMessage(fileName);

    return CompleteFileOpening(fileName);
}

bool VampirConnecter::OpenRemoteTraceFile(string const &fileName, string const &serverName, dbus_uint32_t portNumber)
{
    if(verbose)
    {
        cout << "Vampir instance at bus name " << busName
             << " tries to open remote file " << fileName << " on " << serverName << ":" << portNumber << endl;
    }
    InitiateCommunication("openRemoteTrace");
    AddMessage(fileName);
    AddMessage(serverName);
    AddMessage(portNumber);

    return CompleteFileOpening(fileName);
}

string VampirConnecter::DisplayTypeToString(DisplayType type)
{
    switch(type)
    {
        case Timeline:
            return "Master Timeline";
        case CounterTimeline:
            return "Counter Data Timeline";
        case SummaryChart:
            return "Summary Chart";
        case ProcessProfile:
            return "Process Summary";
    }
    throw VampirConnecterException("Unsupported display type");
}

bool VampirConnecter::OpenDisplay(DisplayType type)
{
    dbus_uint32_t displayId = 0;
    //we have only one file, so we need not search the right map key
    map<std::string,trace_file_session>::iterator it;
    it = openedFiles.begin();

    //at first, we will check whether a display type was already opened
    if(isDisplayOpen(type, &displayId))
    {
        it->second.displayIds[displayId] = type;

        if(verbose)
        {
            cout << "Vampir instance at bus name " << busName
                 << " has already a opened display " << DisplayTypeToString(type)
                 << "with id " << displayId
                 << " for session " << it->second.sessionId << endl;
        }
        return true;
    }
    //if the type not open, we can try to open it
    else
    {
        if(verbose)
        {
            cout << "Vampir instance at bus name " << busName
                 << " is opening display " << DisplayTypeToString(type)
                 << " for session " << it->second.sessionId << endl;
        }
        InitiateCommunication("openDisplay");
        AddMessage(DisplayTypeToString(type));
        AddMessage(it->second.sessionId);

        if(CompleteCommunication(true, (dbus_uint32_t*)&displayId))
        {
            it->second.displayIds[displayId] = type;

            //give time to initialize the opened vampir window and wait
            //for 'displayLoaded'
            //check the loaded displayId with the set display id
            dbus_uint32_t loadedDisplayId = 0, statusId = 1;
            DBusMessage *replyMessage = 0;
            DBusMessageIter replyArgs;
            while(true)
            {
                dbus_connection_read_write(connection, 50);
                replyMessage = dbus_connection_pop_message(connection);

                if(replyMessage != 0 &&
                   dbus_message_is_signal(replyMessage, interfaceName.c_str(),
                                          "displayLoaded"))
                {
                    //check loaded displayId
                    if(dbus_message_iter_init(replyMessage,&replyArgs))
                    {
                        if(DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type(&replyArgs))
                        {
                            dbus_message_iter_get_basic(&replyArgs,&loadedDisplayId);
                            if(loadedDisplayId == displayId)
                            {
                                if(verbose)
                                {
                                    cout << "Vampir instance at bus name " << busName
                                         << " loaded display " << DisplayTypeToString(type)
                                         << " with display id " << displayId
                                         << " for session " << it->second.sessionId << endl;
                                }

                                //FIXME:
                                sleep(1);
                                return true;
                            }
                        }
                    }
                }
                //check signal for loading errors
                else if(replyMessage != 0 &&
                        dbus_message_is_signal(replyMessage, interfaceName.c_str(),
                                               "status"))
                {
                    //check status
                    //a status id which is greater than 0 is not ok and was error is occurred
                    if(dbus_message_iter_init(replyMessage,&replyArgs)){
                        if(DBUS_TYPE_UINT32 == dbus_message_iter_get_arg_type(&replyArgs))
                        {
                            dbus_message_iter_get_basic(&replyArgs,&statusId);
                            if(statusId > 0)
                            {
                                if(verbose)
                                {
                                    cout << "Vampir instance at bus name " << busName
                                         << " threw error status id " << statusId << endl;
                                }
                                return false;
                            }
                        }
                    }
                }

                //do nothing and wait for right signal
                if(replyMessage != 0)
                {
                    dbus_message_unref(replyMessage);
                }
                replyMessage = 0;
            }
        }
    }

    return false;
}

bool VampirConnecter::ZoomDisplay(DisplayType type,
                                  double startTick, double endTick)
{
    char buffer[128];
    (void) type;

    //we can only have one opened file at the monent, so we take the first from map
    map<std::string const,trace_file_session>::iterator it_sessions;
    it_sessions = openedFiles.begin();
    trace_file_session session = it_sessions->second;

    //we open only one display at the moment, so we can take the first display from display map
    map<dbus_uint32_t,DisplayType>::iterator it_displays;
    it_displays = session.displayIds.begin();

    double tickMultiplier = -1;
    switch(fileType)
    {
        case ELG:
            tickMultiplier = 1e9;
            break;
        case OTF:
            tickMultiplier = 1e10;
            break;
        default:
            tickMultiplier = 1e10;
            break;
    }

    InitiateCommunication("setZoom");
    sprintf(buffer,"%f",tickMultiplier * startTick);
    AddMessage(buffer);
    sprintf(buffer,"%f",tickMultiplier * endTick);
    AddMessage(buffer);
    AddMessage("tick");
    AddMessage(it_displays->first);
    AddMessage(session.sessionId);

    if(verbose)
    {
        cout << "Vampir instance at bus name " << busName
                << " start: " << tickMultiplier * startTick
                << " end: " << tickMultiplier * endTick
                << "numberFormat: tick"
                << " zoom display: " << it_displays->first
                << " session id: " << session.sessionId << endl;
    }

    return CompleteCommunication(true, true);
}

bool VampirConnecter::isDisplayOpen(DisplayType type, dbus_uint32_t *displayId)
{
    dbus_uint32_t_list *openDisplayIdList = NULL;
    char *buffer = NULL;

    //we can only have one opened file at the monent, so we take the first from map
    map<std::string const,trace_file_session>::iterator it_sessions;
    it_sessions = openedFiles.begin();
    trace_file_session session = it_sessions->second;

    InitiateCommunication("listDisplays");
    AddMessage(session.sessionId);

    if(verbose)
    {
        cout << "In isDisplayOpen: We will check all opened display, now"  << endl;
    }

    //get all opened display ids and check the display name
    if(CompleteCommunication(true,&openDisplayIdList))
    {
        //check display id step by step
        while(openDisplayIdList){
            //now we must ask vampir after the name for current display id
            //after that, we must check the display name with the name, which we want to open
            InitiateCommunication("displayIdToText");
            AddMessage(openDisplayIdList->data);
            AddMessage(session.sessionId);
            CompleteCommunication(true,&buffer);

            if(verbose)
            {
                cout << "In isDisplayOpen: Display id " << openDisplayIdList->data << " is a " <<  buffer << " type"  << endl;
            }

            if(strcmp(DisplayTypeToString(type).c_str(), buffer) == 0)
            {
                if(verbose)
                {
                    cout << "In isDisplayOpen: Display is open " << (char*)buffer << " == " << DisplayTypeToString(type) << endl;
                    cout << "In isDisplayOpen: We have found a already opened display!!!"  << endl;
                }

                *displayId = openDisplayIdList->data;
                free(buffer);
                destroyUint32_t_list(openDisplayIdList);
                return true;
            }
            else
            {
                if(verbose)
                {
                    cout << "In isDisplayOpen: " << (char*)buffer << " != " << DisplayTypeToString(type) << endl;
                }
                free(buffer);
            }

            openDisplayIdList = openDisplayIdList->next;
        }
    }

    if(verbose)
    {
        cout << "In isDisplayOpen: Display is NOT open."  << endl;
    }

    destroyUint32_t_list(openDisplayIdList);
    return false;
}

void VampirConnecter::destroyUint32_t_list(dbus_uint32_t_list *list)
{
    while(list)
    {
        dbus_uint32_t_list *next = list->next;
        delete list;
        list = next;
    }
}

void VampirConnecter::Exit()
{
    if(verbose)
    {
        cout << "Calling exit on vampir client at bus name " << busName << endl;
    }
    InitiateCommunication("quit");
    CompleteCommunicationGeneric(true);
}

bool VampirConnecter::IsActive() const
{
    return isActive;
}

VampirConnecter::~VampirConnecter()
{
    if(ExistsVampirWithBusName(busName))
    {
        Exit();
    }
}

bool VampirConnecter::ExistsVampirWithBusName(string const &name)
{
    DBusError error;
    dbus_error_init(&error);
    //connect to the bus and check for errors
    DBusConnection *connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    //Prevent the application from being closed when the connection closes
    dbus_connection_set_exit_on_disconnect(connection,  false);
    CheckError(error);
    if(connection == 0)
    {
        throw VampirConnecterException("Connection failed");
    }
    bool res = dbus_bus_name_has_owner(connection, name.c_str(), &error);
    dbus_error_free(&error);
    return res;
}

string VampirConnecter::GetVampirBusName(int index)
{
    assert(index < GetMaxVampirNumber());
    switch(index)
    {
        case 0:
            return "com.gwt.vampir";
        case 1:
            return "com.gwt.vampir.slave";
        default:
            return "";
    }
}

int VampirConnecter::GetMaxVampirNumber()
{
    return 2;
}

