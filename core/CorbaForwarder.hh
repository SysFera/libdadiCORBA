/**
 * @file CorbaForwarder.hh
 *
 * @brief  DIET forwarder implementation
 *
 * @author
 *         - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef CORBAFORWARDER_HH
#define CORBAFORWARDER_HH

#include <list>
#include <map>
#include <string>
#include <omnithread.h>
#include "Forwarder.hh"
#include "common_types.hh"
#include "LogTypes.hh"
#include "response.hh"
#include "dadi/Logging/Logger.hh"

/**
 * @brief The corba forwarder class that defines all the methods that can pass
 * throught the forwarder. For non documented methods, please see the Forwarder
 * and its parents idl interfaces.
 * @class CorbaForwarder
 */
class CorbaForwarder : public POA_Forwarder,
                       public PortableServer::RefCountServantBase {
public:
/**
 * @brief Constructor
 * @param name Name of the forwarder
 */
  explicit CorbaForwarder(const std::string& name);

  /* DIET object factory methods. */

  Agent_ptr
  getAgent(const char* name);
  Callback_ptr
  getCallback(const char* name);
  LocalAgent_ptr
  getLocalAgent(const char* name);
  MasterAgent_ptr
  getMasterAgent(const char* name);
  SeD_ptr
  getSeD(const char* name);
  Dagda_ptr
  getDagda(const char* name);

  CltMan_ptr
  getCltMan(const char* name);
  MaDag_ptr
  getMaDag(const char* name);
  WfLogService_ptr
  getWfLogService(const char* name);

  /* Common methods implementations. */
  ::CORBA::Long
  ping(const char* objName);
  void
  getRequest(const ::corba_request_t& req, const char* objName);
  char*
  getHostname(const char* objName);
  ::CORBA::Long
  bindParent(const char* parentName, const char* objName);
  ::CORBA::Long
  disconnect(const char* objName);
  ::CORBA::Long
  removeElement(::CORBA::Boolean recursive, const char* objName);

  /*
   * @brief To determine if the call is from another forwarder and
   * modify the object name.
   * @param objName The name of the object to check if local
   * @return If the call is remote = true
   */
  static bool
  remoteCall(std::string& objName);

  /* CORBA remote management implementation. */
  void
  bind(const char* objName, const char* ior);
  void
  unbind(const char* objName);
  SeqString*
  getBindings(const char* ctxt);
  void
  connectPeer(const char* ior, const char* host, const ::CORBA::Long port);
  char*
  getIOR();
  char*
  getName();
  char*
  getPeerName();
  char*
  getHost();
  char*
  getPeerHost();
  /**
   * @brief Set this forwarder peer object (not CORBA).
   * @param peer The peer to set
   */
  void
  setPeer(Forwarder_ptr peer);
  /**
   * @brief To get the corresponding peer
   * @return The corresponding peer
   */
  Forwarder_var
  getPeer();
  /* Object caches management functions. */
/**
 * @brief To remove the object name from the cache
 * @param name The object to remove from the cache
 */
  void
  removeObjectFromCache(const std::string& name);
/**
 * @brief To clean the caches
 */
  void
  cleanCaches();

  /* Utility function. */

/**
 * @brief To get the other forwarders on this domain
 * @return A set of forwarder name
 */
  std::list<std::string>
  otherForwarders() const;

  /* AgentFwdr implementation. */

  ::CORBA::Long
  agentSubscribe(const char* agentName,
                 const char* hostname,
                 const ::SeqCorbaProfileDesc_t& services,
                 const char* objName);

  ::CORBA::Long
  serverSubscribe(const char* seDName,
                  const char* hostname,
                  const ::SeqCorbaProfileDesc_t& services,
                  const char* objName);

  ::CORBA::Long
  childUnsubscribe(::CORBA::ULong childID,
                   const ::SeqCorbaProfileDesc_t& services,
                   const char* objName);

  ::CORBA::Long
  childRemoveService(::CORBA::ULong childID,
                     const ::corba_profile_desc_t& profile,
                     const char* objName);

  ::CORBA::Long
  addServices(::CORBA::ULong myID,
              const ::SeqCorbaProfileDesc_t& services,
              const char* objName);

  void
  getResponse(const ::corba_response_t& resp, const char* objName);

  char*
  getDataManager(const char* objName);

  /* CallbackFwdr implementation. */
  ::CORBA::Long
  notifyResults(const char* path,
                const ::corba_profile_t& pb,
                ::CORBA::Long reqID,
                const char* objName);

  ::CORBA::Long
  solveResults(const ::corba_profile_t& pb,
               ::CORBA::Long reqID,
               ::CORBA::Long result,
               const char* objName);

  /* CltManFwdr implementation. */
  ::CORBA::Long
  execNodeOnSed(const char* node_id,
                const char* dag_id,
                const char* seDName,
                ::CORBA::ULong reqID,
                ::corba_estimation_t& ev,
                const char* objName);

  ::CORBA::Long
  execNode(const char* node_id, const char* dag_id, const char* objName);

  char*
  release(const char* dag_id,
          ::CORBA::Boolean successful,
          const char* objName);

  /* Dagda implementation. */
  ::CORBA::Boolean
  lclIsDataPresent(const char* dataID, const char* objName);

  ::CORBA::Boolean
  lvlIsDataPresent(const char* dataID, const char* objName);

  ::CORBA::Boolean
  pfmIsDataPresent(const char* dataID, const char* objName);

  void
  lclAddData(const char* srcDagda,
             const ::corba_data_t& data,
             const char* objName);

  void
  lvlAddData(const char* srcDagda,
             const ::corba_data_t& data,
             const char* objName);

  void
  pfmAddData(const char* srcDagda,
             const ::corba_data_t& data,
             const char* objName);

  void
  registerFile(const ::corba_data_t& data, const char* objName);

  void
  lclAddContainerElt(const char* containerID,
                     const char* dataID,
                     ::CORBA::Long index,
                     ::CORBA::Long flag,
                     const char* objName);

  ::CORBA::Long
  lclGetContainerSize(const char* containerID, const char* objName);

  void
  lclGetContainerElts(const char* containerID,
                      ::SeqString& dataIDSeq,
                      ::SeqLong& flagSeq,
                      ::CORBA::Boolean ordered,
                      const char* objName);

  void
  lclRemData(const char* dataID, const char* objName);

  void
  lvlRemData(const char* dataID, const char* objName);

  void
  pfmRemData(const char* dataID, const char* objName);

  void
  lclUpdateData(const char* srcDagda,
                const ::corba_data_t& data,
                const char* objName);

  void
  lvlUpdateData(const char* srcDagda,
                const ::corba_data_t& data,
                const char* objName);

  void
  pfmUpdateData(const char* srcDagda,
                const ::corba_data_t& data,
                const char* objName);

  SeqCorbaDataDesc_t*
  lclGetDataDescList(const char* objName);

  SeqCorbaDataDesc_t*
  lvlGetDataDescList(const char* objName);

  SeqCorbaDataDesc_t*
  pfmGetDataDescList(const char* objName);

  corba_data_desc_t*
  lclGetDataDesc(const char* dataID, const char* objName);

  corba_data_desc_t*
  lvlGetDataDesc(const char* dataID, const char* objName);

  corba_data_desc_t*
  pfmGetDataDesc(const char* dataID, const char* objName);

  void
  lclReplicate(const char* dataID,
               ::CORBA::Long ruleTarget,
               const char* pattern,
               ::CORBA::Boolean replace,
               const char* objName);

  void
  lvlReplicate(const char* dataID,
               ::CORBA::Long ruleTarget,
               const char* pattern,
               ::CORBA::Boolean replace,
               const char* objName);

  void
  pfmReplicate(const char* dataID,
               ::CORBA::Long ruleTarget,
               const char* pattern,
               ::CORBA::Boolean replace,
               const char* objName);

  char*
  writeFile(const ::SeqChar& data,
            const char* basename,
            ::CORBA::Boolean replace,
            const char* objName);

  char*
  sendFile(const ::corba_data_t& data,
           const char* destDagda,
           const char* objName);

  char*
  recordData(const ::SeqChar& data,
             const ::corba_data_desc_t& dataDesc,
             ::CORBA::Boolean replace,
             ::CORBA::Long offset,
             const char* objName);

  char*
  sendData(const char* ID,
           const char* destDagda,
           const char* objName);

  char*
  sendContainer(const char* ID,
                const char* destDagda,
                ::CORBA::Boolean sendElements,
                const char* objName);

  SeqString*
  lvlGetDataManagers(const char* dataID, const char* objName);

  SeqString*
  pfmGetDataManagers(const char* dataID, const char* objName);

  void
  subscribe(const char* dagdaName, const char* objName);

  void
  unsubscribe(const char* dagdaName, const char* objName);

  char*
  getID(const char* objName);

  void
  lockData(const char* dataID, const char* objName);

  void
  unlockData(const char* dataID, const char* objName);

  Dagda::dataStatus
  getDataStatus(const char* dataID, const char* objName);

  char*
  getBestSource(const char* destDagda,
                const char* dataID,
                const char* objName);

  void
  checkpointState(const char* objName);

  void
  subscribeParent(const char* parentID, const char* objName);

  void
  unsubscribeParent(const char* objName);


  /* MaDagFwdr implementation. */
  ::CORBA::Long
  processDagWf(const ::corba_wf_desc_t& dag_desc,
               const char* cltMgrRef,
               ::CORBA::Long wfReqId,
               const char* objName);

  ::CORBA::Long
  processMultiDagWf(const ::corba_wf_desc_t& dag_desc,
                    const char* cltMgrRef,
                    ::CORBA::Long wfReqId,
                    ::CORBA::Boolean release,
                    const char* objName);

  ::CORBA::Long
  getWfReqId(const char* objName);

  void
  releaseMultiDag(::CORBA::Long wfReqId, const char* objName);

  void
  cancelDag(::CORBA::Long dagId, const char* objName);

  void
  setPlatformType(::MaDag::pfmType_t pfmType, const char* objName);

  /* MasterAgentFwdr implementation. */
  corba_response_t*
  submit(const ::corba_pb_desc_t& pb_profile,
         ::CORBA::ULong maxServers,
         const char* objName);

  ::CORBA::Long
  get_session_num(const char* objName);

  char*
  get_data_id(const char* objName);

  ::CORBA::ULong
  dataLookUp(const char* id, const char* objName);

  corba_data_desc_t*
  get_data_arg(const char* argID, const char* objName);

  ::CORBA::Long
  diet_free_pdata(const char* argID, const char* objName);

  SeqCorbaProfileDesc_t*
  getProfiles(::CORBA::Long& length, const char* objName);

  wf_response_t*
  submit_pb_set(const ::corba_pb_desc_seq_t& seq_pb, const char* objName);

  response_seq_t*
  submit_pb_seq(const ::corba_pb_desc_seq_t& pb_seq,
                ::CORBA::Long reqCount,
                ::CORBA::Boolean& complete,
                ::CORBA::Long& firstReqId,
                ::CORBA::Long& seqReqId,
                const char* objName);

  SeqString*
  searchData(const char* request, const char* objName);

  ::CORBA::Long
  insertData(const char* key,
             const ::SeqString& values,
             const char* objName);

  ::CORBA::Boolean
  handShake(const char* masterAgentName,
            const char* myName,
            const char* objName);

  char*
  getBindName(const char* objName);

  void
  searchService(const char* masterAgentName,
                const char* myName,
                const ::corba_request_t& request,
                const char* objName);

  void
  stopFlooding(::CORBA::Long reqId,
               const char* senderId,
               const char* objName);

  void
  serviceNotFound(::CORBA::Long reqId,
                  const char* senderId,
                  const char* objName);

  void
  newFlood(::CORBA::Long reqId,
           const char* senderId,
           const char* objName);

  void
  floodedArea(::CORBA::Long reqId,
              const char* senderId,
              const char* objName);

  void
  alreadyContacted(::CORBA::Long reqId,
                   const char* senderId,
                   const char* objName);

  void
  serviceFound(::CORBA::Long reqId,
               const ::corba_response_t& decision,
               const char* objName);
  /* SeD implementation. */

  ::CORBA::Long
  checkContract(::corba_estimation_t& estimation,
                const ::corba_pb_desc_t& pb,
                const char* objName);

  void
  updateTimeSinceLastSolve(const char* objName);

  ::CORBA::Long
  solve(const char* path,
        ::corba_profile_t& pb,
        const char* objName);

  void
  solveAsync(const char* path,
             const ::corba_profile_t& pb,
             const char* volatileclientPtr,
             const char* objName);

  char*
  getDataMgrID(const char* objName);

  SeqCorbaProfileDesc_t*
  getSeDProfiles(::CORBA::Long& length, const char* objName);

  /* WfLogServiceFwdr implementation. */
  void
  initWorkflow(const char* wfId, const char* name,
               const char* parentWfId, const char* objName);

  void
  updateWorkflow(const char* wfId, const char* wfState,
                 const char* data, const char* objName);

  void
  createDag(const char* dagId, const char* wfId,
            const char* objName);

  void
  updateDag(const char* dagId, const char* wfId,
            const char* dagState, const char* data,
            const char* objName);

  void
  createDagNode(const char* dagNodeId, const char* dagId,
                const char* wfId, const char* objName);

  void
  createDagNodePort(const char* dagNodePortId, const char* portDirection,
                    const char* dagNodeId, const char* wfId,
                    const char* objName);

  void
  setInPortDependencies(const char* dagNodePortId, const char* dagNodeId,
                        const char* wfId, const char* dependencies,
                        const char* objName);

  void
  createDagNodeLink(const char* srcNodeId, const char* srcWfId,
                    const char* destNodeId, const char* destWfId,
                    const char* objName);

  void
  createDagNodeData(const char* dagNodeId, const char* wfId,
                    const char* dagNodePortId, const char* dataId,
                    const char* objName);

  void
  createDataElements(const char* dataId, const char* elementIdList,
                     const char* objName);

  void
  createSourceDataTree(const char* sourceId, const char* wfId,
                       const char* dataIdTree, const char* objName);

  void
  createSinkData(const char* sinkId, const char* wfId,
                 const char* dataId, const char* objName);

  void
  nodeIsReady(const char* dagNodeId, const char* wfId, const char* objName);

  void
  nodeIsStarting(const char* dagNodeId, const char* wfId,
                 const char* pbName, const char* hostname,
                 const char* objName);

  void
  nodeIsRunning(const char* dagNodeId, const char* wfId, const char* objName);

  void
  nodeIsDone(const char* dagNodeId, const char* wfId, const char* objName);

  void
  nodeIsFailed(const char* dagNodeId, const char* wfId, const char* objName);

  /* Utility fonctions to extract name & context from context/name. */
  static std::string
  getName(const std::string& namectxt);

  static std::string
  getCtxt(const std::string& namectxt);

  ComponentConfigurator_ptr getCompoConf(const char* name);
  ToolMsgReceiver_ptr getToolMsgReceiver(const char* name);
  LogCentralComponent_ptr getLogCentralComponent(const char* name);
  LogCentralTool_ptr getLogCentralTool(const char* name);
  /* LogComponentFwdr implementation. */
  void setTagFilter(const ::tag_list_t& tagList,
                    const char* objName);
  void addTagFilter(const ::tag_list_t& tagList,
                    const char* objName);
  void removeTagFilter(const ::tag_list_t& tagList,
                       const char* objName);
  short
  disconnectTool(const char* toolName, const char* objName);
  tag_list_t*
  getDefinedTags(const char* objName);
  component_list_t*
  getDefinedComponents(const char*  objName);
  short
  addFilter(const char* toolName, const filter_t& filter, const char* objName);
  void
  sendMsg(const log_msg_buf_t& msgBuf, const char*  objName);
  short
  connectTool(char*& toolName, const char* msgReceiver,  const char* objName);
  short
  flushAllFilters(const char* toolName, const char* objName);
  short
  removeFilter(const char* toolName,
               const char* filterName,
               const char* objName);
  short connectComponent(char*&, const char*, const char*, const char*,
                         const log_time_t&, tag_list_t&, const char*);
  short
  disconnectComponent(const char* componentName,
                      const char* message,
                      const char* objName);
  void
  sendBuffer(const log_msg_buf_t &buffer,
             const char* objName);
  void
  synchronize(const char* componentName,
              const log_time_t& componentTime,
              const char* objName);
private:
  /**
   * @brief When a new forwarder object is created, we cache it.
   * Because this kind of object contains only the object
   * name and a reference to this forwarder, there is no
   * risk to cache it, even if the object is restarted or
   * disappear.
   */
  std::map<std::string, ::CORBA::Object_ptr> mobjectCache;
  /**
   * @brief We also maintain a list of activated servant objects.
   */
  std::map<std::string, PortableServer::ServantBase*> mservants;

  ::CORBA::Object_ptr
  getObjectCache(const std::string& name);

  /**
   * @brief The forwarder associated to this one.
   */
  Forwarder_var mpeer;
  /* Mutexes */
/**
 * @brief Mutex to handle the peer
 */
  omni_mutex mpeerMutex;   // To wait for the peer initialization
/**
 * @brief Mutex to handle the cache
 */
  omni_mutex mcachesMutex;  // Protect access to caches

/**
 * @brief Forwarder name
 */
  std::string mname;
/**
 * @brief Forwarder machine hostname
 */
  std::string mhost;

/**
 * @brief Logger
 */
  dadi::LoggerPtr mlogger;
/**
 * @brief Channel for logger
 */
  dadi::ChannelPtr mcc;
};

#endif
