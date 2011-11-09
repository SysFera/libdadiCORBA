/****************************************************************************/
/* Header for the Core thread                                               */
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
 ****************************************************************************/

#ifndef _CORETHREAD_HH_
#define _CORETHREAD_HH_

#include <omnithread.h>
#include "TimeBuffer.hh"
#include "StateManager.hh"
#include "FilterManagerInterface.hh"
#include "ToolList.hh"

class CoreThread:public omni_thread {
public:
  CoreThread(TimeBuffer* timeBuffer, StateManager* stateManager,
             FilterManagerInterface* filterManager, ToolList* toolList);

  ~CoreThread();

  /**
   * Start the thread. Return immediately.
   */
  void
  startThread();

  /**
   * Stop the thread. Return when the thread is stopped.
   */
  void
  stopThread();

private:
  void*
  run_undetached(void* params);

  TimeBuffer* timeBuffer;
  StateManager* stateManager;
  FilterManagerInterface* filterManager;
  ToolList* toolList;
  bool threadRunning;
};
#endif


