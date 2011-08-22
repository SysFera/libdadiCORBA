/**
 * \file ORBMgr.hh
 * \brief This file defines the ORBMgr class
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */

#ifndef ORBMGR_HH
#define ORBMGR_HH

#include <string>
#include <map>
#include <list>

#include <omniORB4/CORBA.h>
#include <sys/types.h>

#include <omnithread.h>



#define FWRDCTXT "forwarder"

using namespace std;

#include "Connector.hh"
#include "Forwarder.hh"


class ORBMgr;

class ORBMgr {
public:
  /* Constructors. */
  ORBMgr(int argc, char* argv[]);
  ORBMgr(CORBA::ORB_ptr ORB);
  ORBMgr(CORBA::ORB_ptr ORB, PortableServer::POA_var POA);
  
  /* Destructor. */
  ~ORBMgr();

  /* Bind the object using its ctxt/name */
  void bind(const std::string& ctxt, const std::string& name,
            CORBA::Object_ptr object, const bool rebind = false) const;
  /* Bind an object using its IOR. */
  void bind(const std::string& ctxt, const std::string& name,
            const std::string& IOR, const bool rebind = false) const;
  /* Rebind objects. */
  void rebind(const std::string& ctxt, const std::string& name,
              CORBA::Object_ptr object) const;
  void rebind(const std::string& ctxt, const std::string& name,
              const std::string& IOR) const;
  /* Unbind an object. */
  void unbind(const std::string& ctxt, const std::string& name) const;
	
  /* Forwarders binding. */
  void fwdsBind(const std::string& ctxt, const std::string& name,
                const std::string& ior, const std::string& fwName = "") const;
  /* Forwarders unbinding. */
  void fwdsUnbind(const std::string& ctxt, const std::string& name,
                  const std::string& fwName = "") const;
	
  /* Resolve an object using its IOR or ctxt/name. */
  CORBA::Object_ptr resolveObject(const std::string& IOR) const;
  CORBA::Object_ptr resolveObject(const std::string& ctxt, const std::string& name,
				  const string& connectId,
				  const std::string& fwdName = "") const;
	
  /* Get the list of the objects id binded in the omniNames server for a given context. */
  std::list<std::string> list(CosNaming::NamingContext_var& ctxt) const;
  std::list<std::string> list(const std::string& ctxtName) const;
  
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr resolve(const std::string& ctxt, const std::string& name,
		    const std::string& fwdName = "") const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject(ctxt, name, fwdName)));
  }
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr resolve(const std::string& IOR) const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject(IOR)));
  }
  
  /* Return the IOR of the passed object. */
  std::string getIOR(CORBA::Object_ptr object) const;
  std::string getIOR(const std::string& ctxt, const std::string& name, std::string id) const;
  
  /* Activate an object. */
  void activate(PortableServer::ServantBase* object) const;
  /* Deactivate an object. */
  void deactivate(PortableServer::ServantBase* object) const;
  
  /* Wait for the request on activated objects. */
  void wait() const;
  void shutdown(bool waitForCompletion);

  static void init(int argc, char* argv[]);
  
  static ORBMgr* getMgr();
  
  /* IOR management functions. */
  static void makeIOR(const std::string& strIOR, IOP::IOR& ior);
  static void makeString(const IOP::IOR& ior, std::string& strIOR);
  static std::string getHost(IOP::IOR& ior);
  static std::string getHost(const std::string& strIOR);
  static unsigned int getPort(IOP::IOR& ior);
  static unsigned int getPort(const std::string& strIOR);
  static std::string getTypeID(IOP::IOR& ior);
  static std::string getTypeID(const std::string& strIOR);
  static std::string convertIOR(IOP::IOR& ior, const std::string& host,
				const unsigned int port);
  static std::string convertIOR(const std::string& ior, const std::string& host,
				const unsigned int port);
  
  /* Object cache management functions. */
  void resetCache() const;
  void removeObjectFromCache(const std::string& name) const;
  void removeObjectFromCache(const std::string& ctxt,
			     const std::string& name) const;
  void cleanCache() const;
  static void hexStringToBuffer(const char* ptr, const size_t size,
				cdrMemoryStream& buffer);

  /**
   * \brief To add a connector on the orbmgr. it does not add an existing connector twice
   * \param conn: The connector
   * \param id: The id of the connector. The id is out of the connector to make possible having the same connector functionnalities for 2 objects. Otherwize a connector would be forced to be unique
   */
  void
  addConnector(Connector* conn, string id);

  /**
   * \brief To get the connector corresponding to id in the map
   * \param id: The id of the desired connector
   * \return The desired connector or null if it is not found
   */
  Connector*
  getConnector(string id) const;

private:
  /* Map containing the connectors */
  std::map<string, Connector*> mconn;

  /* The omniORB Object Request Broker for this manager. */
  CORBA::ORB_ptr ORB;
  /* The Portable Object Adaptor. */
  PortableServer::POA_var POA;
  
  /* Is the ORB down? */
  bool down;
  
  /* CORBA initialization. */
  void init(CORBA::ORB_ptr ORB);

  /* Object cache to avoid to contact OmniNames too many times. */
  mutable std::map<std::string, CORBA::Object_ptr> cache;
  /* Cache mutex. */
  mutable omni_mutex cacheMutex;

  /* The manager instance. */
  static ORBMgr* theMgr;

  static void
  sigIntHandler(int sig);
#ifndef __cygwin__
  static omni_mutex waitLock;
#else
  static sem_t waitLock;
#endif


};


#endif
