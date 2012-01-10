/**
 * @file CltWfMgrImpl.hh
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

#ifndef __CLTWFMGRFWDR__HH__
#define __CLTWFMGRFWDR__HH__

#include "Forwarder.hh"
/**
 * @brief The CltWfMgr forwarder class that implements all the workflows methods
 * throught the forwarder
 * @class CltWfMgrFwdr
 * For non documented methods, please see the workflow and its parents idl interfaces.
 */
/* Forwarder part. */
class CltWfMgrFwdr : public POA_CltManFwdr,
public PortableServer::RefCountServantBase {
public:
  CltWfMgrFwdr(Forwarder_ptr fwdr, const char *objName);

  virtual CORBA::Long
  execNodeOnSed(const char *node_id, const char *dag_id,
                const char *sed, const CORBA::ULong reqID,
                corba_estimation_t &ev);

  virtual CORBA::Long
  execNode(const char *node_id, const char *dag_id);

  virtual char *
  release(const char *dag_id, bool successful);

  virtual CORBA::Long
  ping();

private:
  Forwarder_ptr forwarder;
  char *objName;
};

#endif // __CLTWFMGRFWDR__HH__
