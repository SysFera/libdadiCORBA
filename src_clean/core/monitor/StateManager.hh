/**
 * @file StateManager.hh
 *
 * @brief Header of the StateManager class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _STATEMANAGER_HH_
#define _STATEMANAGER_HH_

#include "LogTypes.hh"
#include "ReadConfig.hh"
#include "ToolList.hh"
#include "utils/FullLinkedList.hh"
#include <stdio.h>

typedef FullLinkedList<log_msg_t> StateList;

/**
 * @brief Class that manages the different states of the log central
 * @class StateManager
 */
class StateManager
{
public:
  /**
   * @brief Initialize the StateManager
   * The tags IN and OUT are always important.
   * @param readConfig ReadConfig reference
   * @param success used to indicated if the initialisation was successfull.
   */
  StateManager(ReadConfig* readConfig, bool* success);
  ~StateManager();

  /**
   * @brief To know if a message must be broadcast to all Tools
   * By the same time register it if necessary (Important Tag)
   * @param msg Message to check and/or register
   * @return true if the message must be boradcast
   */
  bool
  check(log_msg_t* msg);

  /**
   * @brief Call by the LogCentraTool_impl to send the current SystemState
   * to a new tool.
   * @param toolName name of the tool to send the SystemState
   */
  void
  askForSystemState(OutBuffer* outBuffer);

private:
  tag_list_t* dynamicTagsList;
  tag_list_t* dynamicStarts;
  tag_list_t* dynamicStops;
  tag_list_t* staticTagsList;
  tag_list_t* uniqueTagsList;
  char* start;
  char* stop;
  ToolList* toolList;
  StateList* stateList;
};

#endif

