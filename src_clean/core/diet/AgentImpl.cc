
#include "AgentImpl.hh"

#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>  // For gethostname()

#include "common_types.hh"
#include "ORBMgr.hh"

/** The trace level. */
extern unsigned int TRACE_LEVEL;

#define AGT_TRACE_FUNCTION(formatted_text)              \
  TRACE_TEXT(TRACE_ALL_STEPS, "Agt::");                 \
  TRACE_FUNCTION(TRACE_ALL_STEPS, formatted_text)

#define MAX_HOSTNAME_LENGTH  256

// Forwarder part
AgentFwdrImpl::AgentFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

CORBA::Long
AgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                              const SeqCorbaProfileDesc_t& services) {
  return mforwarder->agentSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
AgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                               const SeqCorbaProfileDesc_t& services) {
  return mforwarder->serverSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
AgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                const SeqCorbaProfileDesc_t& services) {
  return mforwarder->childUnsubscribe(childID, services, mobjName);
}

CORBA::Long
AgentFwdrImpl::removeElement(bool recursive) {
  return mforwarder->removeElement(recursive, mobjName);
}

CORBA::Long
AgentFwdrImpl::bindParent(const char* parent) {
  return mforwarder->bindParent(parent, mobjName);
}
CORBA::Long
AgentFwdrImpl::disconnect() {
  return mforwarder->disconnect(mobjName);
}



CORBA::Long
AgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                  const corba_profile_desc_t& profile) {
  return mforwarder->childRemoveService(childID, profile, mobjName);
}

char* AgentFwdrImpl::getDataManager() {
  return mforwarder->getDataManager(mobjName);
}

SeqString*
AgentFwdrImpl::searchData(const char* request) {
  return mforwarder->searchData(request, mobjName);
}

CORBA::Long
AgentFwdrImpl::addServices(CORBA::ULong myID,
                           const SeqCorbaProfileDesc_t& services) {
  return mforwarder->addServices(myID, services, mobjName);
}

void AgentFwdrImpl::getResponse(const corba_response_t& resp) {
  mforwarder->getResponse(resp, mobjName);
}

CORBA::Long AgentFwdrImpl::ping() {
  return mforwarder->ping(mobjName);
}

char* AgentFwdrImpl::getHostname() {
  return mforwarder->getHostname(mobjName);
}
