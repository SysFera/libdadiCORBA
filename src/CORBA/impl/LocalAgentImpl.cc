
#include "LocalAgentImpl.hh"
#include "debug.hh"
#include "ORBMgr.hh"

#include <iostream>
using namespace std;

/** The trace level. */
extern unsigned int TRACE_LEVEL;

#define LA_TRACE_FUNCTION(formatted_text)               \
  TRACE_TEXT(TRACE_ALL_STEPS, "LA::");                  \
  TRACE_FUNCTION(TRACE_ALL_STEPS, formatted_text)


LocalAgentFwdrImpl::LocalAgentFwdrImpl(Forwarder_ptr fwdr,
                                       const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

CORBA::Long
LocalAgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                                   const SeqCorbaProfileDesc_t& services) {
  return forwarder->agentSubscribe(me, hostName, services, objName);
}

CORBA::Long
LocalAgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                                    const SeqCorbaProfileDesc_t& services) {
  return forwarder->serverSubscribe(me, hostName, services, objName);
}

CORBA::Long
LocalAgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                     const SeqCorbaProfileDesc_t& services) {
  return forwarder->childUnsubscribe(childID, services, objName);
}

CORBA::Long
LocalAgentFwdrImpl::bindParent(const char * parentName) {
  return forwarder->bindParent(parentName, objName);
}

CORBA::Long LocalAgentFwdrImpl::disconnect() {
  return forwarder->disconnect(objName);
}

//CORBA::Long LocalAgentFwdrImpl::removeElement(bool recursive) {
//  return forwarder->removeElement(objName);
//}

SeqString*
LocalAgentFwdrImpl::searchData(const char* request) {
  return forwarder->searchData(request, objName);
}

void
LocalAgentFwdrImpl::getRequest(const corba_request_t& req) {
  forwarder->getRequest(req, objName);
}

CORBA::Long
LocalAgentFwdrImpl::addServices(CORBA::ULong myID,
                                const SeqCorbaProfileDesc_t& services) {
  return forwarder->addServices(myID, services, objName);
}

CORBA::Long
LocalAgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                       const corba_profile_desc_t& profile) {
  return forwarder->childRemoveService(childID, profile, objName);
}

char*
LocalAgentFwdrImpl::getDataManager() {
  return forwarder->getDataManager(objName);
}

void
LocalAgentFwdrImpl::getResponse(const corba_response_t& resp) {
  forwarder->getResponse(resp, objName);
}

CORBA::Long
LocalAgentFwdrImpl::ping() {
  return forwarder->ping(objName);
}

char*
LocalAgentFwdrImpl::getHostname() {
  return forwarder->getHostname(objName);
}
