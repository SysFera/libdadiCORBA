/**
* @file Dagda.cc
*
* @brief  DIET forwarder implementation - Dagda forwarder implementation
*
* @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
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
#include <string>
#include <iostream>


::CORBA::Boolean
CorbaForwarder::lclIsDataPresent(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclIsDataPresent(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclIsDataPresent(dataID);
  }

::CORBA::Boolean
CorbaForwarder::lvlIsDataPresent(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlIsDataPresent(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlIsDataPresent(dataID);
  }

::CORBA::Boolean
CorbaForwarder::pfmIsDataPresent(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmIsDataPresent(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmIsDataPresent(dataID);
  }

void CorbaForwarder::lclAddData(const char* srcDagda,
                               const ::corba_data_t& data,
                               const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclAddData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda = ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name,
                                                                this->mname);
  return dagda->lclAddData(srcDagda, data);
}

void
CorbaForwarder::lvlAddData(const char* srcDagda,
                          const ::corba_data_t& data,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlAddData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlAddData(srcDagda, data);
}

void
CorbaForwarder::pfmAddData(const char* srcDagda,
                          const ::corba_data_t& data,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmAddData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmAddData(srcDagda, data);
}

void
CorbaForwarder::registerFile(const ::corba_data_t& data, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->registerFile(data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda = ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name,
                                                                this->mname);
  return dagda->registerFile(data);
}

void
CorbaForwarder::lclAddContainerElt(const char* containerID,
                                  const char* dataID,
                                  ::CORBA::Long index,
                                  ::CORBA::Long flag,
                                  const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclAddContainerElt(containerID, dataID, index,
                                         flag, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclAddContainerElt(containerID, dataID, index, flag);
}

::CORBA::Long
CorbaForwarder::lclGetContainerSize(const char* containerID,
                                   const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclGetContainerSize(containerID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclGetContainerSize(containerID);
}

void
CorbaForwarder::lclGetContainerElts(const char* containerID,
                                   ::SeqString& dataIDSeq,
                                   ::SeqLong& flagSeq,
                                   ::CORBA::Boolean ordered,
                                   const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclGetContainerElts(containerID, dataIDSeq,
                                          flagSeq, ordered,
                                          objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclGetContainerElts(containerID, dataIDSeq, flagSeq, ordered);
}

void
CorbaForwarder::lclRemData(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclRemData(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclRemData(dataID);
}

void
CorbaForwarder::lvlRemData(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlRemData(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlRemData(dataID);
}

void
CorbaForwarder::pfmRemData(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmRemData(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmRemData(dataID);
}

void
CorbaForwarder::lclUpdateData(const char* srcDagda,
                             const ::corba_data_t& data,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclUpdateData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclUpdateData(srcDagda, data);
}

void
CorbaForwarder::lvlUpdateData(const char* srcDagda,
                             const ::corba_data_t& data,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlUpdateData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlUpdateData(srcDagda, data);
}

void
CorbaForwarder::pfmUpdateData(const char* srcDagda,
                             const ::corba_data_t& data,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmUpdateData(srcDagda, data, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmUpdateData(srcDagda, data);
}

SeqCorbaDataDesc_t*
CorbaForwarder::lclGetDataDescList(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclGetDataDescList(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclGetDataDescList();
}

SeqCorbaDataDesc_t*
CorbaForwarder::lvlGetDataDescList(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlGetDataDescList(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlGetDataDescList();
}

SeqCorbaDataDesc_t*
CorbaForwarder::pfmGetDataDescList(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmGetDataDescList(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmGetDataDescList();
}

corba_data_desc_t*
CorbaForwarder::lclGetDataDesc(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclGetDataDesc(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclGetDataDesc(dataID);
}

corba_data_desc_t*
CorbaForwarder::lvlGetDataDesc(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlGetDataDesc(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda = ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name,
                                                                this->mname);
  return dagda->lvlGetDataDesc(dataID);
}

corba_data_desc_t*
CorbaForwarder::pfmGetDataDesc(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmGetDataDesc(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmGetDataDesc(dataID);
}

void
CorbaForwarder::lclReplicate(const char* dataID,
                            ::CORBA::Long ruleTarget,
                            const char* pattern,
                            ::CORBA::Boolean replace,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lclReplicate(dataID, ruleTarget, pattern, replace,
                                   objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lclReplicate(dataID, ruleTarget, pattern, replace);
}

void
CorbaForwarder::lvlReplicate(const char* dataID,
                            ::CORBA::Long ruleTarget,
                            const char* pattern,
                            ::CORBA::Boolean replace,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlReplicate(dataID, ruleTarget, pattern, replace,
                                   objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlReplicate(dataID, ruleTarget, pattern, replace);
}

void
CorbaForwarder::pfmReplicate(const char* dataID,
                            ::CORBA::Long ruleTarget,
                            const char* pattern,
                            ::CORBA::Boolean replace,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmReplicate(dataID, ruleTarget, pattern, replace,
                                   objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmReplicate(dataID, ruleTarget, pattern, replace);
}

char*
CorbaForwarder::writeFile(const ::SeqChar& data,
                         const char* basename,
                         ::CORBA::Boolean replace,
                         const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->writeFile(data, basename, replace, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->writeFile(data, basename, replace);
}

char*
CorbaForwarder::sendFile(const ::corba_data_t& data,
                        const char* destDagda,
                        const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->sendFile(data, destDagda, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->sendFile(data, destDagda);
}

char*
CorbaForwarder::recordData(const ::SeqChar& data,
                          const ::corba_data_desc_t& dataDesc,
                          ::CORBA::Boolean replace,
                          ::CORBA::Long offset,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->recordData(data, dataDesc, replace, offset,
                                 objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->recordData(data, dataDesc, replace, offset);
}

char*
CorbaForwarder::sendData(const char* ID,
                        const char* destDagda,
                        const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->sendData(ID, destDagda, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->sendData(ID, destDagda);
}

char*
CorbaForwarder::sendContainer(const char* ID,
                             const char* destDagda,
                             ::CORBA::Boolean sendElements,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->sendContainer(ID, destDagda, sendElements,
                                    objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->sendContainer(ID, destDagda, sendElements);
}

SeqString*
CorbaForwarder::lvlGetDataManagers(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lvlGetDataManagers(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lvlGetDataManagers(dataID);
}

SeqString*
CorbaForwarder::pfmGetDataManagers(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->pfmGetDataManagers(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->pfmGetDataManagers(dataID);
}

void
CorbaForwarder::subscribe(const char* dagdaName, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->subscribe(dagdaName, objString.c_str());
  }
  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  dagda->subscribe(dagdaName);
}

void
CorbaForwarder::unsubscribe(const char* dagdaName, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->unsubscribe(dagdaName, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->unsubscribe(dagdaName);
}

char*
CorbaForwarder::getID(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getID(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->getID();
}

void
CorbaForwarder::lockData(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->lockData(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->lockData(dataID);
}

void
CorbaForwarder::unlockData(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->unlockData(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->unlockData(dataID);
}

Dagda::dataStatus
CorbaForwarder::getDataStatus(const char* dataID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getDataStatus(dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->getDataStatus(dataID);
}

char*
CorbaForwarder::getBestSource(const char* destDagda,
                             const char* dataID,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getBestSource(destDagda, dataID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->getBestSource(destDagda, dataID);
}

void
CorbaForwarder::checkpointState(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->checkpointState(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->checkpointState();
}

void
CorbaForwarder::subscribeParent(const char* parentID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->subscribeParent(parentID, objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->subscribeParent(parentID);
}

void
CorbaForwarder::unsubscribeParent(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->unsubscribeParent(objString.c_str());
  }

  name = getName(objString);

  Dagda_var dagda =
    ORBMgr::getMgr()->resolve<Dagda, Dagda_var>(DAGDACTXT, name, this->mname);
  return dagda->unsubscribeParent();
}

