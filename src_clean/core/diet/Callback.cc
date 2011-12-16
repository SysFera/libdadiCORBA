/**
 * @file Callback.cc
 *
 * @brief  DIET forwarder implementation - Client forwarder implementation
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

::CORBA::Long
CorbaForwarder::notifyResults(const char* path, const ::corba_profile_t& pb,
                             ::CORBA::Long reqID, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->notifyResults(path, pb, reqID, objString.c_str());
  }

  name = getName(objString);

  Callback_var cb =
    ORBMgr::getMgr()->resolve<Callback, Callback_var>(CLIENTCTXT, name,
                                                      this->mname);
  return cb->notifyResults(path, pb, reqID);
  }

::CORBA::Long
CorbaForwarder::solveResults(const ::corba_profile_t& pb,
                            ::CORBA::Long reqID,
                            ::CORBA::Long result,
                            const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->solveResults(pb, reqID, result, objString.c_str());
  }

  name = getName(objString);

  Callback_var cb =
    ORBMgr::getMgr()->resolve<Callback, Callback_var>(CLIENTCTXT, name,
                                                      this->mname);
  return cb->solveResults(pb, reqID, result);
  }
