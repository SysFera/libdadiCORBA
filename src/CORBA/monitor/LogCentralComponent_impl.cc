/****************************************************************************/
/* Implementation corresponding to the LogComponentComponent interface      */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.10  2011/02/04 15:14:26  bdepardo
 * Initialize members in constructor.
 * Reduce variable scope.
 *
 * Revision 1.9  2010/12/13 12:21:14  kcoulomb
 * Clean types
 *
 * Revision 1.8  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/Options due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.7  2010/12/03 12:40:27  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.6  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.5  2007/09/03 06:33:24  bdepardo
 * Removed IOR, it was useless.
 *
 * Revision 1.4  2007/08/31 16:41:17  bdepardo
 * When trying to add a new component, we check if the name of the component exists and if the component is reachable
 * - it the name already exists:
 *    - if the component is reachable, then we do not connect the new component
 *    - else we consider that the component is lost, and we delete the old component ant add the new one
 * - else add the component
 *
 * Revision 1.3  2004/06/01 21:39:57  hdail
 * Tracking down seg fault of LogService:
 * - Corrected mis-matched malloc / delete
 * - Corrected error in size of string allocation, and terminated string with \0
 *
 * Revision 1.2  2004/03/02 08:38:16  rbolze
 * add some information in the IN and OUT messages
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "LogCentralComponent_impl.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "LocalTime.hh"
#include "LogOptions.hh"
#include "ORBMgr.hh"

// helpers
#include "ORBTools.hh"

#include "debug.hh"


LastPing::LastPing() {
  this->name = NULL;
  this->time.sec = 0;
  this->time.msec = 0;
  this->timeDifference.sec = 0;
  this->timeDifference.msec = 0;
}

LastPing::LastPing(LastPing& newLastPing) {
  this->name = strdup(newLastPing.name);
  this->time = newLastPing.time;
  this->timeDifference = newLastPing.timeDifference;
}

LastPing::~LastPing() {
  if (this->name != NULL) {
    free(this->name);
  }
}

LastPing&
LastPing::operator=(LastPing& newLastPing) {
  if (this->name != NULL) {
    delete this->name;
  }
  this->name = strdup(newLastPing.name);
  this->time = newLastPing.time;
  this->timeDifference = newLastPing.timeDifference;
  return *this;
}

/****************************************************************************
 * LogCentralComponent_impl inplementation
 ****************************************************************************/

LogCentralComponent_impl::LogCentralComponent_impl(
  ComponentList* componentList, FilterManagerInterface* filterManager,
  TimeBuffer* timeBuffer) {
  this->componentList = componentList;
  this->filterManager = filterManager;
  this->timeBuffer = timeBuffer;
  this->lastPings = new LastPings();
  this->aliveCheckThread = new AliveCheckThread(this);
  this->aliveCheckThread->startThread();
}

LogCentralComponent_impl::~LogCentralComponent_impl() {
  delete this->lastPings;
  // stop and (automatically) delete the thread
  this->aliveCheckThread->stopThread();
}


CORBA::Short
LogCentralComponent_impl::connectComponent(char*& componentName,
                                           const char* componentHostname,
                                           const char* message,
                                           const char* compConfigurator,
                                           const log_time_t& componentTime,
                                           tag_list_t& initialConfig) {
  if (strcmp(componentName, "*") == 0) {
    fprintf(stderr, "Bad name componnent. Cannot connect component. \n");
    return LS_COMPONENT_CONNECT_BADNAME;
  }
  ComponentConfigurator_ptr compoConf =
    ORBMgr::getMgr()->resolve<ComponentConfigurator,
                                 ComponentConfigurator_ptr>(LOGCOMPCONFCTXT,
                                                            compConfigurator);

  if (CORBA::is_nil(compoConf)) {
    fprintf(stderr, "Bad component configurator **** \n");
    return LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR;
  }

  // Put this here to be synchronised
  ComponentList::Iterator* it = componentList->getIterator();
  // Generate a unique name if the name is empty
  if (componentName == NULL || strcmp(componentName, "") == 0) {
    const char* s = getGeneratedName(componentHostname, it);
    componentName = CORBA::string_dup(s);
  }

  // Check for the previous existence of the component
  bool lost = false;
  if (isComponentExists((const char*)componentName, it)) {
    // check if component is still alive
    try {
//      it->getCurrentRef()->componentConfigurator->test();
    }
    catch (...) {
      lost = true;
    }

    if (!lost) {
      TRACE_TEXT(TRACE_MAIN_STEPS,
                 "Connection of component '" << componentName << "' failed, "
                 << "component already exists\n");
      delete(it);
      return LS_COMPONENT_CONNECT_ALREADYEXISTS;
    }
  }
  // Add the component
  ComponentElement* ce = new ComponentElement();
  ce->componentConfigurator = ComponentConfigurator::_narrow(compoConf);
  ce->componentName = CORBA::string_dup(componentName);
  delete it;
  if (lost) {
    char* msg;
    size_t sz = 63 + strlen(componentName);
    msg = (char*) malloc(sizeof(char)*sz);
    snprintf(msg, sz,
             "Component name %s already exists, but component seems to be down",
             componentName);
    disconnectComponent(componentName, msg);
  }

  it = componentList->getIterator();
  it->reset();
  it->insertBeforeRef(ce);

  // Add the last ping
  log_time_t localTime = getLocalTime();
  LastPing lp;
  lp.name = strdup(componentName);
  lp.time = localTime;
  lp.timeDifference.sec = localTime.sec - componentTime.sec;
  lp.timeDifference.msec = localTime.msec - componentTime.msec;
  this->lastPings->push(&lp);
  // Notify the FilterManager
  tag_list_t* tl = this->filterManager->componentConnect(
    (const char*)componentName, it);
  if (tl == NULL) {
    fprintf(stderr, "Connecting component failed after filter \n");
    delete(it);
    return LS_COMPONENT_CONNECT_INTERNALERROR;
  }
  delete it;

  // Create a new IN message
  char* msg;
  size_t sz = strlen(message) + strlen(componentHostname) + 2;
  msg = (char*) malloc(sz);
  snprintf(msg, sz, "%s %s\0", message , componentHostname);
  log_msg_t* inmsg = new log_msg_t();
  inmsg->componentName = CORBA::string_dup(componentName);
  inmsg->time = localTime;
  inmsg->tag = CORBA::string_dup("IN");
  inmsg->msg = CORBA::string_dup(msg);
  this->timeBuffer->put(inmsg);
  delete inmsg;
  // Return the initialConfig
  // update the tag_list_t&
  initialConfig = *tl;
  TRACE_TEXT(TRACE_MAIN_STEPS, "Connection of component '"
             << componentName << "' with message"
             << " '" << message << "'\n");
  return LS_OK;
}

CORBA::Short
LogCentralComponent_impl::disconnectComponent(const char* componentName,
                                              const char* message) {
  // Find the component to delete it
  ComponentList::Iterator* it = this->componentList->getIterator();
  bool found = false;
  while (it->hasCurrent()) {
    if (strcmp(it->getCurrentRef()->componentName, componentName) == 0) {
      found = true;
      break;
    }
    it->nextRef();
  }
  if (!found) {
    delete it;
    TRACE_TEXT(TRACE_MAIN_STEPS,
               "Disconnection of component '" << componentName << "' failed,"
               << " component not exists\n");
    return LS_COMPONENT_DISCONNECT_NOTEXISTS;
  }
  // Remove the component
  it->removeCurrent();
  // Migrate it to a ReadItarator
  ComponentList::ReadIterator* readIterator =
    this->componentList->reduceWriteIterator(it);

  // Notify the FilterManager
  this->filterManager->componentDisconnect(componentName, readIterator);

  // Remove the last ping
  LastPings::Iterator* it2 = this->lastPings->getIterator();
  while (it2->hasCurrent()) {
    if (strcmp(it2->getCurrentRef()->name, componentName) == 0) {
      it2->removeCurrent();
      break;
    }
    it2->nextRef();
  }
  delete it2;
  delete readIterator;

  // Create a new OUT message
  log_time_t localTime = getLocalTime();
  log_msg_t* inmsg = new log_msg_t();
  inmsg->componentName = CORBA::string_dup(componentName);
  inmsg->time = localTime;
  inmsg->tag = CORBA::string_dup("OUT");
  inmsg->msg = CORBA::string_dup(message);
  this->timeBuffer->put(inmsg);
  delete inmsg;

  TRACE_TEXT(TRACE_MAIN_STEPS,
             "Disconnection of '" << componentName << "' with message"
             << " '" << message << "'\n");
  return LS_OK;
}

void
LogCentralComponent_impl::sendBuffer(const log_msg_buf_t& buffer) {
  // for each message, correction of its time and the message is sent to the
  // TimeBuffer.
  log_time_t td;

  LastPing* lp = NULL;
  if (buffer.length() != 0) {
    const char* name = buffer[0].componentName;
    LastPings::ReadIterator* it = this->lastPings->getReadIterator();

    bool compExists = false;
    while (it->hasCurrent()) {
      lp = it->nextRef();
      if (strcmp(lp->name, name) == 0) {
        td = lp->timeDifference;
        compExists = true;
        break;
      }
    }
    delete it;

    if (compExists == false) {
      TRACE_TEXT(TRACE_MAIN_STEPS,
                 "Discarded messageBuffer from unknown component "
                 << name << "\n");
      return;
    }

    for (unsigned int i = 0; i < buffer.length(); i++) {
      log_msg_t msg = buffer[i];
      // Correct the time derivation
      msg.time.sec += td.sec;
      msg.time.msec += td.msec;
      while (msg.time.msec < 0) {
        msg.time.msec += 1000;
        msg.time.sec -= 1;
      }
      while (msg.time.msec >= 1000) {
        msg.time.msec -= 1000;
        msg.time.sec += 1;
      }
      // FIXME: manage overflows here
      this->timeBuffer->put(&msg);
    }
  }
}

bool
LogCentralComponent_impl::isComponentExists(const char* name,
                                            ComponentList::ReadIterator* it) {
  it->reset();
  bool found = false;
  while (it->hasCurrent()) {
    if (strcmp(it->getCurrentRef()->componentName, name) == 0) {
      found = true;
      break;
    }
    it->nextRef();
  }
  return found;
}

void
LogCentralComponent_impl::ping(const char* componentName) {
  // Find the component
  LastPings::Iterator* it = this->lastPings->getIterator();
  while (it->hasCurrent()) {
    if (strcmp(it->getCurrentRef()->name, componentName) == 0) {
      it->getCurrentRef()->time = getLocalTime();
      break;
    }
    it->nextRef();
  }
  delete it;
}

void
LogCentralComponent_impl::synchronize(const char* componentName,
                                      const log_time_t& componentTime) {
  log_time_t localTime = getLocalTime();
  // Find the component
  LastPings::Iterator* it = this->lastPings->getIterator();
  while (it->hasCurrent()) {
    if (strcmp(it->getCurrentRef()->name, componentName) == 0) {
      it->getCurrentRef()->timeDifference.sec =
        localTime.sec - componentTime.sec;
      it->getCurrentRef()->timeDifference.msec =
        localTime.msec - componentTime.msec;
      break;
    }
    it->nextRef();
  }
  delete it;
}

char*
LogCentralComponent_impl::getGeneratedName(const char* hostname,
                                           ComponentList::ReadIterator* it) {
  unsigned int num = 1;
  size_t sz = strlen(hostname) + 10;
  char* ret = new char[sz];  // I think it's sufficient
  snprintf(ret, sz, "%s_%u", hostname, num);
  while (isComponentExists(ret, it)) {
    num++;
    snprintf(ret, sz, "%s_%u", hostname, num);
  }
  return strdup(ret);
}

/****************************************************************************
 * AliveCheckThread implementation
 ****************************************************************************/

LogCentralComponent_impl::AliveCheckThread::AliveCheckThread(
  LogCentralComponent_impl* LCC)
  : LCC(LCC), threadRunning(false) {
}

LogCentralComponent_impl::AliveCheckThread::~AliveCheckThread() {
  if (this->threadRunning) {
    this->stopThread();
  }
}

void
LogCentralComponent_impl::AliveCheckThread::startThread() {
  if (this->threadRunning) {
    return;
  }
  this->threadRunning = true;
  start_undetached();
}

void
LogCentralComponent_impl::AliveCheckThread::stopThread() {
  if (!this->threadRunning) {
    return;
  }
  this->threadRunning = false;
  join(NULL);
}

void*
LogCentralComponent_impl::AliveCheckThread::run_undetached(void* params) {
  LastPing* lp = NULL;
  log_time_t checkTime;
  FullLinkedList<char>* componentsToDisconnect = NULL;
  bool somePingTimeout = false;
  while (this->threadRunning) {
    checkTime = getLocalTime();
    checkTime.sec -= LogOptions::ALIVECHECKTHREAD_DEAD_TIME_SEC;
    checkTime.msec -= LogOptions::ALIVECHECKTHREAD_DEAD_TIME_MSEC;
    componentsToDisconnect = new FullLinkedList<char>();
    somePingTimeout = false;
    // Check for all in the LastPings buffer
    LastPings::ReadIterator* it = this->LCC->lastPings->getReadIterator();
    while (it->hasCurrent()) {
      lp = it->nextRef();
      if ((lp->time.sec < checkTime.sec)
          || ((lp->time.sec == checkTime.sec)
              && (lp->time.msec < checkTime.msec))) {
        // ping too old
        componentsToDisconnect->pushRef(strdup(lp->name));
        somePingTimeout = true;
      }
    }
    delete it;
    char* s;
    if (somePingTimeout) {
      FullLinkedList<char>::ReadIterator* it2 =
        componentsToDisconnect->getReadIterator();
      while (it2->hasCurrent()) {
        char* msg;
        s = it2->nextRef();
        size_t sz = strlen(s) + strlen("Ping Timeout");
        msg = (char*) malloc(sz);
        TRACE_TEXT(TRACE_MAIN_STEPS,
                   "Ping Timeout of '" << s
                   <<"': disconnect the component.\n");
        snprintf(msg, sz, "%s Ping Timeout", s);
        this->LCC->disconnectComponent(s, msg);
      }
      delete it2;
    }
    delete componentsToDisconnect;
    sleep(LogOptions::ALIVECHECKTHREAD_SLEEP_TIME_SEC,
          LogOptions::ALIVECHECKTHREAD_SLEEP_TIME_NSEC);
  }
  return NULL;
}

void
LogCentralComponent_impl::test() {
  return;
}


////////////////////////////////// FORWARDER IMPL


/****************************************************************************
 * LogCentralComponent_impl inplementation
 ****************************************************************************/

LogCentralComponentFwdrImpl::LogCentralComponentFwdrImpl(
  Forwarder_ptr fwdr, const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
  this->componentList = NULL;
  this->filterManager = NULL;
  this->timeBuffer = NULL;
  this->lastPings = NULL;
}

LogCentralComponentFwdrImpl::~LogCentralComponentFwdrImpl() {
}

CORBA::Short
LogCentralComponentFwdrImpl::connectComponent(char*& componentName,
                                              const char* componentHostname,
                                              const char* message,
                                              const char* compConfigurator,
                                              const log_time_t& componentTime,
                                              tag_list_t& initialConfig) {
  return forwarder->connectComponent(componentName, componentHostname,
                                     message, compConfigurator,
                                     componentTime, initialConfig, objName);
}

CORBA::Short
LogCentralComponentFwdrImpl::disconnectComponent(const char* componentName,
                                                 const char* message) {
  return forwarder->disconnectComponent(componentName, message, objName);
}

void
LogCentralComponentFwdrImpl::sendBuffer(const log_msg_buf_t& buffer) {
  return forwarder->sendBuffer(buffer, objName);
}

void
LogCentralComponentFwdrImpl::ping(const char* componentName) {
//  return forwarder->ping(componentName, objName);
  return;
}

void
LogCentralComponentFwdrImpl::synchronize(const char* componentName,
                                         const log_time_t& componentTime) {
  return forwarder->synchronize(componentName, componentTime, objName);
}

void
LogCentralComponentFwdrImpl::test() {
//  return forwarder->test(objName);
}


ComponentConfiguratorFwdr_impl::ComponentConfiguratorFwdr_impl(
  Forwarder_ptr fwdr, const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

ComponentConfiguratorFwdr_impl::~ComponentConfiguratorFwdr_impl() {
}

void
ComponentConfiguratorFwdr_impl::setTagFilter(const tag_list_t& tagList) {
  return forwarder->setTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::addTagFilter(const tag_list_t& tagList) {
  return forwarder->addTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::removeTagFilter(const tag_list_t& tagList) {
  return forwarder->removeTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::test() {
//  return forwarder->test (objName);
}
