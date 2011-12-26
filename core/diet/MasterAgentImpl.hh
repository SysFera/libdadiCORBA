/**
 * @file  MasterAgentImpl.hh
 *
 * @brief  DIET master agent implementation header
 *
 * @author - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
 *         - Sylvain DAHAN (Sylvain.Dahan@lifc.univ-fcomte.fr)
 *         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _MASTERAGENTIMPL_HH_
#define _MASTERAGENTIMPL_HH_

#include "MasterAgent.hh"
#include "AgentImpl.hh"
#include "utils/LinkedList.hh"

#if HAVE_MULTI_MA
#include "BindService.hh"
#include "KeyString.hh"
#include "ts_container/ts_set.hh"
class FloodRequestsList;
#endif

#include "Forwarder.hh"
#include "MasterAgentFwdr.hh"

#include "dadi/Logging/Logger.hh"


/**
 * @brief The master agent forwarder class that implements all the agents methods
 * throught the forwarder
 * @class MasterAgentFwdrImpl
 * For non documented methods, please see the MasterAgent and its parents idl interfaces.
 */
class MasterAgentFwdrImpl : public POA_MasterAgentFwdr,
                            public PortableServer::RefCountServantBase {
public:
  MasterAgentFwdrImpl(Forwarder_ptr fwdr, const char* objName);

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
  getResponse(const corba_response_t& resp);
  virtual CORBA::Long
  ping();

  virtual char*
  getHostname();

  virtual CORBA::Long
  addServices(CORBA::ULong myID, const SeqCorbaProfileDesc_t& services);

  corba_response_t*
  submit(const corba_pb_desc_t& pb_profile, CORBA::ULong maxServers);

  virtual CORBA::Long
  get_session_num();

  virtual char *
  get_data_id();

  virtual CORBA::ULong
  dataLookUp(const char* argID);

  virtual corba_data_desc_t*
  get_data_arg(const char* argID);

  virtual CORBA::Long
  diet_free_pdata(const char *argID);

  virtual SeqCorbaProfileDesc_t*
  getProfiles(CORBA::Long& length);

#ifdef HAVE_MULTI_MA
  virtual CORBA::Boolean
  handShake(const char* name, const char* myName);

  virtual char*
  getBindName();

  virtual void
  searchService(const char* predecessor,
                const char* predecessorId,
                const corba_request_t& request);

  virtual void
  stopFlooding(CORBA::Long reqId, const char* senderId);

  virtual void
  serviceNotFound(CORBA::Long reqId, const char* senderId);

  virtual void
  newFlood(CORBA::Long reqId, const char* senderId);

  virtual void
  floodedArea(CORBA::Long reqId, const char* senderId);

  virtual void
  alreadyContacted(CORBA::Long reqId, const char* senderId);

  virtual void
  serviceFound(CORBA::Long reqId, const corba_response_t& decision);
#endif
#ifdef HAVE_WORKFLOW
  virtual wf_response_t *
  submit_pb_set(const corba_pb_desc_seq_t& seq_pb);

  virtual response_seq_t*
  submit_pb_seq(const corba_pb_desc_seq_t& pb_seq,
                CORBA::Long reqCount,
                CORBA::Boolean& complete,
                CORBA::Long& firstReqId,
                CORBA::Long& seqReqId);

#endif
  virtual CORBA::Long
  childRemoveService(CORBA::ULong childID, const corba_profile_desc_t& profile);

  virtual SeqString*
  searchData(const char* request);

  virtual CORBA::Long
  insertData(const char* key, const SeqString& values);

  virtual char*
  getDataManager();

protected:
// Logger
  dadi::LoggerPtr mlogger;
// Channel for logger
  dadi::ChannelPtr mcc;

  Forwarder_ptr mforwarder;
  char* mobjName;
};
#endif  // _MASTERAGENTIMPL_HH_
