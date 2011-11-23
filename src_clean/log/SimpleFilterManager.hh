/****************************************************************************/
/* The SimpleFilterManager implements the FilterManagerInterface. This      */
/* Implementation is complete, but it is not very efficient.                */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#ifndef _SIMPLEFILTERMANAGER_HH_
#define _SIMPLEFILTERMANAGER_HH_

#include "LogTypes.hh"
#include "ToolList.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "utils/FullLinkedList.hh"

/**
 * Define an internal datastructure
 */
struct ConfigElement {
  CORBA::String_var componentName;
  tag_list_t config;
  tag_list_t oldConfig;
};

/**
 * A list to hold the configuration of components
 */
typedef FullLinkedList<ConfigElement> ConfigList;


/**
 * Simple implementation of the FilterManagerInterface
 * Mainly offers empty functions to make testers compile/run
 */
class SimpleFilterManager:public FilterManagerInterface {
public:
  /**
   * Create a SimpleFilterManager.
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

  virtual
  ~SimpleFilterManager();

  /**
   * Notify the manager that a new tool has connected.
   * The Tool must already exist in the toolList. iter must
   * be an ReadIterator to the toolList. It should be the
   * same iterator that was used for inserting the tool.
   */
  void
  toolConnect(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * Notify the manager that a tool is about to disconnect.
   * This function must be called right before deleting the
   * tool from the list, as Filtermanager will delete all its
   * internal data on this tool (including remaining filters).
   * The iterator passed should be the iterator used for
   * deleting the tool.
   */
  void
  toolDisconnect(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * Notify the manager that a new filter has been added.
   * the filter must already exist in the toollist. A
   * iterator used for inserting should be passed.
   */
  void
  addFilter(const char* toolName, const char* filterName,
            ToolList::ReadIterator* iter);

  /**
   * Notify the manager that a filter will be removed.
   * This function should be called right before deleting
   * the filter. The iterator for deleting the filter should
   * be passed.
   */
  void
  removeFilter(const char* toolName, const char* filterName,
               ToolList::ReadIterator* iter);

  /**
   * Notify the manager that all filters for a certain
   * tool will be removed. The filtermanager might implement
   * this more efficient that removing each filter one by
   * one.
   */
  virtual void
  flushAllFilters(const char* toolName, ToolList::ReadIterator* iter);

  /**
   * Notify the manager that a component has connected
   * The component should be already in the list, the
   * corresponding iterator should be passed.
   */
  tag_list_t*
  componentConnect(const char* componentName,
                   ComponentList::ReadIterator* iter);

  /**
   * Notify the manager that a component will disconnect.
   * This must happen right before the deletion of the
   * component. The iterator passed should be the same
   * iterator used for deleting.
   */
  void
  componentDisconnect(const char* componentName,
                      ComponentList::ReadIterator* iter);

  /**
   * Forwards a message to all interested Tools.
   * The message will be checked and stored in the outBuffer
   * of all tools with a matching filter.
   */
  void
  sendMessageWithFilters(log_msg_t* message);

private:
  ToolList* toolList;
  ComponentList* componentList;

  /**
   * hold current configuration for each component
   */
  ConfigList configList;

  /**
   * taglist that contains stateChanging tags that must always be sent
   */
  tag_list_t systemStateTags;

  /**
   * Checks if a given component_list_t contains the
   * value given in name. list may contain the star
   */
  bool
  containsComponent(component_list_t* list, const char* name);

  /**
   * Checks if a given component_list_t contains the
   * value given in componentName. Stars ('*') are not treated
   * as special characters
   */
  bool
  containsComponentName(component_list_t* list, const char* name);

  /**
   * Checks if a given component_list_t contains the '*'
   */
  bool
  containsComponentStar(component_list_t* list);

  /**
   * Checks if a given tag_list_t contains the
   * value given in name. list may contain the star
   */
  bool
  containsTag(tag_list_t* list, const char* name);

  /**
   * Checks if a given tag_list_t contains the
   * value given in name. Stars ('*') are not treated
   * as special characters
   */
  bool
  containsTagName(tag_list_t* list, const char* name);

  /**
   * Checks if a given component_list_t contains the '*'
   */
  bool
  containsTagStar(tag_list_t* list);

  /**
   * Adds the tags in the srcList to the destList. If the tags
   * already exist in the destList, they are not copied. The
   * destList will not contain duplicates
   */
  void
  addTagList(tag_list_t* destList, tag_list_t* srcList);

  /**
   * Add a filter to the system. The tags of this filter will
   * be added to all applying components.
   */
  void
  addFilter(filter_t* filter);

  /**
   * Forward the configuration of the components (as stored in the
   * configList to the ComponentConfigurators.
   */
  void
  updateComponentConfigs();
};

#endif
