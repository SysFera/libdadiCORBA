/**
 * @file SeD.cc
 *
 * @brief  DIET forwarder implementation - SeD forwarder implementation
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
CorbaForwarder::checkContract(::corba_estimation_t& estimation,
                             const ::corba_pb_desc_t& pb,
                             const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->checkContract(estimation, pb, objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->checkContract(estimation, pb);
  }

void
CorbaForwarder::updateTimeSinceLastSolve(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->updateTimeSinceLastSolve(objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->updateTimeSinceLastSolve();

}

::CORBA::Long
CorbaForwarder::solve(const char* path,
                     ::corba_profile_t& pb,
                     const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->solve(path, pb, objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->solve(path, pb);
}

void
CorbaForwarder::solveAsync(const char* path,
                          const ::corba_profile_t& pb,
                          const char* volatileclientPtr,
                          const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->solveAsync(path, pb, volatileclientPtr, objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->solveAsync(path, pb, volatileclientPtr);
}

char*
CorbaForwarder::getDataMgrID(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getDataMgrID(objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->getDataMgrID();
}

SeqCorbaProfileDesc_t*
CorbaForwarder::getSeDProfiles(::CORBA::Long& length,
                              const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->getSeDProfiles(length, objString.c_str());
  }

  name = getName(objString);

  SeD_var sed =
    ORBMgr::getMgr()->resolve<SeD, SeD_var>(SEDCTXT, name, this->mname);
  return sed->getSeDProfiles(length);
}
