/**
 * @file CltMan.cc
 *
 * @brief  DIET forwarder implementation - Workflow mngr forwarder implementation
 *
 * @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   | LICENSE|
 */

#include "CorbaForwarder.hh"
#include "ORBMgr.hh"
#include <string>

::CORBA::Long
CorbaForwarder::execNodeOnSed(const char* node_id,
                             const char* dag_id,
                             const char* seDName,
                             ::CORBA::ULong reqID,
                             ::corba_estimation_t& ev,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->execNodeOnSed(node_id, dag_id, seDName,
                                    reqID, ev, objString.c_str());
  }

  name = getName(objString);

  CltMan_var clt =
    ORBMgr::getMgr()->resolve<CltMan, CltMan_var>(WFMGRCTXT, name, this->name);
  return clt->execNodeOnSed(node_id, dag_id, seDName, reqID, ev);
  }

::CORBA::Long
CorbaForwarder::execNode(const char* node_id,
                        const char* dag_id,
                        const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->execNode(node_id, dag_id, objString.c_str());
  }

  name = getName(objString);

  CltMan_var clt =
    ORBMgr::getMgr()->resolve<CltMan, CltMan_var>(WFMGRCTXT, name, this->name);
  return clt->execNode(node_id, dag_id);
  }

char*
CorbaForwarder::release(const char* dag_id,
                       ::CORBA::Boolean successful,
                       const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->release(dag_id, successful, objString.c_str());
  }

  name = getName(objString);

  CltMan_var clt =
    ORBMgr::getMgr()->resolve<CltMan, CltMan_var>(WFMGRCTXT, name, this->name);
  return clt->release(dag_id, successful);
}
