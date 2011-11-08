
#include <cstdio>
#include <iostream>

#include "MasterAgentImpl.hh"
#include "debug.hh"

#ifdef HAVE_MULTI_MA
#include "ORBMgr.hh"
#endif


#ifdef HAVE_WORKFLOW
/*
  Workflow utilities header
*/

omni_mutex reqCount_mutex;

#endif /* HAVE_WORKFLOW */

//#define aff_val(x)
#define aff_val(x) cout << #x << " = " << x << "\n";

#define MA_TRACE_FUNCTION(formatted_text)               \
  TRACE_TEXT(TRACE_ALL_STEPS, "MA::");                  \
  TRACE_FUNCTION(TRACE_ALL_STEPS, formatted_text)


MasterAgentFwdrImpl::MasterAgentFwdrImpl(Forwarder_ptr fwdr,
                                         const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

corba_response_t*
MasterAgentFwdrImpl::submit(const corba_pb_desc_t& pb_profile,
                            CORBA::ULong maxServers) {
  TRACE_TEXT(TRACE_MAIN_STEPS, __FILE__ << ": l." << __LINE__
             << " (" << __FUNCTION__ << ")" << std::endl
             << "submit(pb_profile, " << maxServers << ", "
             << objName << ")" << std::endl);
  return forwarder->submit(pb_profile, maxServers, objName);
}

CORBA::Long
MasterAgentFwdrImpl::get_session_num() {
  return forwarder->get_session_num(objName);
}

char*
MasterAgentFwdrImpl::get_data_id() {
  return forwarder->get_data_id(objName);
}

CORBA::ULong
MasterAgentFwdrImpl::dataLookUp(const char* argID) {
  return forwarder->dataLookUp(argID, objName);
}

corba_data_desc_t*
MasterAgentFwdrImpl::get_data_arg(const char* argID) {
  return forwarder->get_data_arg(argID, objName);
}

CORBA::Long
MasterAgentFwdrImpl::diet_free_pdata(const char *argID) {
  return forwarder->diet_free_pdata(argID, objName);
}

SeqCorbaProfileDesc_t*
MasterAgentFwdrImpl::getProfiles(CORBA::Long& length) {
  return forwarder->getProfiles(length, objName);
}

#ifdef HAVE_MULTI_MA
CORBA::Boolean
MasterAgentFwdrImpl::handShake(const char* name,
                               const char* myName) {
  return forwarder->handShake(name, myName, objName);
}

char*
MasterAgentFwdrImpl::getBindName() {
  return forwarder->getBindName(objName);
}

void
MasterAgentFwdrImpl::searchService(const char* predecessor,
                                   const char* predecessorId,
                                   const corba_request_t& request) {
  forwarder->searchService(predecessor, predecessorId, request, objName);
}

void
MasterAgentFwdrImpl::stopFlooding(CORBA::Long reqId,
                                  const char* senderId) {
  forwarder->stopFlooding(reqId, senderId, objName);
}

void
MasterAgentFwdrImpl::serviceNotFound(CORBA::Long reqId,
                                     const char* senderId) {
  forwarder->serviceNotFound(reqId, senderId, objName);
}

void
MasterAgentFwdrImpl::newFlood(CORBA::Long reqId,
                              const char* senderId) {
  forwarder->newFlood(reqId, senderId, objName);
}

void
MasterAgentFwdrImpl::floodedArea(CORBA::Long reqId,
                                 const char* senderId) {
  forwarder->floodedArea(reqId, senderId, objName);
}

void
MasterAgentFwdrImpl::alreadyContacted(CORBA::Long reqId,
                                      const char* senderId) {
  forwarder->alreadyContacted(reqId, senderId, objName);
}

void
MasterAgentFwdrImpl::serviceFound(CORBA::Long reqId,
                                  const corba_response_t& decision) {
  forwarder->serviceFound(reqId, decision, objName);
}
#endif /* HAVE_MULTI_MA */
#ifdef HAVE_WORKFLOW
wf_response_t*
MasterAgentFwdrImpl::submit_pb_set(const corba_pb_desc_seq_t& seq_pb) {
  return forwarder->submit_pb_set(seq_pb, objName);
}

response_seq_t*
MasterAgentFwdrImpl::submit_pb_seq(const corba_pb_desc_seq_t& pb_seq,
                                   CORBA::Long reqCount,
                                   CORBA::Boolean& complete,
                                   CORBA::Long& firstReqId,
                                   CORBA::Long& seqReqId) {
  return forwarder->submit_pb_seq(pb_seq, reqCount, complete,
                                  firstReqId, seqReqId, objName);
}
#endif /* HAVE_WORKFLOW */

SeqString* MasterAgentFwdrImpl::searchData(const char* request) {
  return forwarder->searchData(request, objName);
}

CORBA::Long MasterAgentFwdrImpl::insertData(const char* key,
                                            const SeqString& values) {
  return forwarder->insertData(key, values, objName);
}

CORBA::Long
MasterAgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                                    const SeqCorbaProfileDesc_t& services) {
  return forwarder->agentSubscribe(me, hostName, services, objName);
}

CORBA::Long
MasterAgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                                     const SeqCorbaProfileDesc_t& services) {
  return forwarder->serverSubscribe(me, hostName, services, objName);
}

CORBA::Long
MasterAgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                      const SeqCorbaProfileDesc_t& services) {
  return forwarder->childUnsubscribe(childID, services, objName);
}

CORBA::Long
MasterAgentFwdrImpl::bindParent(const char * parentName) {
  return forwarder->bindParent(parentName, objName);
}

CORBA::Long
MasterAgentFwdrImpl::disconnect() {
  return forwarder->disconnect(objName);
}

//CORBA::Long
//MasterAgentFwdrImpl::removeElement(bool recursive) {
//  return forwarder->removeElement(objName);
//}

CORBA::Long
MasterAgentFwdrImpl::addServices(CORBA::ULong myID,
                                 const SeqCorbaProfileDesc_t& services) {
  return forwarder->addServices(myID, services, objName);
}

CORBA::Long
MasterAgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                        const corba_profile_desc_t& profile) {
  return forwarder->childRemoveService(childID, profile, objName);
}

char*
MasterAgentFwdrImpl::getDataManager() {
  return forwarder->getDataManager(objName);
}

void
MasterAgentFwdrImpl::getResponse(const corba_response_t& resp) {
  forwarder->getResponse(resp, objName);
}

CORBA::Long
MasterAgentFwdrImpl::ping() {
  return forwarder->ping(objName);
}

char*
MasterAgentFwdrImpl::getHostname() {
  return forwarder->getHostname(objName);
}

