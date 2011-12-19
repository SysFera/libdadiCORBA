/**
 * @file CoreThread.cc
 *
 * @brief Implementation of the CoreThread class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "CoreThread.hh"
#include "LogTypes.hh"
#include "utils/LocalTime.hh"
#include "LogOptions.hh"
#include <iostream>

using namespace std;

CoreThread::CoreThread(TimeBuffer* timeBuffer, StateManager* stateManager,
                       FilterManagerInterface* filterManager,
                       ToolList* toolList):
mtimeBuffer(timeBuffer),
mstateManager(stateManager),
mfilterManager(filterManager),
mtoolList(toolList),
mthreadRunning(false)
{
}

CoreThread::~CoreThread()
{
  if (this->mthreadRunning) {
    this->mthreadRunning = false;
    join(NULL);
  }
}

void
CoreThread::startThread()
{
  if (this->mthreadRunning) {
    return;
  }
  this->mthreadRunning = true;
  start_undetached();
}

void
CoreThread::stopThread()
{
  if (!this->mthreadRunning) {
    return;
  }
  this->mthreadRunning = false;
  join(NULL);
}

void*
CoreThread::run_undetached(void* params)
{
  log_time_t minAge;
  log_msg_t* msg = NULL;
  ToolList::ReadIterator* it = NULL;
  bool haveMsgs;
  while (this->mthreadRunning) {
    minAge = getLocalTime();
    minAge.sec -= LogOptions::CORETHREAD_MINAGE_TIME_SEC;
    minAge.msec -= LogOptions::CORETHREAD_MINAGE_TIME_MSEC;
    if (minAge.msec < 0) {
      minAge.sec--;
      minAge.msec += 1000;
    }
    haveMsgs=true;
    while (haveMsgs) {
      msg = this->mtimeBuffer->get(minAge);
      if (msg != NULL) { // we have a message
        if (this->mstateManager->check(msg)) { // directly sent
          it = this->mtoolList->getReadIterator();
          while (it->hasCurrent()) {
            it->getCurrentRef()->outBuffer.push(msg);
            it->nextRef();
          }
          delete it;
        } else { // send to the FilterManager
          this->mfilterManager->sendMessageWithFilters(msg);
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

