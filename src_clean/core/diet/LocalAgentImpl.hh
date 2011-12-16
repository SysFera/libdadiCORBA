/**
 * @file  LocalAgentImpl.hh
 *
 * @brief  DIET local agent implementation header
 *
 * @author - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
 *         - Sylvain DAHAN (Sylvain.Dahan@lifc.univ-fcomte.fr)
 *         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _LOCALAGENTIMPL_HH_
#define _LOCALAGENTIMPL_HH_

#include "Agent.hh"

#include "Forwarder.hh"
#include "LocalAgentFwdr.hh"


/**
 * @brief The Local Agent forwarder class that implements all the agents methods
 * throught the forwarder
 * @class LocalAgentImpl
 * For non documented methods, please see the \ref LocalAgentIDL and its parents idl interfaces.
 */
class LocalAgentFwdrImpl : public POA_LocalAgentFwdr,
                           public PortableServer::RefCountServantBase {
public:
  LocalAgentFwdrImpl(Forwarder_ptr fwdr, const char* objName);

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
  bindParent(const char * parentName);

  CORBA::Long
  disconnect();

  virtual void
  getRequest(const corba_request_t& req);

  virtual void
  getResponse(const corba_response_t& resp);

  virtual CORBA::Long
  ping();

  virtual char*
  getHostname();

  virtual CORBA::Long
  addServices(CORBA::ULong myID, const SeqCorbaProfileDesc_t& services);

  virtual CORBA::Long
  childRemoveService(CORBA::ULong childID,
                     const corba_profile_desc_t& profile);

  virtual char*
  getDataManager();

  virtual SeqString*
  searchData(const char* request);

private:
  Forwarder_ptr mforwarder;
  char* mobjName;
};

#endif  // _LOCALAGENTIMPL_HH_
