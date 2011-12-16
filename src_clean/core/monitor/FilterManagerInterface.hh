/**
 * @file ComponentList.hh
 *
 * @briefdefines interface that defines functions that an implementation of a
 * FilterManager must offer to be notified correctly on systemchanges
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _FILTERMANAGERINTERFACE_HH_
#define _FILTERMANAGERINTERFACE_HH_

#include "LogTypes.hh"
#include "ToolList.hh"
#include "ComponentList.hh"

/**
 * @brief Interface to the Filtermanager (Abstract class)
 * The FilterManager keeps track of all attached Components
 * and of all configured filters. Based on this information,
 * it can be used to filter incoming Messages and forward
 * them to the appropriate tools. If tools change their
 * filters, the FilterManager adjusts the Components output.
 * See the LogComponent.idl file for more data about the methods
 * @class FilterManagerInterface
 */
class FilterManagerInterface {

public:
  /**
   * @brief Notify the manager that a new tool has connected.
   * The Tool must already exist in the toolList. iter must
   * be an ReadIterator to the toolList. It should be the
   * same iterator that was used for inserting the tool.
   */
  virtual void
  toolConnect(const char* toolName, ToolList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that a tool is about disconnects.
   * This function must be called right before deleting the
   * tool from the list, as Filtermanager will delete all its
   * internal data on this tool (including remaining filters).
   * The iterator passed should be the iterator used for
   * deleting the tool.
   */
  virtual void
  toolDisconnect(const char* toolName, ToolList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that a new filter has been added.
   * the filter must already exist in the toollist. A
   * iterator used for inserting should be passed.
   */
  virtual void
  addFilter(const char* toolName, const char* filterName,
            ToolList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that a filter will be removed.
   * This function should be called right before deleting
   * the filter. The iterator for deleting the filter should
   * be passed.
   */
  virtual void
  removeFilter(const char* toolName, const char* filterName,
               ToolList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that all filters for a certain
   * tool will be removed. The filtermanager might implement
   * this more efficient that removing each filter one by
   * one.
   */
  virtual void
  flushAllFilters(const char* toolName, ToolList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that a component connected.
   * The component should be already in the list, the
   * corresponding iterator should be passed.
   */
  virtual tag_list_t*
  componentConnect(const char* componentName,
                   ComponentList::ReadIterator* iter) = 0;

  /**
   * @brief Notify the manager that a component will disconnects.
   * This must happen right before the deletion of the
   * component. The iterator passed should be the same
   * iterator used for deleting.
   */
  virtual void
  componentDisconnect(const char* componentName,
                      ComponentList::ReadIterator* iter) = 0;

  /**
   * @brief Forwards a message to all interested Tools.
   * The message will be checked and stored in the outBuffer
   * of all tools with a matching filter.
   */
  virtual void
  sendMessageWithFilters(log_msg_t* message) = 0;
};

#endif
