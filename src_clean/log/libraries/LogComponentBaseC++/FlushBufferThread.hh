/****************************************************************************/
/* FlushBufferThread header class                                           */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: FlushBufferThread.hh,v 1.1 2010/11/10 03:02:28 kcoulomb Exp $
 * $Log: FlushBufferThread.hh,v $
 * Revision 1.1  2010/11/10 03:02:28  kcoulomb
 * Add missing files
 *
 * Revision 1.2  2007/08/31 16:41:17  bdepardo
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

#ifndef _FLUSHBUFFERTHREAD_HH_
#define _FLUSHBUFFERTHREAD_HH_


#include "LogTypes.hh"
#include "LogComponent.hh"
#include <omnithread.h>


class FlushBufferThread:public omni_thread
{
public:
  FlushBufferThread(LogComponentBase* LCB);
  ~FlushBufferThread();

  void
  startThread();

  void
  stopThread();

private:
  void*
  run_undetached(void* params);

private:
  LogComponentBase* LCB;
  char* name;
  bool threadRunning;
};


#endif
