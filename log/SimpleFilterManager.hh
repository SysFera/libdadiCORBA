/**
 * @brief The SimpleFilterManager implements the FilterManagerInterface. This
 * Implementation is complete, but it is not very efficient.
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _SIMPLEFILTERMANAGER_HH_
#define _SIMPLEFILTERMANAGER_HH_

#include "LogTypes.hh"
#include "ToolList.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "utils/FullLinkedList.hh"

/**
 * @brief Define an internal datastructure
 * @struct ConfigElement
 */
struct ConfigElement {
/**
 * @brief Thename of the component
 */
  CORBA::String_var componentName;
/**
 * @brief The tag of configuration
 */
  tag_list_t config;
/**
 * @brief The old tag of configuration
 */
  tag_list_t oldConfig;
};

/**
 * @brief A list to hold the configuration of components
 */
typedef FullLinkedList<ConfigElement> ConfigList;


/**
 * @brief Simple implementation of the FilterManagerInterface
 * Mainly offers empty functions to make testers compile/run
 * @class SimpleFilterManager
 */
class SimpleFilterManager:public FilterManagerInterface {
public:
  /**
   * @brief Create a SimpleFilterManager.
   *
   * @param toolList The ToolList to use
   * @param compList The ComponentList to use
   * @param stateTags A list of all Tags that are important
   *   for the system state and must always be sent. The
   *   FilterManager copies this list and does not take its
   *   ownership.
   */
  SimpleFilterManager(ToolList* toolList,
                      ComponentList* compList,
                      tag_list_t* stateTags);

/**
 * @brief Destructor
 */
  virtual
  ~SimpleFilterManager();

  /**
   * @brief Notify the manager that a new tool has connected.
   * The Tool must already exist in the toolList. iter must
   * be an ReadIterator to the toolList. It should be the
   * same iterator that was used for inserting the tool.
   * @param toolName The name of the tool
   * @param iter The iterator over a tool list
   */
  void
  toolConnect(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * @brief Notify the manager that a tool is about to disconnect.
   * This function must be called right before deleting the
   * tool from the list, as Filtermanager will delete all its
   * internal data on this tool (including remaining filters).
   * The iterator passed should be the iterator used for
   * deleting the tool.
   * @param toolName The name of the tool
   * @param iter The iterator over a tool list
   */
  void
  toolDisconnect(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * @brief Notify the manager that a new filter has been added.
   * the filter must already exist in the toollist. A
   * iterator used for inserting should be passed.
   * @param toolName The name of the tool
   * @filterName The name of the filter
   * @param iter The iterator over a tool list
   */
  void
  addFilter(const char* toolName, const char* filterName,
            ToolList::ReadIterator* iter);

  /**
   * @brief Notify the manager that a filter will be removed.
   * This function should be called right before deleting
   * the filter. The iterator for deleting the filter should
   * be passed.
   * @param toolName The name of the tool
   * @filterName The name of the filter
   * @param iter The iterator over a tool list
   */
  void
  removeFilter(const char* toolName, const char* filterName,
               ToolList::ReadIterator* iter);

  /**
   * @brief Notify the manager that all filters for a certain
   * tool will be removed. The filtermanager might implement
   * this more efficient that removing each filter one by
   * one.
   * @param toolName The name of the tool
   * @param iter The iterator over a tool list
   */
  virtual void
  flushAllFilters(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * @brief Notify the manager that a component has connected
   * The component should be already in the list, the
   * corresponding iterator should be passed.
   * @param componentName The name of the tool
   * @param iter The iterator over a component list
   * @return TODO
   */
  tag_list_t*
  componentConnect(const char* componentName,
                   ComponentList::ReadIterator* iter);

  /**
   * @brief Notify the manager that a component will disconnect.
   * This must happen right before the deletion of the
   * component. The iterator passed should be the same
   * iterator used for deleting.
   * @param componentName The name of the tool
   * @param iter The iterator over a component list
   */
  void
  componentDisconnect(const char* componentName,
                      ComponentList::ReadIterator* iter);

  /**
   * @brief Forwards a message to all interested Tools.
   * The message will be checked and stored in the outBuffer
   * of all tools with a matching filter.
   * @param message The message to send
   */
  void
  sendMessageWithFilters(log_msg_t* message);

private:
/**
 * @brief The list of tools
 */
  ToolList* mtoolList;
/**
 * @brief The list of components
 */
  ComponentList* mcomponentList;

  /**
   * @brief hold current configuration for each component
   */
  ConfigList mconfigList;

  /**
   * @brief taglist that contains stateChanging tags that must always be sent
   */
  tag_list_t msystemStateTags;

  /**
   * @brief Checks if a given component_list_t contains the
   * value given in name. list may contain the star
   * @param list A list of components
   * @param name The name to test
   * @return True if contain, false otherwise
   */
  bool
  containsComponent(component_list_t* list, const char* name);

  /**
   * @brief Checks if a given component_list_t contains the
   * value given in componentName. Stars ('*') are not treated
   * as special characters
   * @param list A list of components
   * @param name The name to test
   * @return True if contain, false otherwise
   */
  bool
  containsComponentName(component_list_t* list, const char* name);

  /**
   * @brief Checks if a given component_list_t contains the '*'
   * @param list A list of components
   * @return True if contain, false otherwise
   */
  bool
  containsComponentStar(component_list_t* list);

  /**
   * @brief Checks if a given tag_list_t contains the
   * value given in name. list may contain the star
   * @param list A list of tags
   * @param name The name to test
   * @return True if contain, false otherwise
   */
  bool
  containsTag(tag_list_t* list, const char* name);

  /**
   * @brief Checks if a given tag_list_t contains the
   * value given in name. Stars ('*') are not treated
   * as special characters
   * @param list A list of tags
   * @param name The name to test
   * @return True if contain, false otherwise
   */
  bool
  containsTagName(tag_list_t* list, const char* name);

  /**
   * @brief Checks if a given component_list_t contains the '*'
   * @param list A list of tags
   * @return True if contain, false otherwise
   */
  bool
  containsTagStar(tag_list_t* list);

  /**
   * @brief Adds the tags in the srcList to the destList. If the tags
   * already exist in the destList, they are not copied. The
   * destList will not contain duplicates
   * @param destList The destination list
   * @param srcList The srcList
   */
  void
  addTagList(tag_list_t* destList, tag_list_t* srcList);

  /**
   * @brief Add a filter to the system. The tags of this filter will
   * be added to all applying components.
   * @param filter The filter to add
   */
  void
  addFilter(filter_t* filter);

  /**
   * @brief Forward the configuration of the components (as stored in the
   * configList to the ComponentConfigurators.
   */
  void
  updateComponentConfigs();
};

#endif
