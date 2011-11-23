
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


#include "ORBMgr.hh"

#if defined HAVE_ALT_BATCH
#include "BatchCreator.hh"
#endif


/** The trace level. */
extern unsigned int TRACE_LEVEL;

SeDFwdrImpl::SeDFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

CORBA::Long SeDFwdrImpl::ping() {
  return mforwarder->ping(mobjName);
}

CORBA::Long SeDFwdrImpl::bindParent(const char * parentName) {
  return mforwarder->bindParent(parentName, mobjName);
}

CORBA::Long SeDFwdrImpl::disconnect() {
  return mforwarder->disconnect(mobjName);
}

CORBA::Long SeDFwdrImpl::removeElement() {
  return mforwarder->removeElement(false, mobjName);
}

void SeDFwdrImpl::getRequest(const corba_request_t& req) {
  return mforwarder->getRequest(req, mobjName);
}

CORBA::Long SeDFwdrImpl::checkContract(corba_estimation_t& estimation,
                                       const corba_pb_desc_t& pb) {
  return mforwarder->checkContract(estimation, pb, mobjName);
}

void SeDFwdrImpl::updateTimeSinceLastSolve() {
  mforwarder->updateTimeSinceLastSolve(mobjName);
}

CORBA::Long SeDFwdrImpl::solve(const char* pbName, corba_profile_t& pb) {
  return mforwarder->solve(pbName, pb, mobjName);
}

void SeDFwdrImpl::solveAsync(const char* pb_name, const corba_profile_t& pb,
                             const char * volatileclientIOR) {
  mforwarder->solveAsync(pb_name, pb, volatileclientIOR, mobjName);
}

char* SeDFwdrImpl::getDataMgrID() {
  return mforwarder->getDataMgrID(mobjName);
}

SeqCorbaProfileDesc_t*
SeDFwdrImpl::getSeDProfiles(CORBA::Long& length) {
  return mforwarder->getSeDProfiles(length, mobjName);
}

