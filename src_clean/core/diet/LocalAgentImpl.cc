
#include "LocalAgentImpl.hh"
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
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

CORBA::Long
LocalAgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                                   const SeqCorbaProfileDesc_t& services) {
  return mforwarder->agentSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                                    const SeqCorbaProfileDesc_t& services) {
  return mforwarder->serverSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                     const SeqCorbaProfileDesc_t& services) {
  return mforwarder->childUnsubscribe(childID, services, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::bindParent(const char * parentName) {
  return mforwarder->bindParent(parentName, mobjName);
}

CORBA::Long LocalAgentFwdrImpl::disconnect() {
  return mforwarder->disconnect(mobjName);
}

CORBA::Long LocalAgentFwdrImpl::removeElement(bool recursive) {
  return mforwarder->removeElement(recursive, mobjName);
}

SeqString*
LocalAgentFwdrImpl::searchData(const char* request) {
  return mforwarder->searchData(request, mobjName);
}

void
LocalAgentFwdrImpl::getRequest(const corba_request_t& req) {
  mforwarder->getRequest(req, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::addServices(CORBA::ULong myID,
                                const SeqCorbaProfileDesc_t& services) {
  return mforwarder->addServices(myID, services, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                       const corba_profile_desc_t& profile) {
  return mforwarder->childRemoveService(childID, profile, mobjName);
}

char*
LocalAgentFwdrImpl::getDataManager() {
  return mforwarder->getDataManager(mobjName);
}

void
LocalAgentFwdrImpl::getResponse(const corba_response_t& resp) {
  mforwarder->getResponse(resp, mobjName);
}

CORBA::Long
LocalAgentFwdrImpl::ping() {
  return mforwarder->ping(mobjName);
}

char*
LocalAgentFwdrImpl::getHostname() {
  return mforwarder->getHostname(mobjName);
}
