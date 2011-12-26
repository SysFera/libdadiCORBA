/**
 * @file AgentImpl.hh
 *
 * @brief  DIET agent implementation source code
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */


#ifndef _AGENTIMPL_HH_
#define _AGENTIMPL_HH_

#include "Agent.hh"
#include "LocalAgent.hh"


#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif


// Forwarder part
#include "Forwarder.hh"
#include "AgentFwdr.hh"


/**
 * @brief The agent forwarder class that implements all the agents methods
 * throught the forwarder
 * For non documented methods, please see the Agent and its parents idl interfaces.
 * @class AgentFwdrImpl
 */
class AgentFwdrImpl : public POA_AgentFwdr,
                      public PortableServer::RefCountServantBase {
public:
  AgentFwdrImpl(Forwarder_ptr fwdr, const char* objName);

  virtual CORBA::Long
  agentSubscribe(const char* me, const char* hostName,
                 const SeqCorbaProfileDesc_t& services);

  virtual CORBA::Long
  serverSubscribe(const char* me, const char* hostName,
                  const SeqCorbaProfileDesc_t& services);
  virtual CORBA::Long
  childUnsubscribe(CORBA::ULong childID,
                   const SeqCorbaProfileDesc_t& services);

  virtual CORBA::Long
  removeElement(bool recursive);
  CORBA::Long
  bindParent(const char* parent);

  CORBA::Long
  disconnect();


  virtual CORBA::Long
  childRemoveService(CORBA::ULong childID,
                     const corba_profile_desc_t& profile);

  virtual char*
  getDataManager();

  SeqString*
  searchData(const char* request);

  virtual CORBA::Long
  addServices(CORBA::ULong myID, const SeqCorbaProfileDesc_t& services);

  virtual void
  getResponse(const corba_response_t& resp);

  virtual CORBA::Long
  ping();

  virtual char*
  getHostname();

protected:
  Forwarder_ptr mforwarder;
  char* mobjName;
};
#endif  // _AGENTIMPL_HH_
