/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_EventRecord_H
#define EXP_EventRecord_H

#include <elg_rw.h>
#include <iostream>

using namespace std;

/*
 *----------------------------------------------------------------------------
 *
 * class EventRecord
 *
 * Pattern Analysis process
 *
 *---------------------------------------------------------------------------- 
 */

class EventRecord{
public:
EventRecord(double time):_time(time){
}

virtual ~EventRecord() {}

bool operator>(const EventRecord& rhs){
  return (_time > rhs.GetTime());
}

bool operator<=(const EventRecord& rhs){
  return (_time <= rhs.GetTime());
}

double GetTime() const{ return _time;}
virtual void Write(ElgOut* p_trace, long location) const = 0;
private:
double _time;
};

/********* class EnterRecord *********/
class EnterRecord : public EventRecord{
public:
EnterRecord(double time, long region): EventRecord(time), _region(region){}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_ENTER(p_trace, location, GetTime(), _region, 0, NULL);
 // cout << "ENTER Time: " << GetTime() << endl; // " Region: "<< _region << endl;
}
 
private:
long _region;
};

/********* class ExitRecord *********/
class ExitRecord : public EventRecord{
public:
ExitRecord(double time): EventRecord(time){
}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_EXIT(p_trace, location, GetTime(), 0, NULL);
   //cout << "EXIT  Time: " << GetTime() << endl;
 
}
};

/********* class CollExitRecord (SHMEM) ********/
class CollExitRecord : public EventRecord{
public:
CollExitRecord(double time, long comm, long root=ELG_NO_ID, long sent=0, long recvd=0):EventRecord(time), _communicator(comm), _root(root), _sent(sent), _recvd(recvd){}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_COLLEXIT(p_trace, location, GetTime(), 0, 0, _root , _communicator, _sent, _recvd);
 // cout << "CEXIT Time: " << GetTime() << endl; // " Comm: " <<_communicator << endl;
}

private:
long _communicator;
long _root;
long _sent;
long _recvd;
};

/********* class MPICollExitRecord ********/
class MPICollExitRecord : public EventRecord{
public:
MPICollExitRecord(double time, long comm, long root=ELG_NO_ID, long sent=0, long recvd=0):EventRecord(time), _communicator(comm), _root(root), _sent(sent), _recvd(recvd){}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_MPI_COLLEXIT(p_trace, location, GetTime(), 0, 0, _root , _communicator, _sent, _recvd);
 // cout << "CEXIT Time: " << GetTime() << endl; // " Comm: " <<_communicator << endl;
}
private:
long _communicator;
long _root;
long _sent;
long _recvd;
};


/******** class MPIRMACollExitRecord ********/
class MPIRMACollExitRecord: public EventRecord{
public:
MPIRMACollExitRecord(double time, long wid):EventRecord(time), _wid(wid){}
MPIRMACollExitRecord(double time, long wid, long root=ELG_NO_ID, long sent=0, long recvd=0):EventRecord(time), _wid(wid){}

virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_MPI_WINCOLLEXIT(p_trace, location, GetTime(), 0, 0, _wid);
}
private:
long _wid;
};

/******** class OMPCollExitRecord ********/
class OMPCollExitRecord: public EventRecord{
public:
OMPCollExitRecord(double time):EventRecord(time){}
OMPCollExitRecord(double time, long comm=0, long root=ELG_NO_ID, long sent=0, long recvd=0):EventRecord(time){}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_OMP_COLLEXIT(p_trace, location, GetTime(), 0, 0);
}
private:
};

/********* class SendRecord *********/
class SendRecord : public EventRecord{
public:
SendRecord(double time, long dest, long comm, long tag, long sent ):EventRecord(time), _tag(tag), _dest(dest), _communicator(comm),
                                                                    _sent(sent) {}
virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_MPI_SEND(p_trace, location, GetTime(), _dest, _communicator, _tag, _sent);
}
private:
long _tag;
long _dest;
long _communicator;
long _sent;
};

/********* class RecvRecord *********/
class RecvRecord : public EventRecord{
public:
RecvRecord(double time, long src, long comm, long tag):EventRecord(time), _tag(tag), _src(src), _communicator(comm) {}

virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_MPI_RECV(p_trace, location, GetTime(), _src, _communicator, _tag);
}
private:
long _tag;
long _src;
long _communicator;
};

// ******* class RLockRecord ************/
class RLockRecord : public EventRecord{
public:
RLockRecord(double time, long releaseid):EventRecord(time), _releaseid(releaseid){}

virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_OMP_RLOCK(p_trace, location, GetTime(), _releaseid);
}
private:
long _releaseid;
};

// ******* class ALockRecord ************/
class ALockRecord : public EventRecord{
public:
ALockRecord(double time, long lockid):EventRecord(time), _lockid(lockid){}

virtual void Write(ElgOut* p_trace, long location) const{
  ElgOut_write_OMP_ALOCK(p_trace, location, GetTime(), _lockid);
}
private:
long _lockid;
};

#endif
