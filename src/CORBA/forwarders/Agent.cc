/**
* @file Agent.cc
*
* @brief  DIET forwarder implementation - Agent forwarder implementation
*
* @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.8  2011/02/24 16:56:20  bdepardo
 * Use new parser
 *
 * Revision 1.7  2010/12/17 09:47:59  kcoulomb
 * * Set diet to use the new log with forwarders
 * * Fix a CoRI problem
 * * Add library version remove DTM flag from ccmake because deprecated
 *
 * Revision 1.6  2010/11/24 15:12:56  bdepardo
 * searchData is now available on all agents. SeDs are now able to retrieve
 * a DAGDA data from an alias specified by a client.
 * Currently a SeD cannot declare an alias.
 *
 * Revision 1.5  2010/07/27 16:16:49  glemahec
 * Forwarders robustness
 *
 * Revision 1.4  2010/07/27 10:24:32  glemahec
 * Improve robustness & general performance
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
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
    ORBMgr::getMgr()->resolve<Agent, Agent_var>(AGENTCTXT, name, this->name);
  return agent->searchData(request);
}
