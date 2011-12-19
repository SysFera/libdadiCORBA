/**
 * @file SendThread.hh
 *
 * @brief A thread that continously empties the tools outbuffers by sending them
 * to the corresponding toolMsgReceiver. Based on omni_thread and the orb.
 * Attention: causes memory leaks if the orb does not exist !!
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _SENDTHREAD_HH_
#define _SENDTHREAD_HH_

#include <omnithread.h>
#include "ToolList.hh"
#include "LogTool.hh"

class SendThread: public omni_thread {
public:
  /**
   * Creates a SendThread. The thread can be started with runThread().
   * Use stopThread() to stop and delete the thread.
   *
   * @param toolList. The toolList that the thread will work on.
   */
  explicit SendThread(ToolList* toolList);

  /**
   * Start the thread
   */
  void
  startThread();

  /**
   * Stops the thread. Waits for the thread to terminate properly.
   * Deletes the threadobject if the orb is running. A missing orb
   * results in memory leaks.
   */
  void
  stopThread();

protected:
  /**
   * Main function of thread
   * Contains main loop of thread, which has to end if
   * runSendThread is set to false
   */
  void*
  run_undetached(void* arg);

  /**
   * Destructor of function. Stops the thread if still running.
   * This function should not be called directly. The object is
   * implicitly deleted by the orb when join() is called. The
   * orb must exist to make this work.
   */
  ~SendThread();

private:
  /**
   * Shared variable that indicates if the thread is running or not.
   * stopThread will set this to false to stop the main loop of this
   * thread.
   */
  bool runSendThread;

  /**
   * Stores the toolList the thread uses
   */
  ToolList* toolList;
};

#endif
