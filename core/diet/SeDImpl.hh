/**
 * @file SeDImpl.hh
 *
 * @brief  DIET SeD implementation header
 *
 * @author
 *         - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
 *         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */


#ifndef _SEDIMPL_HH_
#define _SEDIMPL_HH_

#include "SeD.hh"


#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif

#include "dagda/DagdaImpl.hh"

#if HAVE_ALT_BATCH
extern "C" {
#include "DIET_server.h" /* For inclusion of diet_server_status_t */
}
#include "BatchSystem.hh"
#endif
#if HAVE_SEDSCHEDULER
#include "SeDScheduler.hh"
#endif

#include "Forwarder.hh"
#include "SeDFwdr.hh"


/**
 * @brief The SeD forwarder class that implements all the SeD methods
 * throught the forwarder
 * @class SeDFwdrImpl
 * For non documented methods, please see the SeD and its parents idl interfaces.
 */
class SeDFwdrImpl : public POA_SeD,
                    public PortableServer::RefCountServantBase {
public:
  SeDFwdrImpl(Forwarder_ptr fwdr, const char* objName);

  virtual CORBA::Long
  ping();

  virtual CORBA::Long
  bindParent(const char * parentName);

  virtual CORBA::Long
  disconnect();

  virtual CORBA::Long
  removeElement();

  virtual void
  getRequest(const corba_request_t& req);

  virtual CORBA::Long
  checkContract(corba_estimation_t& estimation,
                const corba_pb_desc_t& pb);

  virtual void
  updateTimeSinceLastSolve();

  virtual CORBA::Long
  solve(const char* pbName, corba_profile_t& pb);

  virtual void
  solveAsync(const char* pb_name, const corba_profile_t& pb,
             const char * volatileclientIOR);

  virtual char*
  getDataMgrID();

  virtual SeqCorbaProfileDesc_t*
  getSeDProfiles(CORBA::Long& length);

protected:
  Forwarder_ptr mforwarder;
  char* mobjName;
};

#endif  // _SED_IMPL_HH_
