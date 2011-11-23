/****************************************************************************/
/* LogComponentBase implementation class                                    */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogComponentBase.cc,v 1.8 2011/02/04 15:56:03 bdepardo Exp $
 * $Log: LogComponentBase.cc,v $
 * Revision 1.8  2011/02/04 15:56:03  bdepardo
 * Add missing include
 *
 * Revision 1.7  2011/02/04 15:53:55  bdepardo
 * Use a const char* name in constructor, then strdup the name.
 * Then use free instead of delete on this->name
 *
 * Revision 1.6  2011/02/04 15:21:50  bdepardo
 * delete [] hostname instead of delete hostname
 *
 * Revision 1.5  2010/12/03 12:40:26  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.4  2010/11/10 04:24:50  bdepardo
 * LogServiceC instead of LogService
 *
 * Revision 1.3  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
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

#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>

#include "LogComponentBase.hh"
#include "ORBTools.hh"
#include "ORBMgr.hh"

using namespace std;

LogComponentBase::LogComponentBase(bool* success, int argc, char** argv,
                                   unsigned int tracelevel, const char* name,
                                   unsigned int port) {
  *success = false;
  this->name = strdup(name);
  this->hostname = new char[256];
  if (gethostname(hostname, 255)) {
    return;
  }
  if (!ORBTools::init(argc, argv, tracelevel, port)) {
    return;
  }
  CORBA::Object* object;
  if (!ORBTools::findServant("LogServiceC", "", "LCC", "", object)) {
    cerr << "Cannot contact the LogComponent servant of LogService!\n";
    return;
  }
  this->LCCref = LogCentralComponent::_narrow(object);
  this->CCimpl = new ComponentConfigurator_impl(this);
  if (!ORBTools::activateServant(this->CCimpl)) {
    cerr << "Cannot activate the ComponentConfigurator servant !\n";
    return;
  }
  if (!ORBTools::activatePOA()) {
    return;
  }
  this->CCref = this->CCimpl->_this();
  this->CCimpl = NULL;
  this->pingThread = NULL;
  this->flushBufferThread = NULL;

  ORBMgr::getMgr()->bind("LogServiceC", getName(), this->CCref, true);
  ORBMgr::getMgr()->fwdsBind("LogServiceC", getName(),
                             ORBMgr::getMgr()->getIOR(this->CCref));
  // Comment because test is empty
  //  try { // To make sure that the object is callable
  //    //    this->LCCref->test();
  //  } catch (...) {
  //    return;
  //  }
  *success = true;
}

LogComponentBase::~LogComponentBase()
{
  if (this->pingThread != NULL) {
    this->pingThread->stopThread();
  }
  if (this->flushBufferThread != NULL) {
    this->flushBufferThread->stopThread();
  }
  free(this->name);
  delete [] this->hostname;
  ORBTools::kill();
}

void
LogComponentBase::setName(const char* name)
{
  if (this->pingThread != NULL) {
    this->disconnect("change of name");
  }
  free(this->name);
  this->name = strdup(name);
}

char*
LogComponentBase::getName()
{
  return strdup(this->name);
}

char*
LogComponentBase::getHostname()
{
  return strdup(this->hostname);
}

short
LogComponentBase::connect(const char* message)
{
  if (this->pingThread != NULL) {
    this->disconnect("for immediate reconnection");
  }
  short ret;
  CORBA::String_var corbaname = CORBA::string_dup(this->name);
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  log_time_t lt;
  lt.sec = tv.tv_sec;
  lt.msec = tv.tv_usec / 1000;
  try {
    ret = this->LCCref->connectComponent(
      corbaname,
      this->hostname,
      message,
      this->name, // MODIF, avant 'this->CC'
      lt,
      this->currentTagList
    );
  } catch (...) {
    return 1;
  }
  //  delete this->name;
  this->name = CORBA::string_dup(corbaname);
  this->pingThread = new PingThread(this);
  this->flushBufferThread = new FlushBufferThread(this);
  this->pingThread->startThread();
  this->flushBufferThread->startThread();
  return ret;
}

short
LogComponentBase::disconnect(const char* message)
{
  if (this->pingThread == NULL) {
    return LS_OK;
  }
  this->pingThread->stopThread();
  this->flushBufferThread->stopThread();
  this->pingThread = NULL;
  this->flushBufferThread = NULL;
  short ret;
  try {
    ret = this->LCCref->disconnectComponent(this->name, message);
  } catch (...) {
    ret = 1;
  }
  return ret;
}

void
LogComponentBase::sendMsg(const char* tag, const char* msg)
{
  log_msg_t logmsg;
  logmsg.componentName = CORBA::string_dup(this->name);
  logmsg.time = this->getLocalTime();
  logmsg.warning = false;
  logmsg.tag = CORBA::string_dup(tag);
  logmsg.msg = CORBA::string_dup(msg);
  CORBA::ULong l = this->buffer.length();
  this->buffer.length(l + 1);
  this->buffer[l] = logmsg;
}

bool
LogComponentBase::isLog(const char* tagname)
{
  bool ret = false;
  CORBA::ULong l = this->currentTagList.length();
  for (CORBA::ULong i = 0 ; i < l ; i++) {
    if (strcmp(tagname, this->currentTagList[i]) == 0) {
      ret = true;
      break;
    }
    if (strcmp("*", this->currentTagList[i]) == 0) {
      ret = true;
      break;
    }
  }
  return ret;
}

log_time_t
LogComponentBase::getLocalTime() {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  log_time_t ret;
  ret.sec = tv.tv_sec;
  ret.msec = tv.tv_usec / 1000;
  return ret;
}

