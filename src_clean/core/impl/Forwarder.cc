
#include <string>
#include <map>
#include <list>

#include <omnithread.h>

#include "CorbaForwarder.hh"
#include "common_types.hh"


#include "Forwarder.hh"
#include "ORBMgr.hh"
#include "common_types.hh"

#include <stdexcept>
#include <string>
#include <cstring>
#include <list>
#include <unistd.h>  // For gethostname()

#include <iostream>

#ifdef MAXHOSTNAMELEN
#define MAX_HOSTNAME_LENGTH MAXHOSTNAMELEN
#else
#define MAX_HOSTNAME_LENGTH  255
#endif

using namespace std;

bool Forwarder::remoteCall(string& objName) {
  if (objName.find("remote:")!=0) {
    /* Local network call: need to be forwarded to
     * the peer forwarder. Add the prefix.
     */
    objName = "remote:"+objName;
    return false;
  }
  /* Remote network call. Remove the prefix. */
  objName = objName.substr(strlen("remote:"));
  return true;
}


::CORBA::Object_ptr Forwarder::getObjectCache(const std::string& name) {
  map<string, ::CORBA::Object_ptr>::iterator it;
	
  cachesMutex.lock();
  if ((it=objectCache.find(name))!=objectCache.end()) {
    cachesMutex.unlock();
    return CORBA::Object::_duplicate(it->second);
  }
  cachesMutex.unlock();
  return ::CORBA::Object::_nil();	
}

Forwarder::Forwarder(const string& name)
{
  char buffer[MAX_HOSTNAME_LENGTH+1];
  
  gethostname(buffer, MAX_HOSTNAME_LENGTH);
  this->name = name;
  
  this->host = buffer;
  // Wait for the peer init. The unlock will be done on setPeer().
  peerMutex.lock(); 
}


/* Common methods implementations. */
::CORBA::Long Forwarder::ping(const char* objName)
{
  return ::CORBA::Long(0);
}


char* Forwarder::getHostname(const char* objName)
{
}


void Forwarder::bind(const char* objName, const char* ior, const char* connectId) {
  /* To avoid crashes when the peer forwarder is not ready: */
  /* If the peer was not initialized, the following call is blocking. */
  peerMutex.lock();
  peerMutex.unlock();

  string objString(objName);
  string name;
  string ctxt;

  if (!remoteCall(objString)) {
    std::cout << "Forward bind to peer (" << objName << ")" << std::endl;
    return getPeer()->bind(objString.c_str(), ior, connectId);
  }
  ctxt = getCtxt(objString);
  name = getName(objString);
  std::cout << "Bind locally (" << objString << ")" << std::endl;
  /* NEW: Tag the object with the forwarder name. */
  string newIOR = ORBMgr::convertIOR(ior, string("@")+getName(), 0);

  ORBMgr::getMgr()->bind(ctxt, name, newIOR, connectId, true);
  // Broadcast the binding to all forwarders.
  ORBMgr::getMgr()->fwdsBind(ctxt, name, newIOR, connectId, this->name);
  std::cout << "Binded! (" << ctxt << "/" << name << ")" << std::endl;
}

/* Return the local bindings. Result is a set of couple
 * (object name, object ior)
 */
SeqString* Forwarder::getBindings(const char* ctxt) {
  list<string> objects;
  list<string>::iterator it;
  SeqString* result = new SeqString();
  unsigned int cmpt = 0;
	
  objects = ORBMgr::getMgr()->list(ctxt);
  result->length(objects.size()*2);

  for (it=objects.begin(); it!=objects.end(); ++it) {
    try {
      CORBA::Object_ptr obj = ORBMgr::getMgr()->resolveObject(ctxt, it->c_str(),"no-Forwarder", "no-Forwarder");
      (*result)[cmpt++]=it->c_str();
      (*result)[cmpt++]=ORBMgr::getMgr()->getIOR(obj).c_str();
    } catch (const runtime_error& err) {
      continue;
    }
  }
  result->length(cmpt);
  return result;
}

void Forwarder::unbind(const char* objName) {
  string objString(objName);
  string name;
  string ctxt;
	
  if (!remoteCall(objString)) {
    return getPeer()->unbind(objString.c_str());
  }
	
  name = objString;
	
  if (name.find('/')==string::npos) {
    return;
  }
	
  ctxt = getCtxt(objString);
  name = getName(objString);
	
  removeObjectFromCache(name);
	
  ORBMgr::getMgr()->unbind(ctxt, name);
  // Broadcast the unbinding to all forwarders.
  ORBMgr::getMgr()->fwdsUnbind(ctxt, name, this->name);
}

void Forwarder::removeObjectFromCache(const string& name) {
  map<string, ::CORBA::Object_ptr>::iterator it;
  map<string, PortableServer::ServantBase*>::iterator jt;
  /* If the object is in the servant cache. */
  cachesMutex.lock();
  if ((jt=servants.find(name))!=servants.end()) {
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
  if ((it=objectCache.find(name))!=objectCache.end())
    objectCache.erase(it);
  cachesMutex.unlock();
}

/* Remove non existing objects from the caches. */
void Forwarder::cleanCaches() {
  map<string, ::CORBA::Object_ptr>::iterator it;
  list<string> invalidObjects;
  list<string>::const_iterator jt;
	
  cachesMutex.lock();
  for (it=objectCache.begin(); it!=objectCache.end(); ++it) {
    try {
      CorbaForwarder_var object = CorbaForwarder::_narrow(it->second);
      object->getName();
    } catch (const CORBA::TRANSIENT& err) {
      invalidObjects.push_back(it->first);
    }
  }
  cachesMutex.unlock();
  for (jt=invalidObjects.begin(); jt!=invalidObjects.end(); ++jt)
    removeObjectFromCache(*jt);
}

void Forwarder::connectPeer(const char* ior, const char* host,
				const ::CORBA::Long port) {
  string converted = ORBMgr::convertIOR(ior, host, port);
  setPeer(ORBMgr::getMgr()->resolve<CorbaForwarder, CorbaForwarder_ptr>(converted));
}

void Forwarder::setPeer(CorbaForwarder_ptr peer) {
  this->peer = CorbaForwarder::_duplicate(CorbaForwarder::_narrow(peer));
  // Peer was init. The lock was done on the constructor.
  peerMutex.unlock();
}

CorbaForwarder_var Forwarder::getPeer() {
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


char* Forwarder::getIOR() {
  return CORBA::string_dup(ORBMgr::getMgr()->getIOR(_this()).c_str());
}

char* Forwarder::getName() {
  return CORBA::string_dup(name.c_str());
}

char* Forwarder::getPeerName() {
  return CORBA::string_dup(getPeer()->getName());
}

char* Forwarder::getHost() {
  return CORBA::string_dup(host.c_str());
}

char* Forwarder::getPeerHost() {
  return CORBA::string_dup(getPeer()->getHost());
}

SeqString* Forwarder::routeTree() {
  SeqString* result = new SeqString();
  return result;
}


list<string> Forwarder::otherForwarders() const {
  ORBMgr* mgr = ORBMgr::getMgr();
  list<string> result = mgr->list(FWRDCTXT);
  
  result.remove(name);
  return result;
}

string Forwarder::getName(const string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos==string::npos) return namectxt;
  return namectxt.substr(pos+1);
}

string Forwarder::getCtxt(const string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos==string::npos) return "";
  return namectxt.substr(0, pos);
}

void 
Forwarder::getRequest(const ::corba_request_t& req, const char* objName) {
  return;
}
