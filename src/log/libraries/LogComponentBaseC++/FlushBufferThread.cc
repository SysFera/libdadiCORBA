/****************************************************************************/
/* FlushBufferThread implementation class                                   */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: FlushBufferThread.cc,v 1.1 2010/11/10 03:02:28 kcoulomb Exp $
 * $Log: FlushBufferThread.cc,v $
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

FlushBufferThread::FlushBufferThread(LogComponentBase* LCB):
LCB(LCB),
threadRunning(false)
{
  this->name = this->LCB->getName();
}

FlushBufferThread::~FlushBufferThread()
{
  if (this->threadRunning) {
    this->threadRunning = false;
    join(NULL);
  }
  delete this->name;
}

void
FlushBufferThread::startThread()
{
  if (this->threadRunning) {
    return;
  }
  this->threadRunning = true;
  start_undetached();
}

void
FlushBufferThread::stopThread()
{
  if (!this->threadRunning) {
    return;
  }
  this->threadRunning = false;
  join(NULL);
}

void*
FlushBufferThread::run_undetached(void* params)
{
  while (this->threadRunning) {
    try {
      this->LCB->LCCref->sendBuffer(this->LCB->buffer);
      this->LCB->buffer.length(0);
    } catch (...) {
    }
    sleep(FLUSHBUFFERTHREAD_SLEEP_TIME_SEC,
          FLUSHBUFFERTHREAD_SLEEP_TIME_NSEC);
  }
  return NULL;
}
