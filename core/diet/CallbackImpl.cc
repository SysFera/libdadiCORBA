/**
 * @file CalllbackImpl.cc
 *
 * @brief  DIET forwarder implementation
 *
 * @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

using namespace std;
#include <iostream>
#include "CallbackImpl.hh"
//#define TRACE_LEVEL 50

#include "Forwarder.hh"
#include "CallbackFwdr.hh"


CallbackFwdrImpl::CallbackFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

CORBA::Long CallbackFwdrImpl::ping() {
  return mforwarder->ping(mobjName);
}

CORBA::Long CallbackFwdrImpl::notifyResults(const char * path,
                                            const corba_profile_t& pb,
                                            CORBA::Long reqID) {
  return mforwarder->notifyResults(path, pb, reqID, mobjName);
}

CORBA::Long CallbackFwdrImpl::solveResults(const corba_profile_t& pb,
                                           CORBA::Long reqID,
                                           CORBA::Long solve_res) {
  return mforwarder->solveResults(pb, reqID, solve_res, mobjName);
}
