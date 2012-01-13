/**
 * @file MasterAgentImpl.hh
 *
 * @brief  DIET master agent implementation source code
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#include <cstdio>
#include <iostream>

#include "MasterAgentImpl.hh"
//#include "debug.hh"

#include "dadi/Logging/ConsoleChannel.hh"
#include "dadi/Logging/Logger.hh"
#include "dadi/Logging/Message.hh"


#include "ORBMgr.hh"


/*
  Workflow utilities header
*/

omni_mutex reqCount_mutex;

//#define aff_val(x)
#define aff_val(x) cout << #x << " = " << x << "\n";

//#define MA_TRACE_FUNCTION(formatted_text)               \
//  TRACE_TEXT(TRACE_ALL_STEPS, "MA::");                  \
//  TRACE_FUNCTION(TRACE_ALL_STEPS, formatted_text)


MasterAgentFwdrImpl::MasterAgentFwdrImpl(Forwarder_ptr fwdr,
                                         const char* objName) {
// Init logger
  mlogger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  mlogger->setLevel(dadi::Message::PRIO_TRACE);
  mcc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  mlogger->setChannel(mcc);

  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

corba_response_t*
MasterAgentFwdrImpl::submit(const corba_pb_desc_t& pb_profile,
                            CORBA::ULong maxServers) {
  mlogger->log(dadi::Message("ORBMgr",
                             std::string(__FILE__) + ": "
                             + " (" + __FUNCTION__ + ")"
                             + "submit("
                             + std::string(mobjName) + ")\n",
                             dadi::Message::PRIO_DEBUG));

  return mforwarder->submit(pb_profile, maxServers, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::get_session_num() {
  return mforwarder->get_session_num(mobjName);
}

char*
MasterAgentFwdrImpl::get_data_id() {
  return mforwarder->get_data_id(mobjName);
}

CORBA::ULong
MasterAgentFwdrImpl::dataLookUp(const char* argID) {
  return mforwarder->dataLookUp(argID, mobjName);
}

corba_data_desc_t*
MasterAgentFwdrImpl::get_data_arg(const char* argID) {
  return mforwarder->get_data_arg(argID, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::diet_free_pdata(const char *argID) {
  return mforwarder->diet_free_pdata(argID, mobjName);
}

SeqCorbaProfileDesc_t*
MasterAgentFwdrImpl::getProfiles(CORBA::Long& length) {
  return mforwarder->getProfiles(length, mobjName);
}

CORBA::Boolean
MasterAgentFwdrImpl::handShake(const char* name,
                               const char* myName) {
  return mforwarder->handShake(name, myName, mobjName);
}

char*
MasterAgentFwdrImpl::getBindName() {
  return mforwarder->getBindName(mobjName);
}

void
MasterAgentFwdrImpl::searchService(const char* predecessor,
                                   const char* predecessorId,
                                   const corba_request_t& request) {
  mforwarder->searchService(predecessor, predecessorId, request, mobjName);
}

void
MasterAgentFwdrImpl::stopFlooding(CORBA::Long reqId,
                                  const char* senderId) {
  mforwarder->stopFlooding(reqId, senderId, mobjName);
}

void
MasterAgentFwdrImpl::serviceNotFound(CORBA::Long reqId,
                                     const char* senderId) {
  mforwarder->serviceNotFound(reqId, senderId, mobjName);
}

void
MasterAgentFwdrImpl::newFlood(CORBA::Long reqId,
                              const char* senderId) {
  mforwarder->newFlood(reqId, senderId, mobjName);
}

void
MasterAgentFwdrImpl::floodedArea(CORBA::Long reqId,
                                 const char* senderId) {
  mforwarder->floodedArea(reqId, senderId, mobjName);
}

void
MasterAgentFwdrImpl::alreadyContacted(CORBA::Long reqId,
                                      const char* senderId) {
  mforwarder->alreadyContacted(reqId, senderId, mobjName);
}

void
MasterAgentFwdrImpl::serviceFound(CORBA::Long reqId,
                                  const corba_response_t& decision) {
  mforwarder->serviceFound(reqId, decision, mobjName);
}
wf_response_t*
MasterAgentFwdrImpl::submit_pb_set(const corba_pb_desc_seq_t& seq_pb) {
  return mforwarder->submit_pb_set(seq_pb, mobjName);
}

response_seq_t*
MasterAgentFwdrImpl::submit_pb_seq(const corba_pb_desc_seq_t& pb_seq,
                                   CORBA::Long reqCount,
                                   CORBA::Boolean& complete,
                                   CORBA::Long& firstReqId,
                                   CORBA::Long& seqReqId) {
  return mforwarder->submit_pb_seq(pb_seq, reqCount, complete,
                                  firstReqId, seqReqId, mobjName);
}

SeqString* MasterAgentFwdrImpl::searchData(const char* request) {
  return mforwarder->searchData(request, mobjName);
}

CORBA::Long MasterAgentFwdrImpl::insertData(const char* key,
                                            const SeqString& values) {
  return mforwarder->insertData(key, values, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::agentSubscribe(const char* me, const char* hostName,
                                    const SeqCorbaProfileDesc_t& services) {
  return mforwarder->agentSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::serverSubscribe(const char* me, const char* hostName,
                                     const SeqCorbaProfileDesc_t& services) {
  return mforwarder->serverSubscribe(me, hostName, services, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::childUnsubscribe(CORBA::ULong childID,
                                      const SeqCorbaProfileDesc_t& services) {
  return mforwarder->childUnsubscribe(childID, services, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::bindParent(const char * parentName) {
  return mforwarder->bindParent(parentName, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::disconnect() {
  return mforwarder->disconnect(mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::removeElement(bool recursive) {
  return mforwarder->removeElement(recursive, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::addServices(CORBA::ULong myID,
                                 const SeqCorbaProfileDesc_t& services) {
  return mforwarder->addServices(myID, services, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::childRemoveService(CORBA::ULong childID,
                                        const corba_profile_desc_t& profile) {
  return mforwarder->childRemoveService(childID, profile, mobjName);
}

char*
MasterAgentFwdrImpl::getDataManager() {
  return mforwarder->getDataManager(mobjName);
}

void
MasterAgentFwdrImpl::getResponse(const corba_response_t& resp) {
  mforwarder->getResponse(resp, mobjName);
}

CORBA::Long
MasterAgentFwdrImpl::ping() {
  return mforwarder->ping(mobjName);
}

char*
MasterAgentFwdrImpl::getHostname() {
  return mforwarder->getHostname(mobjName);
}

