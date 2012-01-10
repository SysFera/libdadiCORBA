/**
 * @file MaDagImpl.cc
 *
 * @brief  MaDag manager
 *
 * @author
 *         - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
 *         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section License
 *   |LICENSE|
 */

#include "MaDagImpl.hh"

MaDagFwdrImpl::MaDagFwdrImpl(Forwarder_ptr fwdr, const char *objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

CORBA::Long
MaDagFwdrImpl::processDagWf(const corba_wf_desc_t &dag_desc,
                            const char *cltMgrRef,
                            CORBA::Long wfReqId) {
  return forwarder->processDagWf(dag_desc, cltMgrRef, wfReqId, objName);
}

CORBA::Long
MaDagFwdrImpl::processMultiDagWf(const corba_wf_desc_t &dag_desc,
                                 const char *cltMgrRef,
                                 CORBA::Long wfReqId,
                                 CORBA::Boolean release) {
  return forwarder->processMultiDagWf(dag_desc, cltMgrRef,
                                      wfReqId, release, objName);
}

CORBA::Long
MaDagFwdrImpl::getWfReqId() {
  return forwarder->getWfReqId(objName);
}

void
MaDagFwdrImpl::releaseMultiDag(CORBA::Long wfReqId) {
  forwarder->releaseMultiDag(wfReqId, objName);
}

void
MaDagFwdrImpl::cancelDag(CORBA::Long dagId) {
  forwarder->cancelDag(dagId, objName);
}

void
MaDagFwdrImpl::setPlatformType(MaDag::pfmType_t pfmType) {
  forwarder->setPlatformType(pfmType, objName);
}

CORBA::Long
MaDagFwdrImpl::ping() {
  return forwarder->ping(objName);
}
