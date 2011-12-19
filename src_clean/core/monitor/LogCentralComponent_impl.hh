/**
 * @file LogCentralComponent_impl.cc
 *
 * @brief Implementation corresponding to the LogComponentComponent interface
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _LOGCENTRALCOMPONENT_IMPL_HH_
#define _LOGCENTRALCOMPONENT_IMPL_HH_

#include "LogTypes.hh"
#include "LogComponent.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "utils/FullLinkedList.hh"

#include "CorbaForwarder.hh"


/**
 * Errorlevel constants for connectComponent (defined in the idl)
const short LS_OK
const short LS_COMPONENT_CONNECT_BADNAME
const short LS_COMPONENT_CONNECT_ALREADYEXISTS
const short LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR
const short LS_COMPONENT_CONNECT_INTERNALERROR
const short LS_COMPONENT_DISCONNECT_NOTEXISTS
 */


/**
 * @brief Implements the Component-Servant on the LogCentral.
 * The Servant is responsible for connection and deconnection components,
 * forwarding messages and synchronisation. This includes:
 * - store components in internal list
 * - correct time for incoming messages
 *   and forward them to the timebuffer
 * - synchronize with components
 * @class LastPing
 */
class LastPing
{
public:
  LastPing();
  LastPing(LastPing& newLastPing);
  ~LastPing();
  LastPing&
  operator=(LastPing& newLastPing);
  char* name;
  log_time_t time;
  log_time_t timeDifference;
};
typedef FullLinkedList<LastPing> LastPings;

/**
 * @brief The implementation of the log component interface
 * @class LogCentralComponent_impl
 */
class LogCentralComponent_impl: public POA_LogCentralComponent,
                                public PortableServer::RefCountServantBase {
public:
  LogCentralComponent_impl(ComponentList* componentList,
                           FilterManagerInterface* filterManager,
                           TimeBuffer* timeBuffer);
  ~LogCentralComponent_impl();


  /**
   * @brief Connect a component to the LogCentral. The component attaches with its
   * name, which must be unique (among all components) and its hostname (no
   * need to be unique). If the name is an empty string, a name is generated.
   * The return value indicates if the component could be connected.
   * If ALREADYEXISTS is returned, another component with the given name is
   * registered. In this case, the connection was not successful, and the tool
   * must connect with a diferent name before invoking other functions of the
   * LogCentralComponent interface.
   * @param componentName unique name of the component(cannot be *) or the
   * empty string "" for automatic naming.
   * @param componentHostname hostname of the component
   * @param message message to pass to the tools at the connection
   * @param compConfigurator component servant which can process
   *   configuration changes for the component
   * @param componentTime localtime on the component side
   * @param initialConfig holds configuration for the components
   *   tag filter after the call.
   * @returns value indicating if the component could be connected
   */
  CORBA::Short
  connectComponent(char*& componentName,
                   const char* componentHostname,
                   const char* message,
                   const char* compConfigurator,
                   const log_time_t& componentTime,
                   tag_list_t& initialConfig);


  /**
   * @brief Disconnect a connected component from the LogCentral.
   * No further messages should be sent after disconnection.
   * The componentConfigurator will no longer be used by the LogCentral after
   * this call. Returns NOTCONNECTED if the calling component was not
   * registered.
   * @param componentName name of the component
   * @param message message to pass to the tools at the disconnection
   * @returns value indicating if the component could be disconnected
   */
  CORBA::Short
  disconnectComponent(const char* componentName, const char* message);


  /**
   * @brief Send a buffer of log_msg_t to the LogCentral.
   * @param buffer sequence of log_msg_t to be sent
   */
  void
  sendBuffer(const log_msg_buf_t& buffer);

  /**
   * @brief Tell if a component exists or not.
   * @param name name of the component to find
   * @return true if the component exists
   */
  bool
  isComponentExists(const char* name, ComponentList::ReadIterator* it);

  /**
   * @brief To be called by a thread for saying that the component is still alive.
   * If the last ping is too old, the component is considered dead and
   * generate a disconnect message (OUT).
   * @param componentName the name of the component who send the ping
   */
  void
  ping(const char* componentName);

  /**
   * @brief To be called by a thread for clock synchronisation.
   * @param componentTime localtime on the component side
   * @param componentName the name of the component who send the synchro
   */
  void
  synchronize(const char* componentName, const log_time_t& componentTime);

  void
  test();


  /**
   * Uselessbut to have the same idl as in log
   */
//  char*
//  getHostnameOf(const char* toto);

private:
  /**
   * @brief Generate an unique name based on the hostname
   * @param hostname the hostname to base to form the name
   * @return a unique name
   */
  char*
  getGeneratedName(const char* hostname, ComponentList::ReadIterator* it);

private:
  /**
   * @brief A thread to check if it is alive
   * @class LogCentralComponent_impl
   */
  class AliveCheckThread:public omni_thread
  {
  public:
/**
 * @brief Constructor
 */
    AliveCheckThread(LogCentralComponent_impl* LCC);
/**
 * @brief Destructor
 */
    ~AliveCheckThread();
    /**
     * Start the thread. Return immediately.
     */
    void
    startThread();
    /**
     * Stop the thread. Return when the thread is stopped.
     */
    void
    stopThread();
  private:
    void*
    run_undetached(void* params);
  private:
    LogCentralComponent_impl* LCC;
    bool threadRunning;
  };

  friend class LogCentralComponent_impl::AliveCheckThread;

private:
  ComponentList* mcomponentList;
  FilterManagerInterface* mfilterManager;
  TimeBuffer* mtimeBuffer;
  LastPings* mlastPings;
  AliveCheckThread* maliveCheckThread;
}; // end class LogCentralComponen_impl


/**
 * @brief The implementation of the log component interface
 * @class LogCentralComponentFwdrImpl
 */
class LogCentralComponentFwdrImpl: public POA_LogCentralComponentFwdr,
				   public PortableServer::RefCountServantBase {
public:
  LogCentralComponentFwdrImpl(Forwarder_ptr fwdr,
			      const char *objName);

  ~LogCentralComponentFwdrImpl();

 CORBA::Short
  connectComponent(char*& componentName,
                   const char* componentHostname,
                   const char* message,
                   const char* compConfigurator,
                   const log_time_t& componentTime,
                   tag_list_t& initialConfig);

  CORBA::Short
  disconnectComponent(const char* componentName, const char* message);

  void
  sendBuffer(const log_msg_buf_t& buffer);

  void
  ping(const char* componentName);

  void
  test();


  void
  synchronize(const char* componentName, const log_time_t& componentTime);


protected:
	Forwarder_ptr forwarder;
	char* objName;

private:


  ComponentList* componentList;
  FilterManagerInterface* filterManager;
  TimeBuffer* timeBuffer;
  LastPings* lastPings;
}; // end class LogCentralComponentFwdr_impl


/**
 * @brief The implementation of the log component configurator
 * @class ComponentConfiguratorFwdr_impl
 */
class ComponentConfiguratorFwdr_impl: public POA_ComponentConfiguratorFwdr,
				   public PortableServer::RefCountServantBase {
public:
  ComponentConfiguratorFwdr_impl(Forwarder_ptr fwdr,
				 const char* objName);

  ~ComponentConfiguratorFwdr_impl();

  void setTagFilter(const tag_list_t& tagList);

  void addTagFilter(const tag_list_t& tagList);

  void removeTagFilter(const tag_list_t& tagList);

  void test();

protected:
	Forwarder_ptr forwarder;
	char* objName;

}; // end class LogCentralComponentFwdr_impl

#endif
