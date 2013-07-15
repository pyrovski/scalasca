#ifndef PARSECONTEXT_H
#define PARSECONTEXT_H

#include <sstream>
#include <vector>
#include "Cube.h"

class ParseContext{

 public:

  typedef enum {STRINGCONTENT, INTCONTENT, BOOLCONTENT} ContentType;

  bool dynamicMetricLoading;

  std::vector<long> dimVec;
  std::vector<bool> periodicVec;
  std::vector<std::string> dimNameVec;

  std::vector<cube::Region*> regionVec;
  std::vector<cube::Metric*> metricVec;
  std::vector<cube::Cnode*> cnodeVec;
  std::vector<cube::Thread*> threadVec;
  std::vector<cube::Process*> procVec;
  std::vector<cube::Node*> nodeVec;
  std::vector<cube::Machine*> machVec;
  std::vector<cube::Sysres*> systemVec;
  std::vector<std::string> moduleVec;
  std::vector<long> csiteVec;

  cube::Machine* currentMachine;
  cube::Node* currentNode;
  cube::Process* currentProc;
  cube::Thread* currentThread;
  cube::Cnode* currentCnode;
  cube::Cartesian* currentCart;
  cube::Metric* currentMetric;

  int cubeVersion;
  bool ignoreMetric; //in severity it stores if the current metric or one of its parents have
                     //the type VOID and thus can be ignored
  bool realContent;
  bool longContent;
  ContentType contentType;

  std::vector<double> realValues;
  std::vector<long> longValues;
  std::ostringstream str;

  std::string version;
  std::string encoding;
  std::string key;
  std::string value;
  std::string mod;
  std::string murl;
  std::string disp_name;
  std::string uniq_name;
  std::string dtype;
  std::string uom;
  std::string val;
  std::string url;
  std::string descr;
  std::string name;
  std::string dimName;
  std::string cartName;

  int id, calleeid, thrdid, metricid, cnodeid, rank;
  int procid, nodeid, machid;
  int moduleid, calleenr, csiteid, locid;
  long beginln, endln, line, ndims;
  long longAttribute;

  bool idSeen, calleeidSeen, lineSeen, modSeen;
  bool beginSeen, endSeen, periodicSeen, ndimsSeen;
  bool sizeSeen, cnodeidSeen, metricidSeen, machidSeen;
  bool nodeidSeen, procidSeen, thrdidSeen, valueSeen;
  bool keySeen, encodingSeen, versionSeen;

  bool dispnameSeen, uniqnameSeen, dtypeSeen;
  bool uomSeen, valSeen, urlSeen, descrSeen;
  bool murlSeen, nameSeen, rankSeen;

  bool cartNameSeen;
  bool dimNameSeen;
  ParseContext(){

    dynamicMetricLoading = false;

    realContent = false;
    longContent = false;
    contentType = STRINGCONTENT;

    currentMachine = NULL;
    currentNode = NULL;
    currentProc = NULL;
    currentThread = NULL;
    currentCnode = NULL;
    currentCart = NULL;
    currentMetric = NULL;
    cubeVersion = 0;

    ignoreMetric = false;

    idSeen = false;
    calleeidSeen = false;
    lineSeen = false;
    modSeen = false;
    beginSeen = false;
    endSeen = false;
    periodicSeen = false;
    ndimsSeen = false;
    sizeSeen = false;
    cnodeidSeen = false;
    metricidSeen = false;
    machidSeen = false;
    nodeidSeen = false;
    procidSeen = false;
    thrdidSeen = false;
    valueSeen = false;
    keySeen = false;
    encodingSeen = false;
    versionSeen = false;

    dispnameSeen = false;
    uniqnameSeen = false;
    dtypeSeen = false;
    uomSeen = false;
    valSeen = false;
    urlSeen = false;
    descrSeen = false;
    murlSeen = false;
    nameSeen = false;
    rankSeen = false;

    cartNameSeen = false;
    dimNameSeen = false;
  }

  ~ParseContext(){
  }

  bool checkThreadIds(){
    for (unsigned i=0; i<threadVec.size(); i++){
      if (threadVec[i]==NULL) return false;
    }
    return true;
  }

};


#endif // PARSECONTEXT_H
