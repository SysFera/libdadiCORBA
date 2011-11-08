/****************************************************************************/
/* Log forwarder implementation                                             */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)                         */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/

#include "CorbaForwarder.hh"

#include <cstring>
#include <list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>  // For gethostname()

#include "ORBMgr.hh"
#include "common_types.hh"

#include "monitor/LogCentralComponent_impl.hh"
#include "monitor/LogCentralTool_impl.hh"

#include "debug.hh"

#ifdef MAXHOSTNAMELEN
#define MAX_HOSTNAME_LENGTH MAXHOSTNAMELEN
#else
#define MAX_HOSTNAME_LENGTH  255
#endif




LogCentralComponent_ptr
CorbaForwarder::getLogCentralComponent(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(LOGCOMPCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return LogCentralComponent::_duplicate(
        LogCentralComponent::_narrow(object));
    }
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

LogCentralTool_ptr
CorbaForwarder::getLogCentralTool(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;
  if (nm.find('/') == std::string::npos) {
    nm = std::string(LOGTOOLCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return LogCentralTool::_duplicate(LogCentralTool::_narrow(object));
    }
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

ComponentConfigurator_ptr
CorbaForwarder::getCompoConf(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;

  if (nm.find('/') == std::string::npos) {
    nm = std::string(LOGCOMPCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return ComponentConfigurator::_duplicate(
        ComponentConfigurator::_narrow(object));
    }
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

ToolMsgReceiver_ptr
CorbaForwarder::getToolMsgReceiver(const char* name) {
  std::string nm(name);
  ::CORBA::Object_ptr object;
  if (nm.find('/') == std::string::npos) {
    nm = std::string(LOGTOOLCTXT)+"/"+nm;
  }
  object = getObjectCache(nm);
  if (!CORBA::is_nil(object)) {
    if (object->_non_existent()) {
      removeObjectFromCache(name);
    } else {
      return ToolMsgReceiver::_duplicate(ToolMsgReceiver::_narrow(object));
    }
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

void
CorbaForwarder::setTagFilter(const ::tag_list_t& tagList,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->setTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator,
                                 ComponentConfigurator_var>(LOGCOMPCTXT,
                                                            name,
                                                            this->name);
  return cfg->setTagFilter(tagList);
}

void
CorbaForwarder::addTagFilter(const ::tag_list_t& tagList,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->addTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator,
                                 ComponentConfigurator_var>(LOGCOMPCTXT,
                                                            name,
                                                            this->name);
  return cfg->addTagFilter(tagList);
}

void
CorbaForwarder::removeTagFilter(const ::tag_list_t& tagList,
                              const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->removeTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->resolve<ComponentConfigurator,
                                 ComponentConfigurator_var>(LOGCOMPCTXT,
                                                            name,
                                                            this->name);
  return cfg->removeTagFilter(tagList);
}



void
CorbaForwarder::sendMsg(const log_msg_buf_t& msgBuf, const char*  objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->sendMsg(msgBuf, objString.c_str());
  }

  name = getName(objString);

  ToolMsgReceiver_var cfg =
    ORBMgr::getMgr()->resolve<ToolMsgReceiver,
                                 ToolMsgReceiver_var>(LOGTOOLCTXT,
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
CorbaForwarder::connectTool(char*& toolName,
                          const char* msgReceiver,
                          const char* objName) {
  std::string objString(objName);
  std::string name;
  if (!remoteCall(objString)) {
    getPeer()->connectTool(toolName, msgReceiver, objString.c_str());
    return 1;
  }
  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
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
CorbaForwarder::disconnectTool(const char* toolName, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->disconnectTool(toolName, objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
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
CorbaForwarder::getDefinedTags(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getDefinedTags(objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
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
CorbaForwarder::getDefinedComponents(const char*  objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getDefinedComponents(objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGCOMPCTXT,
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
CorbaForwarder::addFilter(const char* toolName,
                        const filter_t& filter,
                        const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->addFilter(toolName, filter, objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
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
CorbaForwarder::removeFilter(const char* toolName,
                           const char* filterName,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->removeFilter(toolName, filterName, objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
                                                     name,
                                                     this->name);
  return cfg->removeFilter(toolName, filterName);
}

/**
 * Removes all defined filters.
 */
short
CorbaForwarder::flushAllFilters(const char* toolName, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->flushAllFilters(toolName, objString.c_str());
  }

  name = getName(objString);

  LogCentralTool_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralTool,
                                 LogCentralTool_var>(LOGTOOLCTXT,
                                                     name,
                                                     this->name);
  return cfg->flushAllFilters(toolName);
}


short
CorbaForwarder::connectComponent(char*& componentName,
                               const char* componentHostname,
                               const char* message,
                               const char* compConfigurator,
                               const log_time_t& componentTime,
                               tag_list_t& initialConfig,
                               const char* objName) {
  std::string objString(objName);
  std::string name;
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
    ORBMgr::getMgr()->resolve<LogCentralComponent,
                                 LogCentralComponent_var>(LOGCOMPCTXT,
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
CorbaForwarder::disconnectComponent(const char* componentName,
                                  const char* message,
                                  const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->disconnectComponent(componentName,
                                          message,
                                          objString.c_str());
  }

  name = getName(objString);

  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent,
                                 LogCentralComponent_var>(LOGCOMPCTXT,
                                                          name,
                                                          this->name);
  return cfg->disconnectComponent(componentName, message);
}




void
CorbaForwarder::sendBuffer(const log_msg_buf_t &buffer,
                         const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->sendBuffer(buffer,
                                 objString.c_str());
  }

  name = getName(objString);

  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent,
                                 LogCentralComponent_var>(LOGCOMPCTXT,
                                                          name,
                                                          this->name);
  return cfg->sendBuffer(buffer);
}


void
CorbaForwarder::synchronize(const char* componentName,
                          const log_time_t& componentTime,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->synchronize(componentName,
                                  componentTime,
                                  objString.c_str());
  }

  name = getName(objString);

  LogCentralComponent_var cfg =
    ORBMgr::getMgr()->resolve<LogCentralComponent,
                                 LogCentralComponent_var>(LOGCOMPCTXT,
                                                          name,
                                                          this->name);
  return cfg->synchronize(componentName, componentTime);
}

