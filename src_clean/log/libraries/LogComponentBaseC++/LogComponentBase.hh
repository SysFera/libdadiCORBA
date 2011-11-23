/****************************************************************************/
/* LogComponentBase header class                                            */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogComponentBase.hh,v 1.5 2011/02/04 15:53:55 bdepardo Exp $
 * $Log: LogComponentBase.hh,v $
 * Revision 1.5  2011/02/04 15:53:55  bdepardo
 * Use a const char* name in constructor, then strdup the name.
 * Then use free instead of delete on this->name
 *
 * Revision 1.4  2010/12/03 12:40:26  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.3  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
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

#ifndef _LOGCOMPONENTBASE_HH_
#define _LOGCOMPONENTBASE_HH_

#include <omnithread.h>

#include "LogTypes.hh"
#include "ComponentConfigurator_impl.hh"
#include "PingThread.hh"
#include "FlushBufferThread.hh"
#include "LogComponent.hh"

#define PINGTHREAD_SLEEP_TIME_SEC          1
#define PINGTHREAD_SLEEP_TIME_NSEC         0
#define PINGTHREAD_SYNCHRO_FREQUENCY       60
#define FLUSHBUFFERTHREAD_SLEEP_TIME_SEC   0
#define FLUSHBUFFERTHREAD_SLEEP_TIME_NSEC  50000000

class LogComponentBase;

class LogComponentBase {
public:
  LogComponentBase(bool* success, int argc, char** argv,
                   unsigned int tracelevel, const char* name,
                   unsigned int port = 0);
  ~LogComponentBase();

  void
  setName(const char* name);

  char*
  getName();

  char*
  getHostname();

  short
  connect(const char* message);

  short
  disconnect(const char* message);

  void
  sendMsg(const char* tag, const char* msg);

  bool
  isLog(const char* tagname);

private:
  log_time_t
  getLocalTime();

public:
  LogCentralComponent_var LCCref;
  ComponentConfigurator_var CCref;
  log_msg_buf_t buffer;
  tag_list_t currentTagList;

private:
  char* name;
  char* hostname;
  ComponentConfigurator_impl* CCimpl;
  PingThread* pingThread;
  FlushBufferThread* flushBufferThread;
};

#endif

