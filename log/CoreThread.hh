/**
 * @file CoreThread.hh
 *
 * @brief Header of the core thread class
 *
 * @author
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
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

/**
 * @brief The core for the log central tool
 * @class CoreThread
 */
class CoreThread:public omni_thread {
public:
/**
 * @brief Constructor
 * @param timeBuffer A time buffer
 * @param stateManager A state manager
 * @param filterManager A filter manager
 * @param toolList A toollist object
 */
  CoreThread(TimeBuffer* timeBuffer, StateManager* stateManager,
             FilterManagerInterface* filterManager, ToolList* toolList);

/**
 * @brief Destructor
 */
  ~CoreThread();

  /**
   * @brief Start the thread. Return immediately.
   */
  void
  startThread();

  /**
   * @brief Stop the thread. Return when the thread is stopped.
   */
  void
  stopThread();

private:
/**
 * @brief Undetach the thread
 */
  void*
  run_undetached(void* params);

/**
 * @brief A time buffer
 */
  TimeBuffer* mtimeBuffer;
/**
 * @brief A state manager
 */
  StateManager* mstateManager;
/**
 * @brief A filter manager
 */
  FilterManagerInterface* mfilterManager;
/**
 * @brief A tool list
 */
  ToolList* mtoolList;
/**
 * @brief If the thread is running
 */
  bool mthreadRunning;
};
#endif


