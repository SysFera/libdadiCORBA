/**
 * @file CltWfMgrImpl.cc
 *
 * @brief  Workflow manager
 *
 * @author
 *         - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
 *         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section License
 *   |LICENSE|
 */
#include "CltWfMgrImpl.hh"

/* Constructor takes the object name and the forwarder to use to
 * contact the object.
 */
CltWfMgrFwdr::CltWfMgrFwdr(Forwarder_ptr fwdr, const char *objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

/* Each method simply calls the forwarder correponding method. */
/* i.e: fun(a, b, ...) => forwarder->fun(a, b, ..., objName) */
CORBA::Long
CltWfMgrFwdr::execNodeOnSed(const char *node_id,
                            const char *dag_id,
                            const char *sed,
                            const CORBA::ULong reqID,
                            corba_estimation_t &ev) {
  return forwarder->execNodeOnSed(node_id, dag_id, sed, reqID, ev, objName);
}

CORBA::Long
CltWfMgrFwdr::execNode(const char *node_id, const char *dag_id) {
  return forwarder->execNode(node_id, dag_id, objName);
}

char *
CltWfMgrFwdr::release(const char *dag_id, bool successful) {
  return forwarder->release(dag_id, successful, objName);
}

CORBA::Long
CltWfMgrFwdr::ping() {
  return forwarder->ping(objName);
}
