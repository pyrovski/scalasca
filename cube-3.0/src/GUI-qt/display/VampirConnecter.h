/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef VAMPIR_CONNECTER_H
#define VAMPIR_CONNECTER_H

#include <dbus/dbus.h>

#include <exception>
#include <string>
#include <map>

#include "VisualizationConnecter.h"

struct dbus_uint32_t_list{
    dbus_uint32_t data;
    dbus_uint32_t_list *next;
};

class VampirConnecterException : public std::exception
{
    public:
    VampirConnecterException(std::string const &error)
        : message("Connection error: " + error)
    {}
    ~VampirConnecterException() throw()
    {}
    virtual const char* what() const throw()
    {
        return message.c_str();
    }
    private:
        std::string message;
};

class VampirConnecter : public VisualizationConnecter
{
    public:
        VampirConnecter(std::string const &theBusName,
                        std::string const &serverName, dbus_uint32_t portNumber,
                        std::string const &fileName, bool verbose);
        virtual std::string InitiateAndOpenTrace();
        virtual std::string ZoomIntervall(double start, double end, int zoomStep);
        virtual bool IsActive() const;
        virtual ~VampirConnecter();
        static bool ExistsVampirWithBusName(std::string const &name);
        static std::string GetVampirBusName(int index);
        static int GetMaxVampirNumber();
    private:
        DBusConnection *connection;
        DBusPendingCall *pending;
        DBusMessageIter messageIterator;
        DBusMessage *message;
        std::string busName, objectName, interfaceName;
        std::string server;
        dbus_uint32_t port;
        std::string file;
        bool isActive;
        bool verbose;
	
        //these methods were public before the change of interface
        //to VisualizationConnecter (maybe useful)
        bool OpenLocalTraceFile(std::string const &fileName);
        bool OpenRemoteTraceFile(std::string const &fileName, std::string const &serverName, dbus_uint32_t portNumber);
        enum DisplayType {Timeline, CounterTimeline, SummaryChart, ProcessProfile};
        //these structure hold a opened file with the given sessionId and all opened displayIds of vampir
        struct trace_file_session
        {
            dbus_uint32_t sessionId;
            std::map<dbus_uint32_t, DisplayType> displayIds;

        };
        //map consists all opened files. at the moment, there is only one opened file.
        //you can open more than one file with qvampir. if you want open more, you can append new trace_file_session to this map.
        std::map<std::string const,trace_file_session> openedFiles;

        bool OpenDisplay(DisplayType type);
        bool ZoomDisplay(DisplayType type, double startTick, double endTick);
        bool isDisplayOpen(DisplayType type, dbus_uint32_t *displayId);
        void destroyUint32_t_list(dbus_uint32_t_list *list);
        void Exit();
	
        //the following methods always were private (just for implementation)
        static void CheckError(DBusError &error);
        void InitiateCommunication(std::string const &methodName);
        void AddMessage(std::string const &messageString);
        void AddMessage(dbus_uint32_t messageNumber);
        void AddMessage(double messageNumber);

        //Attention: When using string literals the bool overload is
        //prefered to the string overload
        bool CompleteCommunication(bool block,
                                   std::string const &successMessageBeginning);
        bool CompleteCommunication(bool block, bool successFlag);
        bool CompleteCommunication(bool block, dbus_uint32_t *successUint32Value);
        bool CompleteCommunication(bool block, dbus_uint32_t_list **successDBusUint32List);
        bool CompleteCommunication(bool block, char **successCharStringValue);
        bool CompleteFileOpening(std::string const &fileName);
        void CompleteCommunicationGeneric(bool block);
        std::string DisplayTypeToString(DisplayType type);
        static bool busNameRegistered;
        enum FileType{ELG, OTF, undefined} fileType;
};

#endif

