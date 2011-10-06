/**
 * \file Connector.hh
 * \brief This file defines the root connector class
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */


/**
 *
 * Root class of the connector, connector play proxy between ORBMgr and calling classes/programs
 *
 */


#ifndef CONNECTOR_HH
#define CONNECTOR_HH

#include <string>
#include<map>

#include <omniORB4/CORBA.h>
#include <sys/types.h>

#include <omnithread.h>

#include "CorbaForwarder.hh"
#include "Forwarder.hh"

#include "common_types.hh"
#include "NetConfig.hh"
#include "ORBMgr.hh"

class Connector;

using namespace std;

class Connector {

public :
  /**
   * \brief Constructor
   */
  Connector(ORBMgr* mgr);
  /**
   * \brief Destructor
   */
  ~Connector();
  /**
   * \brief To register the connector in an ORBMgr
   * \param id: The id to use to register
   */
  void
  registrer(std::string id);
  /**
   * \brief To get the context of the object
   */
  virtual std::string
  getContext(std::string context);
  /**
   * \brief To get the object corresponding to a context and a name in the omninames
   * \param ctxt: The context
   * \param name: The name of the object
   * \return The corresponding corba object
   */
  virtual CORBA::Object_ptr
  getObject(std::string ctxt, std::string name/*, /*CorbaForwarder_var* v*/, string connectId, std::string globContext);

  /* Bind the object using its ctxt/name */
  virtual void bind(const std::string& ctxt, const std::string& name,
		    CORBA::Object_ptr object, const std::string& connect, const bool rebind = false) const;
  /* Bind an object using its IOR. */
  virtual void bind(const std::string& ctxt, const std::string& name,
            const std::string& IOR, const std::string& connect, const bool rebind = false) const;
  /* Rebind objects. */
  virtual void rebind(const std::string& ctxt, const std::string& name,
              CORBA::Object_ptr object, const std::string& connect) const;
  virtual void rebind(const std::string& ctxt, const std::string& name,
              const std::string& IOR, const std::string& connect) const;
  /* Unbind an object. */
  virtual void unbind(const std::string& ctxt, const std::string& name) const;
	
  /* Forwarders binding. */
  virtual void fwdsBind(const std::string& ctxt, const std::string& name,
			const std::string& ior, const std::string& conectId, 
			const std::string& fwName = "") const;
  /* Forwarders unbinding. */
  virtual void fwdsUnbind(const std::string& ctxt, const std::string& name,
                  const std::string& fwName = "") const;
	
  /* Resolve an object using its IOR or ctxt/name. */
  virtual CORBA::Object_ptr resolveObject(const std::string& IOR) const;
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA::Object_ptr resolveObject(const std::string& ctxt, const std::string& name,
				  const string& connectId,
					  const std::string& fwdName = "") const {
    return mmgr->resolve<CORBA_object, CORBA_ptr>(ctxt, name, connectId, fwdName); 
  }
	
  /* Get the list of the objects id binded in the omniNames server for a given context. */
  virtual std::list<std::string> list(CosNaming::NamingContext_var& ctxt) const;
  virtual std::list<std::string> list(const std::string& ctxtName) const;
  
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr resolve(const std::string& ctxt, const std::string& name,
		    const std::string& connectId,
		    const std::string& fwdName = "") const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject<CORBA_object, CORBA_ptr>(ctxt, name, connectId, fwdName)));
  }
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr resolve(const std::string& IOR) const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject(IOR)));
  }
  
  /* Return the IOR of the passed object. */
  virtual std::string getIOR(CORBA::Object_ptr object) const;
  virtual std::string getIOR(const std::string& ctxt, const std::string& name) const;
  
  /* Activate an object. */
  void activate(PortableServer::ServantBase* object) const;
  /* Deactivate an object. */
  void deactivate(PortableServer::ServantBase* object) const;
  
  /* Wait for the request on activated objects. */
  void wait() const;
  void shutdown(bool waitForCompletion);

  void init(int argc, char* argv[]);
  
  ORBMgr* getMgr();
  
  /* IOR management functions. */
  void makeIOR(const std::string& strIOR, IOP::IOR& ior);
  void makeString(const IOP::IOR& ior, std::string& strIOR);
  std::string getHost(IOP::IOR& ior);
  std::string getHost(const std::string& strIOR);
  unsigned int getPort(IOP::IOR& ior);
  unsigned int getPort(const std::string& strIOR);
  std::string getTypeID(IOP::IOR& ior);
  std::string getTypeID(const std::string& strIOR);
  std::string convertIOR(IOP::IOR& ior, const std::string& host,
				const unsigned int port);
  std::string convertIOR(const std::string& ior, const std::string& host,
				const unsigned int port);
  
  /* Object cache management functions. */
  void resetCache() const;
  void removeObjectFromCache(const std::string& name) const;
  void removeObjectFromCache(const std::string& ctxt,
			     const std::string& name) const;
  void cleanCache() const;
  void hexStringToBuffer(const char* ptr, const size_t size,
				cdrMemoryStream& buffer);


protected :
  ORBMgr* mmgr;
  std::string mname;
private :

} ; //end class connector

#endif
