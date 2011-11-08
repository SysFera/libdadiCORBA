
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
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

CORBA::Long
AgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                              const SeqCorbaProfileDesc_t& services) {
  return forwarder->agentSubscribe(me, hostName, services, objName);
}

CORBA::Long
AgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                               const SeqCorbaProfileDesc_t& services) {
  return forwarder->serverSubscribe(me, hostName, services, objName);
}

CORBA::Long
AgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                const SeqCorbaProfileDesc_t& services) {
  return forwarder->childUnsubscribe(childID, services, objName);
}

//CORBA::Long AgentFwdrImpl::removeElement(bool recursive) {
//  return forwarder->removeElement(recursive, objName);
//}

CORBA::Long
AgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                  const corba_profile_desc_t& profile) {
  return forwarder->childRemoveService(childID, profile, objName);
}

char* AgentFwdrImpl::getDataManager() {
  return forwarder->getDataManager(objName);
}

SeqString*
AgentFwdrImpl::searchData(const char* request) {
  return forwarder->searchData(request, objName);
}

CORBA::Long
AgentFwdrImpl::addServices(CORBA::ULong myID,
                           const SeqCorbaProfileDesc_t& services) {
  return forwarder->addServices(myID, services, objName);
}

void AgentFwdrImpl::getResponse(const corba_response_t& resp) {
  forwarder->getResponse(resp, objName);
}

CORBA::Long AgentFwdrImpl::ping() {
  return forwarder->ping(objName);
}

char* AgentFwdrImpl::getHostname() {
  return forwarder->getHostname(objName);
}
