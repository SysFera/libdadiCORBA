/****************************************************************************/
/* Header corresponding to the LogComponentComponent interface              */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.4  2010/12/13 12:21:14  kcoulomb
 * Clean types
 *
 * Revision 1.3  2010/12/03 12:40:27  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.2  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#ifndef _LOGCENTRALCOMPONENT_IMPL_HH_
#define _LOGCENTRALCOMPONENT_IMPL_HH_

#include "LogTypes.hh"
#include "LogComponent.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "FullLinkedList.hh"

#include "Forwarder.hh"
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
 * Implements the Component-Servant on the LogCentral.
 * The Servant is responsible for connection and deconnection components,
 * forwarding messages and synchronisation. This includes:
 * - store components in internal list
 * - correct time for incoming messages
 *   and forward them to the timebuffer
 * - synchronize with components
 */
class LastPing {
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

class LogCentralComponent_impl
  : public POA_LogCentralComponent,
    public PortableServer::RefCountServantBase {
public:
  LogCentralComponent_impl(ComponentList* componentList,
                           FilterManagerInterface* filterManager,
                           TimeBuffer* timeBuffer);

  ~LogCentralComponent_impl();

  void
  test();

  /**
   * Connect a component to the LogCentral. The component attaches with its
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
   * Disconnect a connected component from the LogCentral.
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
   * Send a buffer of log_msg_t to the LogCentral.
   * @param buffer sequence of log_msg_t to be sent
   */
  void
  sendBuffer(const log_msg_buf_t& buffer);

  /**
   * Tell if a component exists or not.
   * @param name name of the component to find
   * @return true if the component exists
   */
  bool
  isComponentExists(const char* name, ComponentList::ReadIterator* it);

  /**
   * To be called by a thread for saying that the component is still alive.
   * If the last ping is too old, the component is considered dead and
   * generate a disconnect message (OUT).
   * @param componentName the name of the component who send the ping
   */
  void
  ping(const char* componentName);

  /**
   * To be called by a thread for clock synchronisation.
   * @param componentTime localtime on the component side
   * @param componentName the name of the component who send the synchro
   */
  void
  synchronize(const char* componentName, const log_time_t& componentTime);

  /**
   * Uselessbut to have the same idl as in log
   */
//  char*
//  getHostnameOf(const char* toto);

private:
  /**
   * Generate an unique name based on the hostname
   * @param hostname the hostname to base to form the name
   * @return a unique name
   */
  char*
  getGeneratedName(const char* hostname, ComponentList::ReadIterator* it);

  class AliveCheckThread:public omni_thread {
  public:
    explicit AliveCheckThread(LogCentralComponent_impl* LCC);

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

    LogCentralComponent_impl* LCC;
    bool threadRunning;
  };

  friend class LogCentralComponent_impl::AliveCheckThread;

private:
  ComponentList* componentList;
  FilterManagerInterface* filterManager;
  TimeBuffer* timeBuffer;
  LastPings* lastPings;
  AliveCheckThread* aliveCheckThread;
};



class LogCentralComponentFwdrImpl
  : public POA_LogCentralComponentFwdr,
    public PortableServer::RefCountServantBase {
public:
  LogCentralComponentFwdrImpl(Forwarder_ptr fwdr, const char *objName);

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
  synchronize(const char* componentName, const log_time_t& componentTime);

  void
  test();

protected:
  Forwarder_ptr forwarder;
  char* objName;

private:
  ComponentList* componentList;
  FilterManagerInterface* filterManager;
  TimeBuffer* timeBuffer;
  LastPings* lastPings;
};


class ComponentConfiguratorFwdr_impl
  : public POA_ComponentConfiguratorFwdr,
    public PortableServer::RefCountServantBase {
public:
  ComponentConfiguratorFwdr_impl(Forwarder_ptr fwdr,
                                 const char* objName);

  ~ComponentConfiguratorFwdr_impl();

  void
  setTagFilter(const tag_list_t& tagList);

  void
  addTagFilter(const tag_list_t& tagList);

  void
  removeTagFilter(const tag_list_t& tagList);

  void
  test();

protected:
  Forwarder_ptr forwarder;
  char* objName;
};

#endif
