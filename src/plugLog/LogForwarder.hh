/****************************************************************************/
/* Log forwarder implementation                                             */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)                         */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.4  2010/12/17 15:18:20  kcoulomb
 * update log
 *
 * Revision 1.3  2010/12/13 12:21:13  kcoulomb
 * Clean types
 *
 * Revision 1.2  2010/12/03 12:40:25  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.1  2010/11/10 02:27:43  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.8  2010/08/04 09:06:18  glemahec
 * Parallel compilation
 *
 * Revision 1.7  2010/07/27 16:16:48  glemahec
 * Forwarders robustness
 *
 * Revision 1.6  2010/07/27 13:25:01  glemahec
 * Forwarders robustness improvements
 *
 * Revision 1.5  2010/07/27 10:24:31  glemahec
 * Improve robustness & general performance
 *
 * Revision 1.4  2010/07/20 08:56:22  bisnard
 * Updated WfLogService IDL
 *
 * Revision 1.3  2010/07/14 23:45:30  bdepardo
 * Header corrections
 *
 * Revision 1.2  2010/07/13 15:24:13  glemahec
 * Warnings corrections and some robustness improvements
 *
 * Revision 1.1  2010/07/12 16:11:03  glemahec
 * DIET 2.5 beta 1 - New ORB manager; dietForwarder application
 ****************************************************************************/

#ifndef LOGFORWARDER_HH
#define LOGFORWARDER_HH 

#include <string>
#include <map>

#include <omnithread.h>

#include "NetConfig.hh"

#include "CorbaLogForwarder.hh"
#include "common_types.hh"

#define LOGCOMPCTXT "LogServiceC" //"LogService component"
#define LOGTOOLCTXT "LogServiceT" //"LogService tool"
#define LOGCOMPCONFCTXT "LogServiceC2" //"LogService component"
#define LOGTOOLMSGCTXT "LogServiceT2" //"LogService tool"

class LogForwarder : public POA_CorbaLogForwarder,
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
  
  ::CORBA::Object_ptr getObjectCache(const std::string& name);
  /* The forwarder associated to this one. */
  CorbaLogForwarder_var peer;
  /* Mutexes */
  omni_mutex peerMutex;   // To wait for the peer initialization
  omni_mutex cachesMutex; // Protect access to caches
  
  std::string peerName;
  std::string name;
  /* To determine if the call is from another forwarder and
   * modify the object name.
   */
  static bool remoteCall(std::string& objName);
  /* Network configuration for this forwarder. */
  NetConfig netCfg;
public:
  ~LogForwarder();
  LogForwarder(const std::string& name, const std::string& cfgPath);
  /* DIET object factory methods. */
  ComponentConfigurator_ptr getCompoConf(const char* name);
  ToolMsgReceiver_ptr getToolMsgReceiver(const char* name);
  LogCentralComponent_ptr getLogCentralComponent(const char* name);
  LogCentralTool_ptr getLogCentralTool(const char* name);
  
  /* Common methods implementations. */
  void ping(const char* compoName, const char* objName);

  /* CORBA remote management implementation. */
  void bind(const char* objName, const char* ior);
  void unbind(const char* objName);
  //  SeqString* getBindings(const char* ctxt);
  
  /* Connect the peer forwarder. */
  void connectPeer(const char* ior, const char* host,
		   const ::CORBA::Long port);
  /* Set this forwarder peer object (not CORBA). */
  void setPeer(CorbaLogForwarder_ptr peer);
  CorbaLogForwarder_var getPeer();
  char* getIOR();
  
  /* Object caches management functions. */
  void removeObjectFromCache(const std::string& name);
  void cleanCaches();
  
  char* getName();
  //  SeqString* acceptList();
  //  SeqString* rejectList();
  ::CORBA::Boolean manage(const char* hostname);
  
  //  SeqString* routeTree();
  
  /* LogComponentFwdr implementation. */
  void setTagFilter(const ::tag_list_t& tagList,
		    const char* objName);
  void addTagFilter(const ::tag_list_t& tagList,
		    const char* objName);
  void removeTagFilter(const ::tag_list_t& tagList,
		       const char* objName);
  
  void test(const char* objName);
  
  /**
   * Disconnects a connected tool from the monitor. No further 
   * filterconfigurations should be sent after this call. The 
   * toolMsgReceiver will not be used by the monitor any more 
   * after this call. Returns NOTCONNECTED if the calling tool
   * was not connected.
   */
  short
  disconnectTool(const char* toolName, const char* objName);

  /**
   * Returns a list of possible tags. This is just a convenience
   * functions and returns the values that are specified in a
   * configuration file. If the file is not up to date, the 
   * application may generate more tags than defined in this
   * list.
   */
  tag_list_t*
  getDefinedTags(const char* objName);

  /**
   * Returns a list of actually connected Components. This is just
   * a convenience function, as the whole state of the system will
   * be sent to the tool right after connection (in the form of
   * messages)
   */  
  component_list_t*
  getDefinedComponents(const char*  objName);


  /**
   * Create a filter for this tool on the monitor. Messages matching
   * this filter will be forwarded to the tool. The filter will be
   * identified by its name, which is a part of filter_t. A tool
   * can have as much filters as it wants. Returns ALREADYEXISTS if
   * another filter with this name is already registered.
   */
  short
  addFilter(const char* toolName, const filter_t& filter, const char* objName);


  void
  sendMsg(const log_msg_buf_t& msgBuf, const char*  objName);

  /**
   * Connect a Tool with its toolName, which must be unique among all
   * tools. The return value indicates the success of the connection.
   * If ALREADYEXISTS is returned, the tool could not be attached, as
   * the specified toolName already exists. In this case the tool must
   * reconnect with another name before specifying any filters. If the 
   * tool sends an empty toolName, the LogCentral will provide a unique
   * toolName and pass it back to the tool.
   */
  short
  connectTool(char*& toolName, const char* msgReceiver,  const char* objName);

  short
  flushAllFilters(const char* toolName, const char* objName);
    
  short
  removeFilter(const char* toolName, const char* filterName, const char* objName);



  /* Utility fonctions to extract name & context from context/name. */
  static std::string getName(const std::string& namectxt);
  static std::string getCtxt(const std::string& namectxt);

  short connectComponent(char*&, const char*, const char*, const char*,
			 const log_time_t&, tag_list_t&, const char*);


  short
  disconnectComponent(const char* componentName,
		      const char* message,
		      const char* objName);  


  void
  sendBuffer(const log_msg_buf_t &buffer, 
	     const char* objName);


  void
  synchronize(const char* componentName,
	      const log_time_t& componentTime,
	      const char* objName);


};

#endif
