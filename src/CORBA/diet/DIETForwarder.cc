/**
* @file CorbaForwarder.cc
*
* @brief   Corba forwarder implementation
*
* @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/

#include "CorbaForwarder.hh"

#include "Forwarder.hh"
#include "ORBMgr.hh"
#include "common_types.hh"

#include "AgentImpl.hh"
#include "CallbackImpl.hh"
#include "LocalAgentImpl.hh"
#include "MasterAgentImpl.hh"
#include "SeDImpl.hh"

#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif

#include "DagdaImpl.hh"

#ifdef HAVE_WORKFLOW
#include "CltWfMgr.hh"
#include "MaDag_impl.hh"
#include "WfLogServiceImpl.hh"
#endif

#include "debug.hh"

#include <stdexcept>
#include <string>
#include <cstring>
#include <list>
#include <unistd.h>  // For gethostname()

#ifdef MAXHOSTNAMELEN
#define MAX_HOSTNAME_LENGTH MAXHOSTNAMELEN
#else
#define MAX_HOSTNAME_LENGTH  255
#endif



/* Corba object factory methods. */
Agent_ptr
CorbaForwarder::getAgent(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(AGENTCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return Agent::_duplicate(Agent::_narrow(object));
  }

  AgentFwdrImpl * agent = new AgentFwdrImpl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(agent);

  cachesMutex.lock();
  servants[nm] = agent;
  objectCache[nm] = CORBA::Object::_duplicate(agent->_this());
  cachesMutex.unlock();

  return Agent::_duplicate(agent->_this());
}

Callback_ptr
CorbaForwarder::getCallback(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(CLIENTCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return Callback::_duplicate(Callback::_narrow(object));
  }

  CallbackFwdrImpl * callback = new CallbackFwdrImpl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(callback);

  cachesMutex.lock();
  servants[nm] = callback;
  objectCache[nm] = CORBA::Object::_duplicate(callback->_this());
  cachesMutex.unlock();

  return Callback::_duplicate(callback->_this());
}

LocalAgent_ptr
CorbaForwarder::getLocalAgent(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;
  if (nm.find('/') == std::string::npos) {
    nm = std::string(AGENTCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return LocalAgent::_duplicate(LocalAgent::_narrow(object));
  }
  LocalAgentFwdrImpl * agent = new LocalAgentFwdrImpl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(agent);

  cachesMutex.lock();
  servants[nm] = agent;
  objectCache[nm] = CORBA::Object::_duplicate(agent->_this());
  cachesMutex.unlock();

  return LocalAgent::_duplicate(agent->_this());
}

MasterAgent_ptr
CorbaForwarder::getMasterAgent(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(AGENTCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return MasterAgent::_duplicate(MasterAgent::_narrow(object));
  }
  MasterAgentFwdrImpl *agent =
    new MasterAgentFwdrImpl(this->_this(), nm.c_str());
  ORBMgr::getMgr()->activate(agent);

  cachesMutex.lock();
  servants[nm] = agent;
  objectCache[nm] = CORBA::Object::_duplicate(agent->_this());
  cachesMutex.unlock();

  return MasterAgent::_duplicate(agent->_this());
}

SeD_ptr
CorbaForwarder::getSeD(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') ==
      std::string::npos) {
    nm = std::string(SEDCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return SeD::_duplicate(SeD::_narrow(object));
    }
  }

  SeDFwdrImpl * sed = new SeDFwdrImpl(this->_this(), nm.c_str());
  ORBMgr::getMgr()->activate(sed);

  cachesMutex.lock();
  objectCache[nm] = CORBA::Object::_duplicate(sed->_this());
  servants[nm] = sed;
  cachesMutex.unlock();

  return SeD::_duplicate(sed->_this());
}

//Dagda_ptr
//CorbaForwarder::getDagda(const char* name) {
//  std::string nm(name);
//  ::CORBA::Object_ptr object;
//
//  if (nm.find('/') == std::string::npos) {
//    nm = std::string(DAGDACTXT)+"/"+nm;
//  }
//  object = getObjectCache(nm);
//  if (!CORBA::is_nil(object)) {
//    if (object->_non_existent()) {
//      removeObjectFromCache(name);
//    } else
//      return Dagda::_duplicate(Dagda::_narrow(object));
//  }
//  DagdaFwdrImpl * dagda = new DagdaFwdrImpl(this->_this(), nm.c_str());
//  ORBMgr::getMgr()->activate(dagda);
//
//  cachesMutex.lock();
//  servants[nm] = dagda;
//  objectCache[nm] = CORBA::Object::_duplicate(dagda->_this());
//  cachesMutex.unlock();
//
//  return Dagda::_duplicate(dagda->_this());
//}

#ifdef HAVE_WORKFLOW
CltMan_ptr
CorbaForwarder::getCltMan(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(WFMGRCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return CltManFwdr::_duplicate(CltManFwdr::_narrow(object));
    }
  }

  CltWfMgrFwdr * mgr = new CltWfMgrFwdr(this->_this(), nm.c_str());
  ORBMgr::getMgr()->activate(mgr);

  cachesMutex.lock();
  servants[nm] = mgr;
  objectCache[nm] = CORBA::Object::_duplicate(mgr->_this());
  cachesMutex.unlock();

  return CltManFwdr::_duplicate(mgr->_this());
}

MaDag_ptr
CorbaForwarder::getMaDag(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(AGENTCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return MaDag::_duplicate(MaDag::_narrow(object));
    }
  }

  MaDagFwdrImpl * madag = new MaDagFwdrImpl(this->_this(), nm.c_str());
  ORBMgr::getMgr()->activate(madag);

  cachesMutex.lock();
  servants[nm] = madag;
  objectCache[nm] = CORBA::Object::_duplicate(madag->_this());
  cachesMutex.unlock();

  return MaDag::_duplicate(madag->_this());
}

WfLogService_ptr
CorbaForwarder::getWfLogService(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(WFLOGCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return WfLogService::_duplicate(WfLogService::_narrow(object));
    }
  }

  WfLogServiceFwdrImpl *wfl =
    new WfLogServiceFwdrImpl(this->_this(), nm.c_str());
  ORBMgr::getMgr()->activate(wfl);

  cachesMutex.lock();
  objectCache[nm] = CORBA::Object::_duplicate(wfl->_this());
  servants[nm] = wfl;
  cachesMutex.unlock();

  return WfLogService::_duplicate(wfl->_this());
}

#endif

::CORBA::Long
CorbaForwarder::disconnect(const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->disconnect(objString.c_str());
  }

  name = objString;

  if (name.find('/') == std::string::npos)
    throw BadNameException(name.c_str(), __FUNCTION__, getName());

  ctxt = getCtxt(objString);
  name = getName(objString);

  if (ctxt == std::string(AGENTCTXT)) {
    LocalAgent_var agent =
      ORBMgr::getMgr()->resolve<LocalAgent, LocalAgent_var>(ctxt, name,
                                                            this->name);
    return agent->disconnect();
  }
  if (ctxt == std::string(SEDCTXT)) {
    SeD_var sed = ORBMgr::getMgr()->resolve<SeD, SeD_var>(ctxt, name,
                                                          this->name);
    return sed->disconnect();
  }
  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
}

//::CORBA::Long
//CorbaForwarder::removeElement(::CORBA::Boolean recursive, const char* objName) {
//  std::string objString(objName);
//  std::string name;
//  std::string ctxt;
//
//  if (!remoteCall(objString)) {
//    return getPeer()->removeElement(recursive, objString.c_str());
//  }
//
//  name = objString;
//
//  if (name.find('/') == std::string::npos) {
//    throw BadNameException(name.c_str(), __FUNCTION__, getName());
//  }
//
//  ctxt = getCtxt(objString);
//  name = getName(objString);
//
//  if (ctxt == std::string(AGENTCTXT)) {
//    Agent_var agent = ORBMgr::getMgr()->resolve<Agent, Agent_var>(ctxt, name,
//                                                                  this->name);
//    return agent->removeElement(recursive);
//  }
//  if (ctxt == std::string(SEDCTXT)) {
//    SeD_var sed =
//      ORBMgr::getMgr()->resolve<SeD, SeD_var>(ctxt, name, this->name);
//    return sed->removeElement();
//  }
//  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
//}

