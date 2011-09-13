/****************************************************************************/
/* PingThread implementation class                                          */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: PingThread.cc,v 1.1 2010/11/10 03:02:28 kcoulomb Exp $
 * $Log: PingThread.cc,v $
 * Revision 1.1  2010/11/10 03:02:28  kcoulomb
 * Add missing files
 *
 * Revision 1.2  2007/08/31 16:41:16  bdepardo
 * When trying to add a new component, we check if the name of the component exists and if the component is reachable
 * - it the name already exists:
 *    - if the component is reachable, then we do not connect the new component
 *    - else we consider that the component is lost, and we delete the old component ant add the new one
 * - else add the component
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "LogComponentBase.hh"
#include "ORBTools.hh"
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
using namespace std;

PingThread::PingThread(LogComponentBase* LCB):
LCB(LCB),
threadRunning(false)
{
  this->name = this->LCB->getName();
}

PingThread::~PingThread()
{
  if (this->threadRunning) {
    this->threadRunning = false;
    join(NULL);
  }
  delete this->name;
}

void
PingThread::startThread()
{
  if (this->threadRunning) {
    return;
  }
  this->threadRunning = true;
  start_undetached();
}

void
PingThread::stopThread()
{
  if (!this->threadRunning) {
    return;
  }
  this->threadRunning = false;
  join(NULL);
}

void*
PingThread::run_undetached(void* params)
{
  unsigned int counter = 0;
  log_time_t lt;
  while (this->threadRunning) {
    try {
      //      this->LCB->LCCref->ping(this->name);
    } catch (...) {
    }
    if (counter == PINGTHREAD_SYNCHRO_FREQUENCY) {
      lt = this->getLocalTime();
      try {
        this->LCB->LCCref->synchronize(this->name, lt);
      } catch (...) {
      }
      counter = 0;
    }
    counter++;
    sleep(PINGTHREAD_SLEEP_TIME_SEC,
          PINGTHREAD_SLEEP_TIME_NSEC);
  }
  return NULL;
}

log_time_t
PingThread::getLocalTime() {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  log_time_t ret;
  ret.sec = tv.tv_sec;
  ret.msec = tv.tv_usec / 1000;
  return ret;
}
