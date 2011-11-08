
#include <cmath>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>   // For gethostname()
#include <ctime>
#include <sys/types.h>


/* CLEAN ME: this was a hilarious pun except that slimfast_api.h is nowhere
   to be found. The Changelog file of package fast-0.8.7 simply mentions:
   - For the LONG CHANGELOG entry of version 0.8.0:
   - SLiM is dead
   - slimfast_api.h renamed to fast.h
   - For entry 0.2.13:
   - slimfast_api.h used to be generated from the concatenation of
   several atomic files.h. [...]
   Also refer to src/utils/FASTMgr.cc, where the inclusion of slimfast_api.h
   is bound to the definition of __FAST_0_4__ preprocessing symbol...
   Hence it really looks like (to me at least) the following include should
   be simply removed ? Anyone knows better ?   --- Injay2461
   #if HAVE_FAST
   #include "slimfast_api.h"
   #endif  // HAVE_FAST
*/

#include "SeDImpl.hh"
#include "Callback.hh"
#include "common_types.hh"
#include "debug.hh"


#include "ORBMgr.hh"

#if defined HAVE_ALT_BATCH
#include "BatchCreator.hh"
#endif


/** The trace level. */
extern unsigned int TRACE_LEVEL;

SeDFwdrImpl::SeDFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

CORBA::Long SeDFwdrImpl::ping() {
  return forwarder->ping(objName);
}

CORBA::Long SeDFwdrImpl::bindParent(const char * parentName) {
  return forwarder->bindParent(parentName, objName);
}

CORBA::Long SeDFwdrImpl::disconnect() {
  return forwarder->disconnect(objName);
}

//CORBA::Long SeDFwdrImpl::removeElement(const char* a) {
//  return forwarder->removeElement(objName);
//}

void SeDFwdrImpl::getRequest(const corba_request_t& req) {
  return forwarder->getRequest(req, objName);
}

CORBA::Long SeDFwdrImpl::checkContract(corba_estimation_t& estimation,
                                       const corba_pb_desc_t& pb) {
  return forwarder->checkContract(estimation, pb, objName);
}

void SeDFwdrImpl::updateTimeSinceLastSolve() {
  forwarder->updateTimeSinceLastSolve(objName);
}

CORBA::Long SeDFwdrImpl::solve(const char* pbName, corba_profile_t& pb) {
  return forwarder->solve(pbName, pb, objName);
}

void SeDFwdrImpl::solveAsync(const char* pb_name, const corba_profile_t& pb,
                             const char * volatileclientIOR) {
  forwarder->solveAsync(pb_name, pb, volatileclientIOR, objName);
}

char* SeDFwdrImpl::getDataMgrID() {
  return forwarder->getDataMgrID(objName);
}

SeqCorbaProfileDesc_t*
SeDFwdrImpl::getSeDProfiles(CORBA::Long& length) {
  return forwarder->getSeDProfiles(length, objName);
}

