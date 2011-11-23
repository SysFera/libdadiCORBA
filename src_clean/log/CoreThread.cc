/****************************************************************************/
/* Implementation for the Core thread                                       */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: CoreThread.cc,v 1.3 2010/12/08 11:37:54 kcoulomb Exp $
 * $Log: CoreThread.cc,v $
 * Revision 1.3  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/LogOptions due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.2  2004/01/13 15:07:15  ghoesch
 * speeded up message processing after poor results in tests
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "CoreThread.hh"
#include "LogTypes.hh"
#include "LocalTime.hh"
#include "LogOptions.hh"
#include <iostream>

using namespace std;

CoreThread::CoreThread(TimeBuffer* timeBuffer, StateManager* stateManager,
                       FilterManagerInterface* filterManager,
                       ToolList* toolList):
timeBuffer(timeBuffer),
stateManager(stateManager),
filterManager(filterManager),
toolList(toolList),
threadRunning(false)
{
}

CoreThread::~CoreThread()
{
  if (this->threadRunning) {
    this->threadRunning = false;
    join(NULL);
  }
}

void
CoreThread::startThread()
{
  if (this->threadRunning) {
    return;
  }
  this->threadRunning = true;
  start_undetached();
}

void
CoreThread::stopThread()
{
  if (!this->threadRunning) {
    return;
  }
  this->threadRunning = false;
  join(NULL);
}

void*
CoreThread::run_undetached(void* params)
{
  log_time_t minAge;
  log_msg_t* msg = NULL;
  ToolList::ReadIterator* it = NULL;
  bool haveMsgs;
  while (this->threadRunning) {
    minAge = getLocalTime();
    minAge.sec -= LogOptions::CORETHREAD_MINAGE_TIME_SEC;
    minAge.msec -= LogOptions::CORETHREAD_MINAGE_TIME_MSEC;
    if (minAge.msec < 0) {
      minAge.sec--;
      minAge.msec += 1000;
    }
    haveMsgs=true;
    while (haveMsgs) {
      msg = this->timeBuffer->get(minAge);
      if (msg != NULL) { // we have a message
        if (this->stateManager->check(msg)) { // directly sent
          it = this->toolList->getReadIterator();
          while (it->hasCurrent()) {
            it->getCurrentRef()->outBuffer.push(msg);
            it->nextRef();
          }
          delete it;
        } else { // send to the FilterManager
          this->filterManager->sendMessageWithFilters(msg);
        }
        delete msg;
      } else {
        haveMsgs=false;
      }
    }

    sleep(LogOptions::CORETHREAD_SLEEP_TIME_SEC,
          LogOptions::CORETHREAD_SLEEP_TIME_NSEC);
  }
  return NULL;
}

