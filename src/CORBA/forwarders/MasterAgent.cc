/**
* @file MasterAgent.cc
*
* @brief  DIET forwarder implementation - MA DAG forwarder implementation
*
* @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.5  2010/11/24 15:12:56  bdepardo
 * searchData is now available on all agents. SeDs are now able to retrieve
 * a DAGDA data from an alias specified by a client.
 * Currently a SeD cannot declare an alias.
 *
 * Revision 1.4  2010/07/27 16:16:49  glemahec
 * Forwarders robustness
 *
 * Revision 1.3  2010/07/14 23:45:30  bdepardo
 * Header corrections
 *
 * Revision 1.2  2010/07/13 15:24:13  glemahec
 * Warnings corrections and some robustness improvements
 *
 * Revision 1.1  2010/07/12 16:08:56  glemahec
 * DIET 2.5 beta 1 - Forwarder implementations
 ****************************************************************************/

#include "CorbaForwarder.hh"
#include "ORBMgr.hh"
#include "debug.hh"

#include <string>
#include <iostream>

corba_response_t*
CorbaForwarder::submit(const ::corba_pb_desc_t& pb_profile,
                      ::CORBA::ULong maxServers,
                      const char* objName) {
  TRACE_TEXT(TRACE_STRUCTURES, __FILE__
             << ": l." << __LINE__ << " (" << __FUNCTION__ << ")\n");
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    TRACE_TEXT(TRACE_MAIN_STEPS,
               "Forwarder remote call submit(pb_profile, "
               << maxServers << ", " << maxServers
               << ", " << objString << ")\n");
    return getPeer()->submit(pb_profile, maxServers, objString.c_str());
  }

  name = getName(objString);

  TRACE_TEXT(TRACE_MAIN_STEPS, "Forwarder local object call: \n");
  TRACE_TEXT(TRACE_MAIN_STEPS,
             "\t- Resolve " << name << " from this forwarder ("
             << this->name << ")\n");

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->submit(pb_profile, maxServers);
}

::CORBA::Long
CorbaForwarder::get_session_num(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->get_session_num(objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->get_session_num();
}

char*
CorbaForwarder::get_data_id(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->get_data_id(objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->get_data_id();
}

::CORBA::ULong
CorbaForwarder::dataLookUp(const char* id, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->dataLookUp(id, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->dataLookUp(id);
}

corba_data_desc_t*
CorbaForwarder::get_data_arg(const char* argID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->get_data_arg(argID, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->get_data_arg(argID);
}

::CORBA::Long
CorbaForwarder::diet_free_pdata(const char* argID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->diet_free_pdata(argID, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->diet_free_pdata(argID);
}

SeqCorbaProfileDesc_t*
CorbaForwarder::getProfiles(::CORBA::Long& length, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getProfiles(length, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->getProfiles(length);
}

#ifdef HAVE_WORKFLOW
wf_response_t*
CorbaForwarder::submit_pb_set(const ::corba_pb_desc_seq_t& seq_pb,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->submit_pb_set(seq_pb, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->submit_pb_set(seq_pb);
}

response_seq_t*
CorbaForwarder::submit_pb_seq(const ::corba_pb_desc_seq_t& pb_seq,
                             ::CORBA::Long reqCount,
                             ::CORBA::Boolean& complete,
                             ::CORBA::Long& firstReqId,
                             ::CORBA::Long& seqReqId,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->submit_pb_seq(pb_seq, reqCount, complete, firstReqId,
                                    seqReqId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->submit_pb_seq(pb_seq, reqCount, complete, firstReqId, seqReqId);
}
#endif

::CORBA::Long
CorbaForwarder::insertData(const char* key,
                          const ::SeqString& values,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->insertData(key, values, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->insertData(key, values);
}

#ifdef HAVE_MULTI_MA
::CORBA::Boolean
CorbaForwarder::handShake(const char* masterAgentName,
                         const char* myName,
                         const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->handShake(masterAgentName, myName, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->handShake(masterAgentName, myName);
}

char*
CorbaForwarder::getBindName(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getBindName(objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->getBindName();
}

void
CorbaForwarder::searchService(const char* masterAgentName,
                             const char* myName,
                             const ::corba_request_t& request,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->searchService(masterAgentName, myName,
                                    request, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->searchService(masterAgentName, myName, request);
}

void
CorbaForwarder::stopFlooding(::CORBA::Long reqId,
                            const char* senderId,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->stopFlooding(reqId, senderId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->stopFlooding(reqId, senderId);
}

void
CorbaForwarder::serviceNotFound(::CORBA::Long reqId,
                               const char* senderId,
                               const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->serviceNotFound(reqId, senderId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->serviceNotFound(reqId, senderId);
}

void
CorbaForwarder::newFlood(::CORBA::Long reqId,
                        const char* senderId,
                        const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->newFlood(reqId, senderId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->newFlood(reqId, senderId);
}

void
CorbaForwarder::floodedArea(::CORBA::Long reqId,
                           const char* senderId,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->floodedArea(reqId, senderId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->floodedArea(reqId, senderId);
}

void
CorbaForwarder::alreadyContacted(::CORBA::Long reqId,
                                const char* senderId,
                                const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->alreadyContacted(reqId, senderId, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->alreadyContacted(reqId, senderId);
}

void
CorbaForwarder::serviceFound(::CORBA::Long reqId,
                            const ::corba_response_t& decision,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->serviceFound(reqId, decision, objString.c_str());
  }

  name = getName(objString);

  MasterAgent_var agent =
    ORBMgr::getMgr()->resolve<MasterAgent, MasterAgent_var>(AGENTCTXT,
                                                            name,
                                                            this->name);
  return agent->serviceFound(reqId, decision);
}
#endif
