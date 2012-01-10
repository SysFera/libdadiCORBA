/**
 * @file MaDagImpl.hh
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

#ifndef __MADAGFWDRIMPL__HH__
#define __MADAGFWDRIMPL__HH__

#include "Forwarder.hh"
#include "MaDag.hh"

/**
 * @brief The MaDag forwarder class that implements all the madag methods
 * throught the forwarder
 * @class CltWfMgrFwdr
 * For non documented methods, please see the workflow and its parents idl interfaces.
 */
class MaDagFwdrImpl : public POA_MaDag,
public PortableServer::RefCountServantBase {
public:
  MaDagFwdrImpl(Forwarder_ptr fwdr, const char *objName);

  virtual CORBA::Long
  processDagWf(const corba_wf_desc_t &dag_desc, const char *cltMgrRef,
               CORBA::Long wfReqId);

  virtual CORBA::Long
  processMultiDagWf(const corba_wf_desc_t &dag_desc, const char *cltMgrRef,
                    CORBA::Long wfReqId, CORBA::Boolean release);

  virtual CORBA::Long
  getWfReqId();

  virtual void
  releaseMultiDag(CORBA::Long wfReqId);

  virtual void
  cancelDag(CORBA::Long dagId);

  virtual void
  setPlatformType(MaDag::pfmType_t pfmType);

  virtual CORBA::Long
  ping();

protected:
  Forwarder_ptr forwarder;
  char *objName;
};

#endif // __MADAGFWDRIMPL__HH__
