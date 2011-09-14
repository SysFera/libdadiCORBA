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
 * Revision 1.5  2011/02/04 15:35:00  bdepardo
 * Code indentation.
 * Removed unused variables.
 *
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
 * Revision 1.9  2010/07/27 16:16:48  glemahec
 * Forwarders robustness
 *
 * Revision 1.8  2010/07/27 13:25:01  glemahec
 * Forwarders robustness improvements
 *
 * Revision 1.7  2010/07/27 10:24:31  glemahec
 * Improve robustness & general performance
 *
 * Revision 1.6  2010/07/20 14:31:45  glemahec
 * Forwarder robustness + bug corrections
 *
 * Revision 1.5  2010/07/20 08:56:22  bisnard
 * Updated WfLogService IDL
 *
 * Revision 1.4  2010/07/14 23:45:30  bdepardo
 * Header corrections
 *
 * Revision 1.3  2010/07/14 22:43:26  glemahec
 * Remove WAITPEERINIT macro
 *
 * Revision 1.2  2010/07/13 15:24:13  glemahec
 * Warnings corrections and some robustness improvements
 *
 * Revision 1.1  2010/07/12 16:11:03  glemahec
 * DIET 2.5 beta 1 - New ORB manager; dietForwarder application
 ****************************************************************************/

#include "LogForwarder.hh"

#include "CorbaLogForwarder.hh"
#include "ORBMgr.hh"
#include "common_types.hh"

#include "monitor/LogCentralComponent_impl.hh"
#include "monitor/LogCentralTool_impl.hh"

//#include "debug.hh"

#include <stdexcept>
#include <string>
#include <cstring>
#include <list>
#include <iostream>

using namespace std;

bool LogForwarder::remoteCall(string& objName) {
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


::CORBA::Object_ptr LogForwarder::getObjectCache(const std::string& name)
{
  map<string, ::CORBA::Object_ptr>::iterator it;
	
  cachesMutex.lock();
  if ((it=objectCache.find(name))!=objectCache.end()) {
    cachesMutex.unlock();
    return CORBA::Object::_duplicate(it->second);
  }
  cachesMutex.unlock();
  return ::CORBA::Object::_nil();	
}

LogForwarder::~LogForwarder(){
}

LogForwarder::LogForwarder(const string& name, const string& cfgPath) :
  netCfg(cfgPath)
{
  this->name = name;
  // Wait for the peer init. The unlock will be done on setPeer().
  peerMutex.lock(); 
}


LogCentralComponent_ptr LogForwarder::getLogCentralComponent(const char* name)
{
  string nm(name);
  ::CORBA::Object_ptr object;
	
  if (nm.find('/') == string::npos) {
    nm = string(LOGCOMPCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return LogCentralComponent::_duplicate(LogCentralComponent::_narrow(object));
  }
	
  LogCentralComponentFwdrImpl * comp = 
    new LogCentralComponentFwdrImpl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(comp);
	
  cachesMutex.lock();
  servants[nm] = comp;
  objectCache[name] = CORBA::Object::_duplicate(comp->_this());
  cachesMutex.unlock();

  return LogCentralComponent::_duplicate(comp->_this());
}

LogCentralTool_ptr LogForwarder::getLogCentralTool(const char* name)
{
  string nm(name);
  ::CORBA::Object_ptr object;
  if (nm.find('/') == string::npos) {
    nm = string(LOGTOOLCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return LogCentralTool::_duplicate(LogCentralTool::_narrow(object));
  }
	
  LogCentralToolFwdr_impl * comp = 
    new LogCentralToolFwdr_impl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(comp);
	
  cachesMutex.lock();
  servants[nm] = comp;
  objectCache[name] = CORBA::Object::_duplicate(comp->_this());
  cachesMutex.unlock();
  return LogCentralTool::_duplicate(comp->_this());
}

ComponentConfigurator_ptr LogForwarder::getCompoConf(const char* name)
{
  string nm(name);
  ::CORBA::Object_ptr object;
	
  if (nm.find('/') == string::npos) {
    nm = string(LOGCOMPCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return ComponentConfigurator::_duplicate(ComponentConfigurator::_narrow(object));
  }
	
  ComponentConfiguratorFwdr_impl * comp = 
    new ComponentConfiguratorFwdr_impl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(comp);
	
  cachesMutex.lock();
  servants[nm] = comp;
  objectCache[name] = CORBA::Object::_duplicate(comp->_this());
  cachesMutex.unlock();

  return ComponentConfigurator::_duplicate(comp->_this());
}

ToolMsgReceiver_ptr LogForwarder::getToolMsgReceiver(const char* name)
{
  string nm(name);
  ::CORBA::Object_ptr object;
  if (nm.find('/') == string::npos) {
    nm = string(LOGTOOLCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else
      return ToolMsgReceiver::_duplicate(ToolMsgReceiver::_narrow(object));
  }
	
  ToolMsgReceiverFwdr_impl * comp = 
    new ToolMsgReceiverFwdr_impl(this->_this(), nm.c_str());

  ORBMgr::getMgr()->activate(comp);
	
  cachesMutex.lock();
  servants[nm] = comp;
  objectCache[name] = CORBA::Object::_duplicate(comp->_this());
  cachesMutex.unlock();
  return ToolMsgReceiver::_duplicate(comp->_this());
}




void LogForwarder::bind(const char* objName, const char* ior, const char* connect) {
  /* To avoid crashes when the peer forwarder is not ready: */
  /* If the peer was not initialized, the following call is blocking. */
  peerMutex.lock();
  peerMutex.unlock();
  
  string objString(objName);
  string name;
  string ctxt;
  
  if (!remoteCall(objString)) {
    //    TRACE_TEXT(TRACE_MAIN_STEPS, "Forward bind to peer" << std::endl);
    return getPeer()->bind(objString.c_str(), ior, connect);
  }
  ctxt = getCtxt(objString);
  name = getName(objString);
  //  TRACE_TEXT(TRACE_MAIN_STEPS, "Bind locally" << std::endl);

  ORBMgr::getMgr()->bind(ctxt, name, ior, connect, true);
  // Broadcast the binding to all forwarders.
  ORBMgr::getMgr()->fwdsBind(ctxt, name, ior, connect, this->name);
  //  TRACE_TEXT(TRACE_MAIN_STEPS, "Binded !" << endl);
}

/* Return the local bindings. Result is a set of couple
 * (object name, object ior)
 */
//SeqString* LogForwarder::getBindings(const char* ctxt) {
//  list<string> objects;
//  list<string>::iterator it;
//  SeqString* result = new SeqString();
//  unsigned int cmpt = 0;
//	
//  objects = ORBMgr::getMgr()->list(ctxt);
//  result->length(objects.size()*2);
//  
//  for (it=objects.begin(); it!=objects.end(); ++it) {
//    try {
//      CORBA::Object_ptr obj = ORBMgr::getMgr()->resolveObject(ctxt, it->c_str(),
//                                                                 "no-Forwarder");
//      (*result)[cmpt++]=it->c_str();
//      (*result)[cmpt++]=ORBMgr::getMgr()->getIOR(obj).c_str();
//    } catch (const runtime_error& err) {
//      continue;
//    }
//  }
//  result->length(cmpt);
//  return result;
//}

void LogForwarder::unbind(const char* objName) {
  string objString(objName);
  string name;
  string ctxt;
  
  if (!remoteCall(objString)) {
    return getPeer()->unbind(objString.c_str());
  }
  
  name = objString;
  
  if (name.find('/')==string::npos)
    return;
  
  ctxt = getCtxt(objString);
  name = getName(objString);
  
  removeObjectFromCache(name);
  
  ORBMgr::getMgr()->unbind(ctxt, name);
  // Broadcast the unbinding to all forwarders.
  ORBMgr::getMgr()->fwdsUnbind(ctxt, name, this->name);
}

void LogForwarder::removeObjectFromCache(const string& name) {
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
void LogForwarder::cleanCaches() {
  map<string, ::CORBA::Object_ptr>::iterator it;
  list<string> invalidObjects;
  list<string>::const_iterator jt;
  
  cachesMutex.lock();
  for (it=objectCache.begin(); it!=objectCache.end(); ++it) {
    try {
      CorbaLogForwarder_var object = CorbaLogForwarder::_narrow(it->second);
      object->getName();
    } catch (const CORBA::TRANSIENT& err) {
      invalidObjects.push_back(it->first);
    }
  }
  cachesMutex.unlock();
  for (jt=invalidObjects.begin(); jt!=invalidObjects.end(); ++jt)
    removeObjectFromCache(*jt);
}

void LogForwarder::connectPeer(const char* ior, const char* host,
			       const ::CORBA::Long port)
{
  string converted = ORBMgr::convertIOR(ior, host, port);
  setPeer(ORBMgr::getMgr()->resolve<CorbaLogForwarder, CorbaLogForwarder_ptr>(converted));
}

void LogForwarder::setPeer(CorbaLogForwarder_ptr peer) {
  this->peer = CorbaLogForwarder::_duplicate(CorbaLogForwarder::_narrow(peer));
  // Peer was init. The lock was done on the constructor.
  peerMutex.unlock();
}

CorbaLogForwarder_var LogForwarder::getPeer() {
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


char* LogForwarder::getIOR() {
  return CORBA::string_dup(ORBMgr::getMgr()->getIOR(_this()).c_str());
}

char* LogForwarder::getName() {
  return CORBA::string_dup(name.c_str());
}

//SeqString* LogForwarder::acceptList() {
//  SeqString* result = new SeqString;
//  list<string>::const_iterator it;
//  
//  result->length(netCfg.getAcceptList().size());
//  
//  unsigned int i=0;
//  for (it=netCfg.getAcceptList().begin();
//       it!=netCfg.getAcceptList().end();
//       ++it, ++i)
//    {
//      (*result)[i] = it->c_str();
//    }
//	
//  return result;
//}
//
//SeqString* LogForwarder::rejectList() {
//  SeqString* result = new SeqString;
//  list<string>::const_iterator it;
//  
//  result->length(netCfg.getRejectList().size());
//  
//  unsigned int i=0;
//  for (it=netCfg.getRejectList().begin();
//       it!=netCfg.getRejectList().end();
//       ++it, ++i)
//    {
//      (*result)[i] = it->c_str();
//    }
//  
//  return result;
//}

::CORBA::Boolean LogForwarder::manage(const char* hostname) {
  return netCfg.manage(hostname);
}

//SeqString* LogForwarder::routeTree() {
//  SeqString* result = new SeqString();
//  /*std::list<string> forwarders = ORBMgr::list(FWRDCTXT);
//    std::list<string>::const_iterator it;
//    
//    result->length(1);
//    (*result)[0]=CORBA::string_dup(name.c_str())
//    for (it=forwarders.begin(); it!=forwarders.end(); ++it) {
//    if (*it==name) continue;
//    
//    Forwarder_var fwd = resolve<Forwarder, Forwarder_var>(FWRDCTXT, *it,
//    this->name);
//    SeqString* tmp = fwd->routeTree();
//    unsigned int curLgth = result->length();
//    unsigned int tmpLgth = tmp->lgth();
//    
//    result->length(lgth+tmpLgth);
//    for (unsigned int i=lgth; i<lgth+tmpLgth; ++i) {
//    (*result)[i] = (*tmp)[i-lgth];
//    }
//    
//    }*/
//  return result;
//}

string LogForwarder::getName(const string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos==string::npos) return namectxt;
  return namectxt.substr(pos+1);
}

string LogForwarder::getCtxt(const string& namectxt) {
  size_t pos = namectxt.find('/');
  if (pos==string::npos) return "";
  return namectxt.substr(0, pos);
}



void LogForwarder::setTagFilter(const ::tag_list_t& tagList,
				const char* objName)
{
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->setTagFilter(tagList, objString.c_str());
  }
	
  name = getName(objString);
  
  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator, ComponentConfigurator_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
															  name,
															  this->name);
  return cfg->setTagFilter(tagList);
}

void LogForwarder::addTagFilter(const ::tag_list_t& tagList,
				const char* objName)
{
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->addTagFilter(tagList, objString.c_str());
  }
  
  name = getName(objString);
  
  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator, ComponentConfigurator_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                                   name,
                                                                                   this->name);
  return cfg->addTagFilter(tagList);
  
}

void LogForwarder::removeTagFilter(const ::tag_list_t& tagList,
				   const char* objName)
{
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->removeTagFilter(tagList, objString.c_str());
  }
  
  name = getName(objString);
  
  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator, ComponentConfigurator_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                                   name,
                                                                                   this->name);
  return cfg->removeTagFilter(tagList);
  
}

void LogForwarder::test(const char* objName) {
  string objString(objName);
  string name;
  
  
  name = getName(objString);
  
  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator, ComponentConfigurator_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                                   name,
                                                                                   this->name);
  return cfg->test();
  
}



void
LogForwarder::sendMsg(const log_msg_buf_t& msgBuf, const char*  objName){
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->sendMsg(msgBuf, objString.c_str());
  }
  
  name = getName(objString);
  
  ToolMsgReceiver_var cfg =
    ORBMgr::getMgr()->resolve<ToolMsgReceiver, ToolMsgReceiver_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                       name,
                                                                       this->name);
  return cfg->sendMsg(msgBuf);
}

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
LogForwarder::connectTool(char*& toolName, const char* msgReceiver,  const char* objName){
  string objString(objName);
  string name;
  if (!remoteCall(objString)) {
    getPeer()->connectTool(toolName, msgReceiver, objString.c_str());
    return 1;
  }
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->connectTool(toolName, msgReceiver);

}

/**
 * Disconnects a connected tool from the monitor. No further 
 * filterconfigurations should be sent after this call. The 
 * toolMsgReceiver will not be used by the monitor any more 
 * after this call. Returns NOTCONNECTED if the calling tool
 * was not connected.
 */
short
LogForwarder::disconnectTool(const char* toolName, const char* objName){
  string objString(objName);
  string name;
	
  if (!remoteCall(objString)) {
    return getPeer()->disconnectTool(toolName, objString.c_str());
  }
	
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->disconnectTool(toolName);

}

/**
 * Returns a list of possible tags. This is just a convenience
 * functions and returns the values that are specified in a
 * configuration file. If the file is not up to date, the 
 * application may generate more tags than defined in this
 * list.
 */
tag_list_t*
LogForwarder::getDefinedTags(const char* objName){
  string objString(objName);
  string name;
	
  if (!remoteCall(objString)) {
    return getPeer()->getDefinedTags(objString.c_str());
  }
	
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->getDefinedTags();

}

/**
 * Returns a list of actually connected Components. This is just
 * a convenience function, as the whole state of the system will
 * be sent to the tool right after connection (in the form of
 * messages)
 */  
component_list_t*
LogForwarder::getDefinedComponents(const char*  objName){
  string objString(objName);
  string name;
	
  if (!remoteCall(objString)) {
    return getPeer()->getDefinedComponents(objString.c_str());
  }
	
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->getDefinedComponents();

}

/**
 * Create a filter for this tool on the monitor. Messages matching
 * this filter will be forwarded to the tool. The filter will be
 * identified by its name, which is a part of filter_t. A tool
 * can have as much filters as it wants. Returns ALREADYEXISTS if
 * another filter with this name is already registered.
 */
short
LogForwarder::addFilter(const char* toolName, const filter_t& filter, const char* objName){
  string objString(objName);
  string name;
	
  if (!remoteCall(objString)) {
    return getPeer()->addFilter(toolName, filter, objString.c_str());
  }
	
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->addFilter(toolName, filter);

}

/**
 * Removes a existing filter from the tools filterList. The filter
 * will be identified by its name in the filter_t. If the specified
 * filter does not exist, NOTEXISTS is returned.
 */
short
LogForwarder::removeFilter(const char* toolName, const char* filterName, const char* objName){
  string objString(objName);
  string name;
	
  if (!remoteCall(objString)) {
    return getPeer()->removeFilter(toolName, filterName, objString.c_str());
  }
	
  name = getName(objString);
	
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->removeFilter(toolName, filterName);

}

/**
 * Removes all defined filters.
 */
short
LogForwarder::flushAllFilters(const char* toolName, const char* objName){
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->flushAllFilters(toolName, objString.c_str());
  }
  
  name = getName(objString);
  
  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGTOOLCTXT,
                                                                     name,
                                                                     this->name);
  return cfg->flushAllFilters(toolName);
  
}


short
LogForwarder::connectComponent(char*& componentName,
			       const char* componentHostname,
			       const char* message,
			       const char* compConfigurator,
			       const log_time_t& componentTime,
			       tag_list_t& initialConfig, 
			       const char* objName){
  string objString(objName);
  string name;
  if (!remoteCall(objString)) {
    return getPeer()->connectComponent(componentName,
				       componentHostname,
				       message,
				       compConfigurator,
				       componentTime,
				       initialConfig,
				       objString.c_str());
  }
  name = getName(objString);
  
  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                               name,
                                                                               this->name);
  return cfg->connectComponent(componentName,
			       componentHostname,
			       message,
			       compConfigurator,
			       componentTime,
			       initialConfig);
  
}



short
LogForwarder::disconnectComponent(const char* componentName,
				  const char* message,
				  const char* objName){
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->disconnectComponent(componentName,
					  message,
					  objString.c_str());
  }
  
  name = getName(objString);
  
  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                               name,
                                                                               this->name);
  return cfg->disconnectComponent(componentName,
				  message);
  
}




void
LogForwarder::sendBuffer(const log_msg_buf_t &buffer, 
			 const char* objName){
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->sendBuffer(buffer,
				 objString.c_str());
  }
  
  name = getName(objString);
  
  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                               name,
                                                                               this->name);
  return cfg->sendBuffer(buffer);
  
}


void
LogForwarder::synchronize(const char* componentName,
			  const log_time_t& componentTime,
			  const char* objName){
  string objString(objName);
  string name;
  
  if (!remoteCall(objString)) {
    return getPeer()->synchronize(componentName,
                                  componentTime,
                                  objString.c_str());
  }
  
  name = getName(objString);
  
  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_var, CorbaLogForwarder, CorbaLogForwarder_var>(LOGCOMPCTXT,
                                                                               name,
                                                                               this->name);
  return cfg->synchronize(componentName,
			  componentTime);
  
}
