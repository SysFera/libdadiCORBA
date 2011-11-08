/**
* @file DIETForwarder.cc
*
* @brief   DIET forwarder implementation
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

#include "impl/AgentImpl.hh"
#include "impl/CallbackImpl.hh"
#include "impl/LocalAgentImpl.hh"
#include "impl/MasterAgentImpl.hh"
#include "impl/SeDImpl.hh"

#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif

#include "impl/DagdaImpl.hh"

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


/**
 * WARNING: tricky function
 * The function return wheter the object designed by objName is local or remote
 * If the object is local, the prefix 'remote' will be added
 * so that when passed to a remote omniname, it is known as a remote object
 * When remote object, removing the remote prefix to be able to use it locally
 */
bool
CorbaForwarder::remoteCall(std::string& objName) {
  // Fixme when cleaning the code, looks for a string
  // that is prefixed by remote:
  if (objName.find("remote:") != 0) {
    /* Local network call: need to be forwarded to
     * the peer forwarder. Add the prefix.
     */
    objName = "remote:" + objName;
    return false;
  }
  /* Remote network call. Remove the prefix. */
  objName = objName.substr(strlen("remote:"));
  return true;
}


::CORBA::Object_ptr
CorbaForwarder::getObjectCache(const std::string& name) {
  std::map<std::string, ::CORBA::Object_ptr>::iterator it;

  cachesMutex.lock();
  if ((it = objectCache.find(name))!=objectCache.end()) {
    cachesMutex.unlock();
    return CORBA::Object::_duplicate(it->second);
  }
  cachesMutex.unlock();
  return ::CORBA::Object::_nil();
}

CorbaForwarder::CorbaForwarder(const std::string& name) {
  char buffer[MAX_HOSTNAME_LENGTH+1];
  gethostname(buffer, MAX_HOSTNAME_LENGTH);

  this->name = name;
  this->host = buffer;

  // Wait for the peer init. The unlock will be done on setPeer().
  peerMutex.lock();
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


/* Common methods implementations. */
::CORBA::Long
CorbaForwarder::ping(const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->ping(objString.c_str());
  }

  name = getName(objString);
  ctxt = getCtxt(objString);

  if (ctxt == std::string(AGENTCTXT)) {
    Agent_var agent =
      ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
    return agent->ping();
  }
  if (ctxt == std::string(CLIENTCTXT)) {
    Callback_var cb =
      ORBMgr::getMgr()->resolve<Callback, Callback_var>(CLIENTCTXT, name,
                                                        this->name);
    return cb->ping();
  }
#ifdef HAVE_WORKFLOW
  if (ctxt == std::string(WFMGRCTXT)) {
    CltMan_var clt =
      ORBMgr::getMgr()->resolve<CltMan, CltMan_var>(WFMGRCTXT, name,
                                                    this->name);
    return clt->ping();
  }
  if (ctxt == std::string(MADAGCTXT)) {
    MaDag_var madag =
      ORBMgr::getMgr()->resolve<MaDag, MaDag_var>(MADAGCTXT, name, this->name);
    return madag->ping();
  }
#endif
  if (ctxt== std::string(SEDCTXT)) {
    SeD_var sed = ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name,
                                                          this->name);
    return sed->ping();
  }
  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
}

void
CorbaForwarder::getRequest(const ::corba_request_t& req, const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->getRequest(req, objString.c_str());
  }

  name = getName(objString);
  ctxt = getCtxt(objString);

  if (ctxt == std::string(AGENTCTXT)) {
    LocalAgent_var agent =
      ORBMgr::getMgr()->resolve<LocalAgent, LocalAgent_var>(ctxt, name,
                                                            this->name);
    return agent->getRequest(req);
  }
  if (ctxt == std::string(SEDCTXT)) {
    SeD_var sed =
      ORBMgr::getMgr()->resolve<SeD, SeD_var>(ctxt, name, this->name);
    return sed->getRequest(req);
  }
}

char*
CorbaForwarder::getHostname(const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->getHostname(objString.c_str());
  }

  name = objString;

  if (name.find('/') == std::string::npos)
    throw BadNameException(name.c_str(), __FUNCTION__, getName());

  ctxt = getCtxt(objString);
  name = getName(objString);

  if (ctxt == std::string(AGENTCTXT)) {
    Agent_var agent =
      ORBMgr::getMgr()->resolve<Agent, Agent_var>(ctxt, name, this->name);
    return agent->getHostname();
  }

  if (ctxt == std::string(DAGDACTXT)) {
    Dagda_var dagda = ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(ctxt, name,
                                                                  this->name);
    return dagda->getHostname();
  }

  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
}

::CORBA::Long
CorbaForwarder::bindParent(const char* parentName, const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->bindParent(parentName, objString.c_str());
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
    return agent->bindParent(parentName);
  }
  if (ctxt == std::string(SEDCTXT)) {
    SeD_var sed =
      ORBMgr::getMgr()->resolve<SeD, SeD_var>(ctxt, name, this->name);
    return sed->bindParent(parentName);
  }
  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
}

//::CORBA::Long
//CorbaForwarder::disconnect(const char* objName) {
//  std::string objString(objName);
//  std::string name;
//  std::string ctxt;
//
//  if (!remoteCall(objString)) {
//    return getPeer()->disconnect(objString.c_str());
//  }
//
//  name = objString;
//
//  if (name.find('/') == std::string::npos)
//    throw BadNameException(name.c_str(), __FUNCTION__, getName());
//
//  ctxt = getCtxt(objString);
//  name = getName(objString);
//
//  if (ctxt == std::string(AGENTCTXT)) {
//    LocalAgent_var agent =
//      ORBMgr::getMgr()->resolve<LocalAgent, LocalAgent_var>(ctxt, name,
//                                                            this->name);
//    return agent->disconnect();
//  }
//  if (ctxt == std::string(SEDCTXT)) {
//    SeD_var sed = ORBMgr::getMgr()->resolve<SeD, SeD_var>(ctxt, name,
//                                                          this->name);
//    return sed->disconnect();
//  }
//  throw BadNameException(objString.c_str(), __FUNCTION__, getName());
//}

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


void
CorbaForwarder::bind(const char* objName, const char* ior) {
  /* To avoid crashes when the peer forwarder is not ready: */
  /* If the peer was not initialized, the following call is blocking. */
  peerMutex.lock();
  peerMutex.unlock();

  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    TRACE_TEXT(TRACE_MAIN_STEPS,
               "Forward bind to peer (" << objName << ")\n");
    return getPeer()->bind(objString.c_str(), ior);
  }
  ctxt = getCtxt(objString);
  name = getName(objString);
  TRACE_TEXT(TRACE_MAIN_STEPS,
             "Bind locally (" << objString << ")\n");
  if (ctxt == LOCALAGENT) {
    ctxt = AGENTCTXT;
    /* Specific case for local agent.
       It is added into the cache to avoid resolution
       problems later.
    */
    LocalAgent_ptr agent = getLocalAgent(name.c_str());
    cachesMutex.lock();
    objectCache[ctxt+"/"+name] = CORBA::Object::_duplicate(agent);
    cachesMutex.unlock();
  } else {
    if (ctxt == MASTERAGENT) {
      ctxt = AGENTCTXT;
      /* Specific case for master agent.
         It is added into the cache to avoid resolution
         problems later.
      */
      MasterAgent_ptr agent = getMasterAgent(name.c_str());
      cachesMutex.lock();
      objectCache[ctxt+"/"+name] = CORBA::Object::_duplicate(agent);
      cachesMutex.unlock();
    }
  }
  /* NEW: Tag the object with the forwarder name. */
  std::string newIOR =
    ORBMgr::convertIOR(ior, std::string("@") + getName(), 0);

  ORBMgr::getMgr()->bind(ctxt, name, newIOR, true);
  // Broadcast the binding to all forwarders.
  ORBMgr::getMgr()->fwdsBind(ctxt, name, newIOR, this->name);
  TRACE_TEXT(TRACE_MAIN_STEPS,
             "Binded! (" << ctxt << "/" << name << ")\n");
}

/* Return the local bindings. Result is a set of couple
 * (object name, object ior)
 */
SeqString*
CorbaForwarder::getBindings(const char* ctxt) {
  std::list<std::string> objects;
  std::list<std::string>::iterator it;
  SeqString* result = new SeqString();
  unsigned int cmpt = 0;

  objects = ORBMgr::getMgr()->list(ctxt);
  result->length(objects.size()*2);

  for (it = objects.begin(); it != objects.end(); ++it) {
    try {
      CORBA::Object_ptr obj =
        ORBMgr::getMgr()->resolveObject(ctxt, it->c_str(), "no-Forwarder");
      (*result)[cmpt++] = it->c_str();
      (*result)[cmpt++] = ORBMgr::getMgr()->getIOR(obj).c_str();
    } catch (const std::runtime_error& err) {
      continue;
    }
  }
  result->length(cmpt);
  return result;
}

void CorbaForwarder::unbind(const char* objName) {
  std::string objString(objName);
  std::string name;
  std::string ctxt;

  if (!remoteCall(objString)) {
    return getPeer()->unbind(objString.c_str());
  }

  name = objString;

  if (name.find('/') == std::string::npos) {
    return;
  }

  ctxt = getCtxt(objString);
  name = getName(objString);

  removeObjectFromCache(name);

  ORBMgr::getMgr()->unbind(ctxt, name);
  // Broadcast the unbinding to all forwarders.
  ORBMgr::getMgr()->fwdsUnbind(ctxt, name, this->name);
}

void
CorbaForwarder::removeObjectFromCache(const std::string& name) {
  std::map<std::string, ::CORBA::Object_ptr>::iterator it;
  std::map<std::string, PortableServer::ServantBase*>::iterator jt;
  /* If the object is in the servant cache. */
  cachesMutex.lock();
  if ((jt = servants.find(name))!=servants.end()) {
    /* - Deactivate object. */
    try {
      ORBMgr::getMgr()->deactivate(jt->second);
    } catch (...) {
      /* There was a problem with the servant. But we want
       * to remove it...
       */
    }
    /* - Remove activated servants. */
    servants.erase(jt);
  }
  /* Remove the object from the cache. */
  if ((it = objectCache.find(name))!=objectCache.end())
    objectCache.erase(it);
  cachesMutex.unlock();
}

/* Remove non existing objects from the caches. */
void
CorbaForwarder::cleanCaches() {
  std::map<std::string, ::CORBA::Object_ptr>::iterator it;
  std::list<std::string> invalidObjects;
  std::list<std::string>::const_iterator jt;

  cachesMutex.lock();
  for (it = objectCache.begin(); it != objectCache.end(); ++it) {
    try {
      Forwarder_var object = Forwarder::_narrow(it->second);
      object->getName();
    } catch (const CORBA::TRANSIENT& err) {
      invalidObjects.push_back(it->first);
    }
  }
  cachesMutex.unlock();
  for (jt = invalidObjects.begin(); jt != invalidObjects.end(); ++jt)
    removeObjectFromCache(*jt);
}

void
CorbaForwarder::connectPeer(const char* ior, const char* host,
                           const ::CORBA::Long port) {
  std::string converted = ORBMgr::convertIOR(ior, host, port);
  setPeer(ORBMgr::getMgr()->resolve<Forwarder, Forwarder_ptr>(converted));
}

void
CorbaForwarder::setPeer(Forwarder_ptr peer) {
  this->peer = Forwarder::_duplicate(Forwarder::_narrow(peer));
  // Peer was init. The lock was done on the constructor.
  peerMutex.unlock();
}

Forwarder_var
CorbaForwarder::getPeer() {
  // Wait for setPeer
  peerMutex.lock();
  peerMutex.unlock();
  try {
    // Check if peer is still alive
    peer->getName();
  } catch (const CORBA::COMM_FAILURE& err) {
    // Lock peerMutex, then wait for setPeer
    // (setPeer() unlock the mutex
    peerMutex.lock();
    peerMutex.lock();
    peerMutex.unlock();
  } catch (const CORBA::TRANSIENT& err) {
    // Lock peerMutex, then wait for setPeer
    // (setPeer() unlock the mutex
    peerMutex.lock();
    peerMutex.lock();
    peerMutex.unlock();
  }
  return peer;
}


char*
CorbaForwarder::getIOR() {
  return CORBA::string_dup(ORBMgr::getMgr()->getIOR(_this()).c_str());
}

char*
CorbaForwarder::getName() {
  return CORBA::string_dup(name.c_str());
}

char*
CorbaForwarder::getPeerName() {
  return CORBA::string_dup(getPeer()->getName());
}

char*
CorbaForwarder::getHost() {
  return CORBA::string_dup(host.c_str());
}

char*
CorbaForwarder::getPeerHost() {
  return CORBA::string_dup(getPeer()->getHost());
}

SeqString*
CorbaForwarder::routeTree() {
  SeqString* result = new SeqString();
  return result;
}


std::list<std::string>
CorbaForwarder::otherForwarders() const {
  ORBMgr* mgr = ORBMgr::getMgr();
  std::list<std::string> result = mgr->list(FWRDCTXT);

  result.remove(name);
  return result;
}

std::string
CorbaForwarder::getName(const std::string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos == std::string::npos) {
    return namectxt;
  }
  return namectxt.substr(pos+1);
}

std::string
CorbaForwarder::getCtxt(const std::string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos == std::string::npos) {
    return "";
  }
  return namectxt.substr(0, pos);
}
