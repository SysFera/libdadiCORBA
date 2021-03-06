/**
 * @file LogCentralTool_impl.hh
 *
 * @brief Defines an object implementing the interface LogCentralTool defined in
 * the LogCentralTool.idl
 *
 * @author
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _LOGCENTRALTOOL_IMPL_HH_
#define _LOGCENTRALTOOL_IMPL_HH_

#include "ToolList.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "StateManager.hh"

#include "CorbaForwarder.hh"

/**
 * Errorlevel constants for connectComponent (defined in the idl)
const short LS_OK
const short LS_TOOL_CONNECT_ALREADYEXISTS
const short LS_TOOL_DISCONNECT_NOTEXISTS
const short LS_TOOL_ADDFILTER_ALREADYEXISTS
const short LS_TOOL_REMOVEFILTER_NOTEXISTS
 */

/**
 * @brief Implements the tool servant for the logCentral.
 * Attaches and detaches tools and forwards their
 * filter-configurations to the FilterManager
 * See the IDL documentation file
 * @class LogCentralTool_impl
 */
class LogCentralTool_impl: public POA_LogCentralTool,
                           public PortableServer::RefCountServantBase
{
public:
/**
 * @brief Constructor
 * @param toolList A list of tools
 * @param compList A list of components
 * @param filterMan A filter manager
 * @param stateMan A state manager
 * @param allTags A list of tags
 */
  LogCentralTool_impl(ToolList* toolList,
                      ComponentList* compList,
                      FilterManagerInterface* filterMan,
                      StateManager* stateMan,
                      tag_list_t* allTags);

/**
 * @brief Destructor
 */
  ~LogCentralTool_impl();

/**
 * @brief Dummy function
 */
  void
  test();

  /**
   * @brief Connects a new Tool to the monitor.
   * Registers the tool internally with its unique toolName
   * and creates all necessary outbuffers, ...
   * Provides a unique name for the tool if the given toolName
   * is the empty string.
   * (Attention: the omniidl is buggy for string INOUT. The
   * String_INOUT_arg of the LogTool.hh must be replaced by
   * char*&)
   * @param toolName the unique name of the tool
   * @param msgReceiver messageConsumer of the tool, which processes
   * incoming messages for the tool
   * @returns if the connection was successful
   */
  CORBA::Short
  connectTool(char*& toolName,
              const char* msgReceiver);

  /**
   * @brief Disconnect a tool from the monitor.
   * Remove the tools filters and deregister tool.
   *
   * @param toolName the unique name of the tool
   * @returns if the deconnection was successful
   */
  CORBA::Short
  disconnectTool(const char* toolName);

  /**
   * @brief This is just a
   * convenience function. It relies on the configuration of
   * the monitor and must not reflect the real system.
   * @return a list of configured tags.
   */
  tag_list_t*
  getDefinedTags();

  /**
   * @brief This is
   * just a convenience function, as all the whole systemstate
   * including all components are sent to the tool upon connection
   * in form of messages.
   * @return a list of currently attached components.
   */
  component_list_t*
  getDefinedComponents();

  /**
   * @brief Add a (posivive) filter for this tool.
   * Messages matching this filter will be forwarded to the tool.
   *
   * @param toolName the name of the tool, which adds the filter
   * @param filter the filterconfiguration containing the filtername,
   * a list of tags and a list of components
   * @return if the filter could be added properly
   */
  CORBA::Short
  addFilter(const char* toolName, const filter_t& filter);

  /**
   * Remove a existing filter from the list.
   *
   * @param toolName the name of the tool who added the filter
   * @param filterName the name of the filter
   * @return if the filter could be removed properly
   */
  CORBA::Short
  removeFilter(const char* toolName, const char* filterName);

  /**
   * @brief Remove all existing filters of the tool.
   *
   * @param toolName the name of the tool whose filterlist will be cleared
   * @return TODO
   */
  CORBA::Short
  flushAllFilters(const char* toolName);


private:
/**
 * @brief A filter manager
 */
  FilterManagerInterface* filterManager;
/**
 * @brief A state manager
 */
  StateManager* stateManager;
/**
 * @brief A tool list
 */
  ToolList* toolList;
/**
 * @brief A component list
 */
  ComponentList* componentList;
/**
 * @brief A list of tags
 */
  tag_list_t allTags;

  /**
   * @brief sets the currentElement() of the ReadIterator to the
   * toolElement named "toolName", to NULL if no tool with
   * the given name exists. Returns true if the tool was
   * found.
   *
   * @param toolName the name of the tool to look for
   * @param it the iterator that will be changed
   * @return bool indicating if the tool with the given
   * name was found
   */
  bool getToolByName(const char* toolName, ToolList::ReadIterator* it);

  /**
   * @brief sets the currentElement() of the ReadIterator to the
   * filter_t named "filterName", to NULL if no filter with
   * the given name exists. Returns true if the filter was
   * found.
   *
   * @param filterName the name of the filter to look for
   * @param it the iterator that will be changed
   * @return bool indicating if the filter with the given
   * name was found
   */
  bool getFilterByName(const char* filterName, FilterList::ReadIterator* it);

};


#endif
