/**
 * @file LogCentralTool_impl.cc
 *
 * @brief Defines an object implementing the interface LogCentralTool defined in
 * the LogCentralTool.idl
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "LogCentralToolFwdr_impl.hh"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include "ORBMgr.hh"

using namespace std;


//////////////////////////////////// FWDR




LogCentralToolFwdr_impl::LogCentralToolFwdr_impl(Forwarder_ptr fwdr,
						 const char* objName){
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

LogCentralToolFwdr_impl::~LogCentralToolFwdr_impl() {
}

void
LogCentralToolFwdr_impl::test(){
}

  /**
   * Connects a new Tool to the monitor.
   * Registers the tool internally with its unique toolName
   * and creates all necessary outbuffers, ...
   * Provides a unique name for the tool if the given toolName
   * is the empty string.
   * (Attention: the omniidl is buggy for string INOUT. The
   * String_INOUT_arg of the LogTool.hh must be replaced by
   * char*&)
   *
   * @param toolName the unique name of the tool
   * @param msgReceiver messageConsumer of the tool, which processes
   * incoming messages for the tool
   * @returns if the connection was successful
   */
CORBA::Short
LogCentralToolFwdr_impl::connectTool(char*& toolName,
				     const char* msgReceiver){
  return forwarder->connectTool (toolName, msgReceiver, objName);
}

  /**
   * Disconnect a tool from the monitor.
   * Remove the tools filters and deregister tool.
   *
   * @param toolName the unique name of the tool
   * @returns if the deconnection was successful
   */
CORBA::Short
LogCentralToolFwdr_impl::disconnectTool(const char* toolName){
  return forwarder->disconnectTool(toolName, objName);
}

  /**
   * Returns a list of configured tags. This is just a
   * convenience function. It relies on the configuration of
   * the monitor and must not reflect the real system.
   */
tag_list_t*
LogCentralToolFwdr_impl::getDefinedTags(){
  return forwarder->getDefinedTags(objName);
}

  /**
   * Returns a list of currently attached components. This is
   * just a convenience function, as all the whole systemstate
   * including all components are sent to the tool upon connection
   * in form of messages.
   */
component_list_t*
LogCentralToolFwdr_impl::getDefinedComponents(){
  return forwarder->getDefinedComponents(objName);
}

  /**
   * Add a (posivive) filter for this tool.
   * Messages matching this filter will be forwarded to the tool.
   *
   * @param toolName the name of the tool, which adds the filter
   * @param filter the filterconfiguration containing the filtername,
   * a list of tags and a list of components
   * @returns if the filter could be added properly
   */
CORBA::Short
LogCentralToolFwdr_impl::addFilter(const char* toolName, const filter_t& filter){
  return forwarder->addFilter(toolName, filter, objName);
}

  /**
   * Remove a existing filter from the list.
   *
   * @param toolName the name of the tool who added the filter
   * @param filterName the name of the filter
   * @returns if the filter could be removed properly
   */
CORBA::Short
LogCentralToolFwdr_impl::removeFilter(const char* toolName, const char* filterName){
  return forwarder->removeFilter(toolName, filterName, objName);
}

  /**
   * Remove all existing filters of the tool.
   *
   * @param toolName the name of the tool whose filterlist will be cleared
   */
CORBA::Short
LogCentralToolFwdr_impl::flushAllFilters(const char* toolName){
  return forwarder->flushAllFilters(toolName, objName);
}


ToolMsgReceiverFwdr_impl::ToolMsgReceiverFwdr_impl(Forwarder_ptr fwdr,
			  const char* objName){
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}


ToolMsgReceiverFwdr_impl::~ToolMsgReceiverFwdr_impl(){

}


void
ToolMsgReceiverFwdr_impl::sendMsg(const log_msg_buf_t& msgBuf){
  return mforwarder->sendMsg(msgBuf, mobjName);
}
