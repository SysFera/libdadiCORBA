
#ifndef FORWARDER_HH
#define FORWARDER_HH 

#include <string>
#include <map>
#include <list>

#include <omnithread.h>

#include "CorbaForwarder.hh"
#include "common_types.hh"

class Forwarder : public POA_CorbaForwarder,
                      public PortableServer::RefCountServantBase
{
private:
  /* When a new forwarder object is created, we cache it.
   * Because this kind of object contains only the object
   * name and a reference to this forwarder, there is no
   * risk to cache it, even if the object is restarted or
   * disappear.
   */
  std::map<std::string, ::CORBA::Object_ptr> objectCache;
  /* We also maintain a list of activated servant objects. */
  std::map<std::string, PortableServer::ServantBase*> servants;
	
  virtual ::CORBA::Object_ptr 
  getObjectCache(const std::string& name);
  /* The forwarder associated to this one. */
  CorbaForwarder_var peer;
  /* Mutexes */
  omni_mutex peerMutex;   // To wait for the peer initialization
  omni_mutex cachesMutex; // Protect access to caches
	
  std::string peerName;
  std::string name;
  std::string host;
public:
  Forwarder(const std::string& name);
	
  /* Common methods implementations. */
  virtual ::CORBA::Long ping(const char* objName);
  virtual void getRequest(const ::corba_request_t& req, const char* objName);
  virtual char* getHostname(const char* objName);
	
  /* To determine if the call is from another forwarder and
   * modify the object name.
   */
  static bool remoteCall(std::string& objName);
  
  /* CORBA remote management implementation. */
  virtual void bind(const char* objName, const char* ior);
  virtual void unbind(const char* objName);
  virtual SeqString* getBindings(const char* ctxt);
	
  /* Connect the peer forwarder. */
  virtual void connectPeer(const char* ior, const char* host,
                   const ::CORBA::Long port);
  /* Set this forwarder peer object (not CORBA). */
  virtual void setPeer(CorbaForwarder_ptr peer);
  virtual CorbaForwarder_var getPeer();
  virtual char* getIOR();
	
  /* Object caches management functions. */
  virtual void removeObjectFromCache(const std::string& name);
  virtual void cleanCaches();
	
  virtual char* getName();
  virtual char* getPeerName();
  virtual char* getHost();
  virtual char* getPeerHost();

  virtual SeqString* routeTree();
  
  /* Utility function. */
  virtual std::list<std::string> otherForwarders() const;

  /* Utility fonctions to extract name & context from context/name. */
  static std::string getName(const std::string& namectxt);
  static std::string getCtxt(const std::string& namectxt);
	
};

#endif
