/**
 * @file Agent.cc
 *
 * @brief  DIET forwarder implementation - Agent forwarder implementation
 *
 * @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "CorbaForwarder.hh"
#include "ORBMgr.hh"
#include <string>
#include <iostream>
#include <stdio.h>

::CORBA::Long
CorbaForwarder::agentSubscribe(const char* agentName,
                              const char* hostname,
                              const ::SeqCorbaProfileDesc_t& services,
                              const char* objName)
{
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->agentSubscribe(agentName, hostname,
                                     services, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->agentSubscribe(agentName, hostname, services);
}

::CORBA::Long
CorbaForwarder::serverSubscribe(const char* seDName, const char* hostname,
                               const ::SeqCorbaProfileDesc_t& services,
                               const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->serverSubscribe(seDName, hostname,
                                      services, objString.c_str());
  }
  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->serverSubscribe(seDName, hostname, services);
  }

::CORBA::Long
CorbaForwarder::childUnsubscribe(::CORBA::ULong childID,
                                const ::SeqCorbaProfileDesc_t& services,
                                const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->childUnsubscribe(childID, services, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->childUnsubscribe(childID, services);
  }

::CORBA::Long
CorbaForwarder::childRemoveService(::CORBA::ULong childID,
                                  const ::corba_profile_desc_t& profile,
                                  const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->childRemoveService(childID, profile, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->childRemoveService(childID, profile);
  }

::CORBA::Long
CorbaForwarder::addServices(::CORBA::ULong myID,
                           const ::SeqCorbaProfileDesc_t& services,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->addServices(myID, services, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->addServices(myID, services);
  }

void
CorbaForwarder::getResponse(const ::corba_response_t& resp,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getResponse(resp, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->getResponse(resp);
}

char*
CorbaForwarder::getDataManager(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getDataManager(objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->getDataManager();
}

SeqString*
CorbaForwarder::searchData(const char* request,
                          const char* objName)
{
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->searchData(request, objString.c_str());
  }

  name = getName(objString);

  Agent_var agent =
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->mname);
  return agent->searchData(request);
}
