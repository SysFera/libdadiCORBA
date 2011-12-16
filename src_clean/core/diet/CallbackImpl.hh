/**
 * @file  CallbackImpl.hh
 *
 * @brief   POA callback implementation
 *
 * @author  - Christophe PERA (christophe.pera@ens-lyon.fr)
 *          - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _CALLBACKIMPL_H_
#define _CALLBACKIMPL_H_

#include "Callback.hh"

#include "Forwarder.hh"
#include "CallbackFwdr.hh"


/**
 * @brief The callback forwarder class that implements all the callback methods
 * throught the forwarder
 * @class CallbackImpl
 * For non documented methods, please see the \ref CallbackIDL and its parents idl interfaces.
 */
class CallbackFwdrImpl : public POA_CallbackFwdr,
                         public PortableServer::RefCountServantBase {
public:
  CallbackFwdrImpl(Forwarder_ptr fwdr, const char* objName);

  virtual CORBA::Long
  ping();

  virtual CORBA::Long
  notifyResults(const char * path,
                const corba_profile_t& pb,
                CORBA::Long reqID);

  virtual CORBA::Long
  solveResults(const corba_profile_t& pb,
               CORBA::Long reqID,
               CORBA::Long solve_res);

private:
  Forwarder_ptr mforwarder;
  char* mobjName;
};

#endif
