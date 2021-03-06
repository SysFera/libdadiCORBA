/**
 * @file LogCentralComponent_impl.cc
 *
 * @brief Implementation corresponding to the LogComponentComponent interface
 *
 * @author
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "LogCentralComponent_impl.hh"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "utils/LocalTime.hh"
#include "LogOptions.hh"
#include "ORBMgr.hh"

// helpers
#include "ORBTools.hh"

#include "dadi/Logging/ConsoleChannel.hh"
#include "dadi/Logging/Logger.hh"
#include "dadi/Logging/Message.hh"


using namespace std;

LastPing::LastPing()
{
  this->name = NULL;
  this->time.sec = 0;
  this->time.msec = 0;
  this->timeDifference.sec = 0;
  this->timeDifference.msec = 0;
}
LastPing::LastPing(LastPing& newLastPing)
{
  this->name = strdup(newLastPing.name);
  this->time = newLastPing.time;
  this->timeDifference = newLastPing.timeDifference;
}
LastPing::~LastPing()
{
  if (this->name != NULL) {
    free(this->name);
  }
}
LastPing&
LastPing::operator=(LastPing& newLastPing)
{
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
  ComponentList* componentList,
  FilterManagerInterface* filterManager,
  TimeBuffer* timeBuffer)
{
  this->mcomponentList = componentList;
  this->mfilterManager = filterManager;
  this->mtimeBuffer = timeBuffer;
  this->mlastPings = new LastPings();
  this->maliveCheckThread = new AliveCheckThread(this);
  this->maliveCheckThread->startThread();
}

LogCentralComponent_impl::~LogCentralComponent_impl()
{
  delete this->mlastPings;
  this->maliveCheckThread->stopThread();  // stop and (automatically) delete the thread
}


CORBA::Short
LogCentralComponent_impl::connectComponent(
  char*& componentName,
  const char* componentHostname,
  const char* message,
  const char* compConfigurator,
  const log_time_t& componentTime,
  tag_list_t& initialConfig)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);

  if (strcmp(componentName, "*") == 0) {
    logger->log(dadi::Message("LCC",
                              "Bad name componnent. Cannot connect component. \n",
                              dadi::Message::PRIO_DEBUG));
    return LS_COMPONENT_CONNECT_BADNAME;
  }
  ComponentConfigurator_ptr compoConf = ORBMgr::getMgr()->resolve
    <ComponentConfigurator,ComponentConfigurator_ptr>(LOGCOMPCONFCTXT, compConfigurator);

  if (CORBA::is_nil(compoConf)) {
    logger->log(dadi::Message("LCC",
                              "Bad component configurator. \n",
                              dadi::Message::PRIO_DEBUG));
    return LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR;
  }

  // Put this here to be synchronised
  ComponentList::Iterator* it = mcomponentList->getIterator();
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
	it->getCurrentRef()->componentConfigurator->test();
      }
      catch (...) {
	lost = true;
      }

    if (!lost) {
    logger->log(dadi::Message("LCC",
                              string("Connexion failed because component "+string(componentName)+" already exists"),
                              dadi::Message::PRIO_DEBUG));
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
    msg = (char*) malloc(sizeof(char)*(63 + strlen(componentName)));
    sprintf(msg, "Component name %s already exists, but component seems to be down", componentName);
    disconnectComponent(componentName, msg);
  }

  it = mcomponentList->getIterator();
  it->reset();
  it->insertBeforeRef(ce);

  // Add the last ping
  log_time_t localTime = getLocalTime();
  LastPing lp;
  lp.name = strdup(componentName);
  lp.time = localTime;
  lp.timeDifference.sec = localTime.sec - componentTime.sec;
  lp.timeDifference.msec = localTime.msec - componentTime.msec;
  this->mlastPings->push(&lp);
  // Notify the FilterManager
  tag_list_t* tl = this->mfilterManager->componentConnect(
    (const char*)componentName, it);
  if (tl == NULL) {
    logger->log(dadi::Message("LCC",
                              "Connecting component failed after filter \n",
                              dadi::Message::PRIO_DEBUG));
    delete(it);
    return LS_COMPONENT_CONNECT_INTERNALERROR;
  }
  delete it;

  // Create a new IN message
  char* msg;
  msg = (char*) malloc(strlen(message) + strlen(componentHostname) + 2);
  sprintf(msg, "%s %s\0", message , componentHostname);
  log_msg_t* inmsg = new log_msg_t();
  inmsg->componentName = CORBA::string_dup(componentName);
  inmsg->time = localTime;
  inmsg->tag = CORBA::string_dup("IN");
  inmsg->msg = CORBA::string_dup(msg);
  this->mtimeBuffer->put(inmsg);
  delete inmsg;
  // Return the initialConfig
  // update the tag_list_t&
  initialConfig = *tl;
  logger->log(dadi::Message("LCC",
                            "Connection of component '" + string(componentName) + "' with message" + " '" + message,
                            dadi::Message::PRIO_DEBUG));
  return LS_OK;
}

CORBA::Short
LogCentralComponent_impl::disconnectComponent(const char* componentName,
                                              const char* message)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);
  // Find the component to delete it
  ComponentList::Iterator* it = this->mcomponentList->getIterator();
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
  logger->log(dadi::Message("LCC",
                            "Disconnection of component '" + string(componentName) + "' failed because it does not exist",
                            dadi::Message::PRIO_DEBUG));
    return LS_COMPONENT_DISCONNECT_NOTEXISTS;
  }
  // Remove the component
  it->removeCurrent();
  // Migrate it to a ReadItarator
  ComponentList::ReadIterator* readIterator =
    this->mcomponentList->reduceWriteIterator(it);

  // Notify the FilterManager
  this->mfilterManager->componentDisconnect(componentName, readIterator);

  // Remove the last ping
  LastPings::Iterator* it2 = this->mlastPings->getIterator();
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
  this->mtimeBuffer->put(inmsg);
  delete inmsg;

  logger->log(dadi::Message("LCC",
                            "Disconnection of component '" + string(componentName) + "' with message "+message,
                            dadi::Message::PRIO_DEBUG));
  return LS_OK;
}

void
LogCentralComponent_impl::sendBuffer(const log_msg_buf_t& buffer)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);
  // for each message, correction of its time and the message is sent to the
  // TimeBuffer.
  log_time_t td;

  LastPing* lp = NULL;
  if (buffer.length() != 0) {
    const char* name = buffer[0].componentName;
    LastPings::ReadIterator* it = this->mlastPings->getReadIterator();

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
      logger->log(dadi::Message("LCC",
                                "Discarded messageBuffer from unknown component " + string(name),
                                dadi::Message::PRIO_DEBUG));
      return;
    }

    for (unsigned int i = 0 ; i < buffer.length() ; i++) {
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
      this->mtimeBuffer->put(&msg);
    }
  }
}

bool
LogCentralComponent_impl::isComponentExists(const char* name,
                            ComponentList::ReadIterator* it)
{
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
LogCentralComponent_impl::ping(const char* componentName)
{
  // Find the component
  LastPings::Iterator* it = this->mlastPings->getIterator();
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
                                      const log_time_t& componentTime)
{
  log_time_t localTime = getLocalTime();
  // Find the component
  LastPings::Iterator* it = this->mlastPings->getIterator();
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
                            ComponentList::ReadIterator* it)
{
  unsigned int num = 1;
  char* ret = new char[strlen(hostname) + 10]; // I think it's sufficient
  sprintf(ret, "%s_%u", hostname, num);
  while (isComponentExists(ret, it)) {
    num++;
    sprintf(ret, "%s_%u", hostname, num);
  }
  return strdup(ret);
}

/****************************************************************************
 * AliveCheckThread implementation
 ****************************************************************************/

LogCentralComponent_impl::AliveCheckThread::AliveCheckThread(
  LogCentralComponent_impl* LCC):
LCC(LCC),
threadRunning(false)
{
}

LogCentralComponent_impl::AliveCheckThread::~AliveCheckThread()
{
  if (this->threadRunning) {
    this->stopThread();
  }
}

void
LogCentralComponent_impl::AliveCheckThread::startThread()
{
  if (this->threadRunning) {
    return;
  }
  this->threadRunning = true;
  start_undetached();
}

void
LogCentralComponent_impl::AliveCheckThread::stopThread()
{
  if (!this->threadRunning) {
    return;
  }
  this->threadRunning = false;
  join(NULL);
}

void*
LogCentralComponent_impl::AliveCheckThread::run_undetached(void* params)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);
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
    LastPings::ReadIterator* it = this->LCC->mlastPings->getReadIterator();
    while (it->hasCurrent()) {
      lp = it->nextRef();
      if ((lp->time.sec < checkTime.sec)
        || ((lp->time.sec == checkTime.sec)
          && (lp->time.msec < checkTime.msec))) { // ping too old
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
        msg = (char*) malloc(strlen(s) + strlen("Ping Timeout"));
        logger->log(dadi::Message("LCC",
                                  "Ping Timeout of '" + string(s) + "' : disconnect the component.\n",
                                  dadi::Message::PRIO_DEBUG));
        sprintf(msg,"%s Ping Timeout",s);
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


void LogCentralComponent_impl::test (){
  return;
}

