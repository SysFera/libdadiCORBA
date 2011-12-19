/**
 * @file CoreThread.hh
 *
 * @brief Header of the core thread class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

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


