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
  /**
   * @brief Constructors.
   * @param argc: C main parameter
   * @param argv: C main parameter
   */
  ORBMgr(int argc, char* argv[]);

  /**
   * @brief Constructors using an existing ORB.
   * @param ORB: The ORB
   */
  explicit ORBMgr(CORBA::ORB_ptr ORB);

  /**
   * @brief Constructors.
   * @param ORB: The ORB to use
   * @param POA: The POA to use
   */
  ORBMgr(CORBA::ORB_ptr ORB, PortableServer::POA_var POA);

  /**
   * @brief Destructor.
   */
  ~ORBMgr();

  /**
   * @brief Bind the object using its ctxt/name
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param object: The object to bind
   * @param rebind: If try to rebind this object when it is said to be already bound
   */
  void
  bind(const std::string& ctxt, const std::string& name,
       CORBA::Object_ptr object, const bool rebind = false) const;

  /**
   * @brief Bind an object using its IOR.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param IOR: The IOR of the object to bind
   * @param rebind: If try to rebind this object when it is said to be already bound
   */
  void
  bind(const std::string& ctxt, const std::string& name,
       const std::string& IOR, const bool rebind = false) const;

  /**
   * @brief Rebind objects.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param object: The object to rebind
   */
  void
  rebind(const std::string& ctxt, const std::string& name,
         CORBA::Object_ptr object) const;

  /**
   * @brief Rebind objects.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param IOR: The IOR of the object to rebind
   */
  void
  rebind(const std::string& ctxt, const std::string& name,
         const std::string& IOR) const;

  /**
   * @brief Unbind an object.
   * @param ctxt: The context to use
   * @param name: The name of the object
   */
  void
  unbind(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Forwarders binding.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param ior: The IOR of the object to forward
   * @param fwName: The name of the forwarder to use
   */
  void
  fwdsBind(const std::string& ctxt, const std::string& name,
           const std::string& ior, const std::string& fwName = "") const;
  /**
   * @brief Forwarders unbinding.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param fwName: The name of the forwarder to use
   */
  void
  fwdsUnbind(const std::string& ctxt, const std::string& name,
             const std::string& fwName = "") const;

  /**
   * @brief Resolve an object using its IOR.
   * @param IOR: The IOR of the object to resolve
   * @return A CORBA pointer to the object
   */
  CORBA::Object_ptr
  resolveObject(const std::string& IOR) const;

  /**
   * @brief Resolve an object using its ctxt/name.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param fwName: The name of the forwarder to use
   * @return A CORBA pointer to the object
   */
  CORBA::Object_ptr
  resolveObject(const std::string& ctxt, const std::string& name,
                const std::string& fwdName = "") const;
  /**
   * @brief Resolve objects without object caching or invoking forwarders.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @return A CORBA pointer to the object
   */
  CORBA::Object_ptr
  simpleResolve(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Get the list of the objects id binded in the omniNames server
   *   for a given context.
   * @param ctxt: The context to list all the object inside
   * @return A list of the names of the binded objects
   */
  std::list<std::string>
  list(CosNaming::NamingContext_var& ctxt) const;

  /**
   * @brief Get the list of the objects id binded in the omniNames server
   *   for a given context.
   * @param ctxt: The name of the context to list all the object inside
   * @return A list of the names of the binded objects
   */
  std::list<std::string>
  list(const std::string& ctxtName) const;

  /**
   * @brief Get the list of declared CORBA contexts.
   * @return A list of all the CORBA context in the omniNames
   */
  std::list<std::string>
  contextList() const;

  /**
   * @brief Return true if the object is local, false if it is reachable through
   *   a forwarder.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @return True if the object is local, false otherwise
   */
  bool
  isLocal(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Return the name of the forwarder that manage the object.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @return The name of the forwarder
   */
  std::string
  forwarderName(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Resolve an object
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @param fwdName: The name of the forwarder to use
   * @return A pointer to the object
   */
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr
  resolve(const std::string& ctxt, const std::string& name,
          const std::string& fwdName = "") const {
    return CORBA_object::_duplicate(
      CORBA_object::_narrow(resolveObject(ctxt, name, fwdName)));
  }

  /**
   * @brief Resolve an object from its IOR
   * @param IOR: The IOR of the object to resolve
   * @return A pointer to the object
   */
  template <typename CORBA_object, typename CORBA_ptr>
  CORBA_ptr
  resolve(const std::string& IOR) const {
    return CORBA_object::_duplicate(CORBA_object::_narrow(resolveObject(IOR)));
  }


  /**
   * @brief Return the IOR of the passed object.
   * @param object: The object to get the IOR
   * @return The IOR
   */
  std::string
  getIOR(CORBA::Object_ptr object) const;

  /**
   * @brief Return the IOR of the passed object.
   * @param ctxt: The context to use
   * @param name: The name of the object
   * @return The IOR
   */
  std::string
  getIOR(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Activate an object.
   * @param object: The POA to activate
   */
  void
  activate(PortableServer::ServantBase* object) const;

  /**
   * @brief Deactivate an object.
   * @param object: The POA to deactivate
   */
  void
  deactivate(PortableServer::ServantBase* object) const;

  /**
   * @brief Wait for the request on activated objects.
   */
  void
  wait() const;

  /**
   * @brief Shut down the ORBMgr
   * @param waitForCompletion: If shut immediatly or wait the completion to shut
   */
  void
  shutdown(bool waitForCompletion);

  /**
   * @brief Init the ORBMgr
   * @param argc: C main parameter
   * @param argv: C main parameter
   */
  static void
  init(int argc, char* argv[]);

  /**
   * @brief Return  the ORBMgr
   * @return A pointer to the ORBMgr
   */
  static ORBMgr*
  getMgr();

  /**
   * @brief List the objects in a context of a forwarder
   * @param fwdName: The name of the forwarder
   * @param ctxt: The context
   * @return The list of the name of the objects
   */
  std::list<std::string>
  forwarderObjects(const std::string& fwdName, const std::string& ctxt) const;

  /**
   * @brief List the local objects in a context
   * @param ctxt: The context
   * @return The list of the name of the local objects
   */
  std::list<std::string>
  localObjects(const std::string& ctxt) const;

  /* IOR management functions. */

  /**
   * @brief Make an IOP::IOR object using a stringified IOR.
   * @param strIOR: The IOR as a string
   * @param ior: The new IOR
   */
  static void
  makeIOR(const std::string& strIOR, IOP::IOR& ior);

  /**
   * @brief Convert IOP::IOR to a stringified IOR.
   * @param ior: The ior
   * @param strIOR: The stringfied IOR
   */
  static void
  makeString(const IOP::IOR& ior, std::string& strIOR);

  /**
   * @brief Get the hostname of the first profile in this IOR.
   * @param ior: The IOR
   * @return The hostname
   */
  static std::string
  getHost(IOP::IOR& ior);

  /**
   * @brief Get the hostname of the first profile in IOR passed
   * as a string.
   * @param strIOR: The IOR
   * @return The hostname
   */
  static std::string
  getHost(const std::string& strIOR);

  /**
   * @brief Get the port of the first profile in this IOR.
   * @param ior: The IOR
   * @return The port
   */
  static unsigned int
  getPort(IOP::IOR& ior);

  /**
   * @brief Get the port of the first profile in IOR passed
   * as a string.
   * @param strIOR: The IOR
   * @return The port
   */
  static unsigned int
  getPort(const std::string& strIOR);

  /**
   * @brief Get the type id of the IOR.
   * @param ior: The IOR
   * @return The dataType
   */
  static std::string
  getTypeID(IOP::IOR& ior);

  /**
   * @brief Get the type id of the IOR passed as a string.
   * @param strIOR: The IOR
   * @return The dataType
   */
  static std::string
  getTypeID(const std::string& strIOR);

  /**
   * @brief Convert the IOR to a string
   * @param ior: The IOR to convert
   * @param host: The hostname
   * @param port: The port number
   * @return The stringified IOR
   */
  static std::string
  convertIOR(IOP::IOR& ior, const std::string& host, const unsigned int port);

  /**
   * @brief Convert the IOR to a string
   * @param ior: The IOR to convert
   * @param host: The hostname
   * @param port: The port number
   * @return The stringified IOR
   */
  static std::string
  convertIOR(const std::string& ior, const std::string& host,
             const unsigned int port);

  /* Object cache management functions. */
  /**
   * @brief Reset the cache
   */
  void
  resetCache() const;

  /**
   * @brief Remove the object called name from the cache
   * @param name: The name of the object to remove
   */
  void
  removeObjectFromCache(const std::string& name) const;

  /**
   * @brief Remove the object called name in the context ctxt from the cache
   * @param ctxt: The context of the object to remove
   * @param name: The name of the object to remove
   */
  void
  removeObjectFromCache(const std::string& ctxt, const std::string& name) const;

  /**
   * @brief Clean the cache
   */
  void
  cleanCache() const;

  /**
   * @brief Translate the string passed as first argument in bytes and
   * record them into the buffer.
   * @param ptr: The source buffer
   * @param size: The size of the char
   * @param buffer: The output buffer
   */
  static void
  hexStringToBuffer(const char* ptr, const size_t size,
                    cdrMemoryStream& buffer);

private:
  /**
   * @brief CORBA initialization.
   * @param ORB: The ORB to use
   */
  void
  init(CORBA::ORB_ptr ORB);

  /**
   * @brief Handler for sigint signal interception
   * @param sig: The received signal
   */
  static void
  sigIntHandler(int sig);

  /**
   * @brief The omniORB Object Request Broker for this manager.
   */
  CORBA::ORB_ptr mORB;
  /**
   * @brief The Portable Object Adaptor.
   */
  PortableServer::POA_var mPOA;

  /**
   * @brief Is the ORB down?
   */
  bool mdown;

  /**
   * @brief Object cache to avoid to contact OmniNames too many times.
   */
  mutable std::map<std::string, CORBA::Object_ptr> mcache;
  /**
   * @brief Cache mutex.
   */
  mutable omni_mutex mcacheMutex;

  /**
   * @brief The manager instance.
   */
  static ORBMgr* theMgr;

  /**
   * @brief Logger to send all log messages
   */
  dadi::LoggerPtr mlogger;
  /**
   * @brief Channel for logger to send all messages
   */
  dadi::ChannelPtr mcc;

  /**
   * @brief Mutex to avoid thread problems
   */
#ifndef __cygwin__
  static omni_mutex waitLock;
#else
  static sem_t waitLock;
#endif
};


#endif
