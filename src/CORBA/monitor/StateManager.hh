/****************************************************************************/
/* Header for the StateManager class                                        */
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

#ifndef _STATEMANAGER_HH_
#define _STATEMANAGER_HH_

#include <cstdio>
#include "LogTypes.hh"
#include "ReadConfig.hh"
#include "ToolList.hh"
#include "FullLinkedList.hh"

typedef FullLinkedList<log_msg_t> StateList;

class StateManager {
public:
  /**
   * Initialize the StateManager
   * The tags IN and OUT are always important.
   * @param readConfig ReadConfig reference
   * @param success used to indicated if the initialisation was successfull.
   */
  StateManager(ReadConfig* readConfig, bool* success);

  ~StateManager();

  /**
   * To know if a message must be broadcast to all Tools
   * By the same time register it if necessary (Important Tag)
   * @param msg Message to check and/or register
   * @return true if the message must be boradcast
   */
  bool
  check(log_msg_t* msg);

  /**
   * Call by the LogCentraTool_impl to send the current SystemState
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

