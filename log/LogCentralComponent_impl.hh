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
/**
 * @brief Constructor
 */
  LastPing();
/**
 * @brief Copy constructor
 */
  LastPing(LastPing& newLastPing);
/**
 * @brief Destructor
 */
  ~LastPing();
/**
 * @brief Affectation
 */
  LastPing&
  operator=(LastPing& newLastPing);
/**
 * @brief The name
 */
  char* name;
/**
 * @brief The time
 */
  log_time_t time;
/**
 * @brief The difference in time
 */
  log_time_t timeDifference;
};
/**
 * @brief A list of last ping
 */
typedef FullLinkedList<LastPing> LastPings;

/**
 * @brief The implementation of the log component interface
 * @class LogCentralComponent_impl
 */
class LogCentralComponent_impl: public POA_LogCentralComponent,
                                public PortableServer::RefCountServantBase {
public:
/**
 * @brief Constructor
 * @paramm componentList A list of component
 * @param filterManager A filer manager
 * timeBuffer A time buffer
 */
  LogCentralComponent_impl(ComponentList* componentList,
                           FilterManagerInterface* filterManager,
                           TimeBuffer* timeBuffer);
/**
 * @brief Destructor
 */
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

/**
 * @brief Dummy function
 */
  void
  test();

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
   * @class AliveCheckThread
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
/**
 * @brief To run the thread undetached
 * @param params The parameters
 * @return TODO
 */
    void*
    run_undetached(void* params);
  private:
/**
 * @brief The log central component
 */
    LogCentralComponent_impl* LCC;
/**
 * @brief If the thread is running
 */
    bool threadRunning;
  };

  friend class LogCentralComponent_impl::AliveCheckThread;

private:
/**
 * @brief A list of components
 */
  ComponentList* mcomponentList;
/**
 * @brief A filter manager
 */
  FilterManagerInterface* mfilterManager;
/**
 * @brief A time buffer
 */
  TimeBuffer* mtimeBuffer;
/**
 * @brief A last ping
 */
  LastPings* mlastPings;
/**
 * @brief Check if the thread is still alive
 */
  AliveCheckThread* maliveCheckThread;
}; // end class LogCentralComponen_impl


#endif
