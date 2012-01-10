/**
 * @file WfLogService.cc
 *
 * @brief  DIET forwarder implementation - Workflow Logservice forwarder implem
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

void
CorbaForwarder::createDag(const char* dagId,
                         const char* wfId,
                         const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDag(dagId, wfId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDag(dagId, wfId);
}

void
CorbaForwarder::createDagNode(const char* dagNodeId, const char* dagId,
                             const char* wfId, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDagNode(dagNodeId, dagId, wfId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDagNode(dagNodeId, dagId, wfId);
}

void
CorbaForwarder::createDagNodeData(const char* dagNodeId, const char* wfId,
                                 const char* dagNodePortId, const char* dataId,
                                 const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDagNodeData(dagNodeId, wfId, dagNodePortId,
                                        dataId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDagNodeData(dagNodeId, wfId, dagNodePortId, dataId);
}

void
CorbaForwarder::createDagNodeLink(const char* srcNodeId, const char* srcWfId,
                                 const char* destNodeId, const char* destWfId,
                                 const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDagNodeLink(srcNodeId, srcWfId, destNodeId,
                                        destWfId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDagNodeLink(srcNodeId, srcWfId, destNodeId, destWfId);
}

void
CorbaForwarder::createDagNodePort(const char* dagNodePortId,
                                 const char* portDirection,
                                 const char* dagNodeId,
                                 const char* wfId,
                                 const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDagNodePort(dagNodePortId, portDirection,
                                        dagNodeId, wfId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDagNodePort(dagNodePortId, portDirection, dagNodeId, wfId);
}

void
CorbaForwarder::createDataElements(const char* dataId,
                                  const char* elementIdList,
                                  const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createDataElements(dataId, elementIdList, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createDataElements(dataId, elementIdList);
}

void
CorbaForwarder::createSinkData(const char* sinkId, const char* wfId,
                              const char* dataId, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createSinkData(sinkId, wfId, dataId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createSinkData(sinkId, wfId, dataId);
}

void
CorbaForwarder::createSourceDataTree(const char* sourceId,
                                    const char* wfId,
                                    const char* dataIdTree,
                                    const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->createSourceDataTree(sourceId, wfId,
                                           dataIdTree, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->createSourceDataTree(sourceId, wfId, dataIdTree);
}

void
CorbaForwarder::initWorkflow(const char* wfId, const char* wfName,
                            const char* parentWfId, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->initWorkflow(wfId, wfName, parentWfId, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->initWorkflow(wfId, wfName, parentWfId);
}

void
CorbaForwarder::setInPortDependencies(const char* dagNodePortId,
                                     const char* dagNodeId,
                                     const char* wfId,
                                     const char* dependencies,
                                     const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->setInPortDependencies(dagNodePortId, dagNodeId,
                                            wfId, dependencies, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->setInPortDependencies(dagNodePortId, dagNodeId,
                                     wfId, dependencies);
}

void
CorbaForwarder::updateDag(const char* dagId, const char* wfId,
                         const char* dagState, const char* data,
                         const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->updateDag(dagId, wfId, dagState, data, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->updateDag(dagId, wfId, dagState, data);
}

void
CorbaForwarder::updateWorkflow(const char* wfId, const char* wfState,
                              const char* data, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->updateWorkflow(wfId, wfState, data, objName);
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->updateWorkflow(wfId, wfState, data);
}

void
CorbaForwarder::nodeIsDone(const char* node_id, const char* wfId,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->nodeIsDone(node_id, wfId, objString.c_str());
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->nodeIsDone(node_id, wfId);
}

void
CorbaForwarder::nodeIsFailed(const char* node_id, const char* wfId,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->nodeIsFailed(node_id, wfId, objString.c_str());
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->nodeIsFailed(node_id, wfId);
}

void
CorbaForwarder::nodeIsReady(const char* node_id, const char* wfId,
                           const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->nodeIsReady(node_id, wfId, objString.c_str());
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->nodeIsReady(node_id, wfId);
}

void
CorbaForwarder::nodeIsRunning(const char* node_id, const char* wfId,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->nodeIsRunning(node_id, wfId, objString.c_str());
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->nodeIsRunning(node_id, wfId);

}

void
CorbaForwarder::nodeIsStarting(const char* node_id, const char* wfId,
                              const char* pbName, const char* hostname,
                              const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->nodeIsStarting(node_id, wfId,
                                     pbName, hostname,
                                     objString.c_str());
  }

  name = getName(objString);

  WfLogService_var wfls =
    ORBMgr::getMgr()->resolve<WfLogService, WfLogService_var>(WFLOGCTXT,
                                                              name,
                                                              this->mname);
  return wfls->nodeIsStarting(node_id, wfId, pbName, hostname);
}

