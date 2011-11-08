
using namespace std;
#include <iostream>
#include "CallbackImpl.hh"
//#define TRACE_LEVEL 50
#include "debug.hh"

#include "Forwarder.hh"
#include "CallbackFwdr.hh"


CallbackFwdrImpl::CallbackFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

CORBA::Long CallbackFwdrImpl::ping() {
  return forwarder->ping(objName);
}

CORBA::Long CallbackFwdrImpl::notifyResults(const char * path,
                                            const corba_profile_t& pb,
                                            CORBA::Long reqID) {
  return forwarder->notifyResults(path, pb, reqID, objName);
}

CORBA::Long CallbackFwdrImpl::solveResults(const char * path,
                                           const corba_profile_t& pb,
                                           CORBA::Long reqID,
                                           CORBA::Long solve_res) {
  return forwarder->solveResults(path, pb, reqID, solve_res, objName);
}
