/****************************************************************************/
/* Defines an object implementing the interface LogCentralTool defined in   */
/* the LogCentralTool.idl                                                   */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
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

#ifndef _LOGCENTRALTOOL_IMPL_HH_
#define _LOGCENTRALTOOL_IMPL_HH_

#include "ToolList.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "StateManager.hh"

#include "CorbaLogForwarder.hh"

/**
 * Errorlevel constants for connectComponent (defined in the idl)
const short LS_OK
const short LS_TOOL_CONNECT_ALREADYEXISTS
const short LS_TOOL_DISCONNECT_NOTEXISTS
const short LS_TOOL_ADDFILTER_ALREADYEXISTS
const short LS_TOOL_REMOVEFILTER_NOTEXISTS
 */

/**
 * Implements the tool servant for the logCentral.
 * Attaches and detaches tools and forwards their
 * filter-configurations to the FilterManager
 */
class LogCentralTool_impl: public POA_LogCentralTool,
                           public PortableServer::RefCountServantBase
{
public:

  LogCentralTool_impl(ToolList* toolList,
                      ComponentList* compList,
                      FilterManagerInterface* filterMan,
                      StateManager* stateMan,
                      tag_list_t* allTags);

  ~LogCentralTool_impl();

  void
  test();

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
  connectTool(char*& toolName,
              const char* msgReceiver);

  /**
   * Disconnect a tool from the monitor.
   * Remove the tools filters and deregister tool.
   *
   * @param toolName the unique name of the tool
   * @returns if the deconnection was successful
   */
  CORBA::Short
  disconnectTool(const char* toolName);

  /**
   * Returns a list of configured tags. This is just a
   * convenience function. It relies on the configuration of
   * the monitor and must not reflect the real system.
   */
  tag_list_t*
  getDefinedTags();

  /**
   * Returns a list of currently attached components. This is
   * just a convenience function, as all the whole systemstate
   * including all components are sent to the tool upon connection
   * in form of messages.
   */
  component_list_t*
  getDefinedComponents();

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
  addFilter(const char* toolName, const filter_t& filter);

  /**
   * Remove a existing filter from the list.
   *
   * @param toolName the name of the tool who added the filter
   * @param filterName the name of the filter
   * @returns if the filter could be removed properly
   */
  CORBA::Short
  removeFilter(const char* toolName, const char* filterName);

  /**
   * Remove all existing filters of the tool.
   *
   * @param toolName the name of the tool whose filterlist will be cleared
   */ 
  CORBA::Short
  flushAllFilters(const char* toolName);


private:

  FilterManagerInterface* filterManager;
  StateManager* stateManager;
  ToolList* toolList;
  ComponentList* componentList;
  tag_list_t allTags;

  /**
   * sets the currentElement() of the ReadIterator to the
   * toolElement named "toolName", to NULL if no tool with
   * the given name exists. Returns true if the tool was
   * found.
   *
   * @param toolName the name of the tool to look for
   * @param it the iterator that will be changed
   * @returns bool indicating if the tool with the given
   * name was found
   */
  bool getToolByName(const char* toolName, ToolList::ReadIterator* it);

  /**
   * sets the currentElement() of the ReadIterator to the
   * filter_t named "filterName", to NULL if no filter with
   * the given name exists. Returns true if the filter was
   * found.
   *
   * @param filterName the name of the filter to look for
   * @param it the iterator that will be changed
   * @returns bool indicating if the filter with the given
   * name was found
   */
  bool getFilterByName(const char* filterName, FilterList::ReadIterator* it);
  
};


class LogCentralToolFwdr_impl: public POA_LogCentralToolFwdr,
			       public PortableServer::RefCountServantBase
{
public:
  
  LogCentralToolFwdr_impl(CorbaLogForwarder_ptr fwdr,
			  const char* objName);
  
  ~LogCentralToolFwdr_impl();
  
  void
  test();
  
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
  connectTool(char*& toolName,
              const char* msgReceiver);

  /**
   * Disconnect a tool from the monitor.
   * Remove the tools filters and deregister tool.
   *
   * @param toolName the unique name of the tool
   * @returns if the deconnection was successful
   */
  CORBA::Short
  disconnectTool(const char* toolName);

  /**
   * Returns a list of configured tags. This is just a
   * convenience function. It relies on the configuration of
   * the monitor and must not reflect the real system.
   */
  tag_list_t*
  getDefinedTags();

  /**
   * Returns a list of currently attached components. This is
   * just a convenience function, as all the whole systemstate
   * including all components are sent to the tool upon connection
   * in form of messages.
   */
  component_list_t*
  getDefinedComponents();

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
  addFilter(const char* toolName, const filter_t& filter);

  /**
   * Remove a existing filter from the list.
   *
   * @param toolName the name of the tool who added the filter
   * @param filterName the name of the filter
   * @returns if the filter could be removed properly
   */
  CORBA::Short
  removeFilter(const char* toolName, const char* filterName);

  /**
   * Remove all existing filters of the tool.
   *
   * @param toolName the name of the tool whose filterlist will be cleared
   */ 
  CORBA::Short
  flushAllFilters(const char* toolName);
  
protected :
  CorbaLogForwarder_ptr forwarder;
  char* objName;
  
};






class ToolMsgReceiverFwdr_impl: public POA_ToolMsgReceiverFwdr,
				public PortableServer::RefCountServantBase
{
public:
  
  ToolMsgReceiverFwdr_impl(CorbaLogForwarder_ptr fwdr,
			  const char* objName);
  
  ~ToolMsgReceiverFwdr_impl();
  
  void sendMsg (const log_msg_buf_t& msgBuf);
  
protected :
  CorbaLogForwarder_ptr forwarder;
  char* objName;
  
};

#endif
