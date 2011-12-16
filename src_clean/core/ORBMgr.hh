/**
 * @file ORBMgr.hh
 *
 * @brief  ORB manager v. 2.0 - CORBA management with DIET forwarders
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef ORBMGR_HH
#define ORBMGR_HH

#include <string>
#include <map>
#include <list>

#include <omniORB4/CORBA.h>
#include <sys/types.h>

#include <omnithread.h>

#include "Forwarder.hh"
#include "dadi/Logging/Logger.hh"

#define DAGDACTXT   "Dagda"
#define LOGCOMPCTXT "LogServiceC"
#define AGENTCTXT   "dietAgent"
#define SEDCTXT     "dietSeD"
#define MADAGCTXT   "dietMADag"
#define WFMGRCTXT   "dietWfMgr"
#define CLIENTCTXT  "dietClient"
#define WFLOGCTXT   "WfLogService"
#define FWRDCTXT    "dietForwarder"
/* LOCALAGENT & MASTERAGENT are not context strings. */
#define LOCALAGENT  "localAgent"
#define MASTERAGENT "masterAgent"
#define LOGCOMPCTXT     "LogServiceC"
#define LOGTOOLCTXT     "LogServiceT"
#define LOGCOMPCONFCTXT "LogServiceC2"
#define LOGTOOLMSGCTXT  "LogServiceT2"
#define LOGFWRDCTXT     "LogForwarder"
#define DIETFWRD        "DietForwarder"

/**
 * @brief The ORBMgr class handles all the CORBA objects and connections
 * @class ORBMgr
 */
class ORBMgr {
public:
  /* Constructors. */
  ORBMgr(int argc, char* argv[]);

  explicit ORBMgr(CORBA::ORB_ptr ORB);

  ORBMgr(CORBA::ORB_ptr ORB, PortableServer::POA_var POA);

  /* Destructor. */
  ~ORBMgr();

  /* Bind the object using its ctxt/name */
  void
  bind(const std::string& ctxt, const std::string& name,
       CORBA::Object_ptr object, const bool rebind = false) const;

  /* Bind an object using its IOR. */
  void
  bind(const std::string& ctxt, const std::string& name,
       const std::string& IOR, const bool rebind = false) const;

  /* Rebind objects. */
  void
  rebind(const std::string& ctxt, const std::string& name,
         CORBA::Object_ptr object) const;

  void
  rebind(const std::string& ctxt, const std::string& name,
         const std::string& IOR) const;

  /* Unbind an object. */
  void
  unbind(const std::string& ctxt, const std::string& name) const;

  /* Forwarders binding. */
  void
  fwdsBind(const std::string& ctxt, const std::string& name,
           const std::string& ior, const std::string& fwName = "") const;
  /* Forwarders unbinding. */
  void
  fwdsUnbind(const std::string& ctxt, const std::string& name,
             const std::string& fwName = "") const;

  /* Resolve an object using its IOR or ctxt/name. */
  CORBA::Object_ptr
  resolveObject(const std::string& IOR) const;

  CORBA::Object_ptr
  resolveObject(const std::string& ctxt, const std::string& name,
                const std::string& fwdName = "") const;
  /* Resolve objects without object caching or invoking forwarders. */
  CORBA::Object_ptr
  simpleResolve(const std::string& ctxt, const std::string& name) const;

  /* Get the list of the objects id binded in the omniNames server
     for a given context. */
  std::list<std::string>
  list(CosNaming::NamingContext_var& ctxt) const;

  std::list<std::string>
  list(const std::string& ctxtName) const;

  /* Get the list of declared CORBA contexts. */
  std::list<std::string>
  contextList() const;

  /* Return true if the object is local, false if it is reachable through
     a forwarder. */
  bool
  isLocal(const std::string& ctxt, const std::string& name) const;

  /* Return the name of the forwarder that manage the object. */
  std::string
  forwarderName(const std::string& ctxt, const std::string& name) const;

  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr
  resolve(const std::string& ctxt, const std::string& name,
          const std::string& fwdName = "") const {
    return CORBA_object::_duplicate(
      CORBA_object::_narrow(resolveObject(ctxt, name, fwdName)));
  }

  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr
  resolve(const std::string& IOR) const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject(IOR)));
  }


  /* Return the IOR of the passed object. */
  std::string
  getIOR(CORBA::Object_ptr object) const;

  std::string
  getIOR(const std::string& ctxt, const std::string& name) const;

  /* Activate an object. */
  void
  activate(PortableServer::ServantBase* object) const;

  /* Deactivate an object. */
  void
  deactivate(PortableServer::ServantBase* object) const;

  /* Wait for the request on activated objects. */
  void
  wait() const;

  void
  shutdown(bool waitForCompletion);

  static void
  init(int argc, char* argv[]);

  static ORBMgr*
  getMgr();

  std::list<std::string>
  forwarderObjects(const std::string& fwdName, const std::string& ctxt) const;

  std::list<std::string>
  localObjects(const std::string& ctxt) const;

  /* IOR management functions. */
  static void
  makeIOR(const std::string& strIOR, IOP::IOR& ior);

  static void
  makeString(const IOP::IOR& ior, std::string& strIOR);

  static std::string
  getHost(IOP::IOR& ior);

  static std::string
  getHost(const std::string& strIOR);

  static unsigned int
  getPort(IOP::IOR& ior);

  static unsigned int
  getPort(const std::string& strIOR);

  static std::string
  getTypeID(IOP::IOR& ior);

  static std::string
  getTypeID(const std::string& strIOR);

  static std::string
  convertIOR(IOP::IOR& ior, const std::string& host, const unsigned int port);

  static std::string
  convertIOR(const std::string& ior, const std::string& host,
             const unsigned int port);

  /* Object cache management functions. */
  void
  resetCache() const;

  void
  removeObjectFromCache(const std::string& name) const;

  void
  removeObjectFromCache(const std::string& ctxt, const std::string& name) const;

  void
  cleanCache() const;

  static void
  hexStringToBuffer(const char* ptr, const size_t size,
                    cdrMemoryStream& buffer);

private:
  /* CORBA initialization. */
  void
  init(CORBA::ORB_ptr ORB);

  static void
  sigIntHandler(int sig);

  /* The omniORB Object Request Broker for this manager. */
  CORBA::ORB_ptr mORB;
  /* The Portable Object Adaptor. */
  PortableServer::POA_var mPOA;

  /* Is the ORB down? */
  bool mdown;

  /* Object cache to avoid to contact OmniNames too many times. */
  mutable std::map<std::string, CORBA::Object_ptr> mcache;
  /* Cache mutex. */
  mutable omni_mutex mcacheMutex;

  /* The manager instance. */
  static ORBMgr* theMgr;

// Logger
  dadi::LoggerPtr mlogger;
// Channel for logger
  dadi::ChannelPtr mcc;


#ifndef __cygwin__
  static omni_mutex waitLock;
#else
  static sem_t waitLock;
#endif
};


#endif
