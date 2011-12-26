/**
 * @file DagdaImpl.hh
 *
 * @brief Dagda component implementation
 *
 * @author Gael Le Mahec (lemahec@clermont.in2p3.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _DAGDAIMPL_HH_
#define _DAGDAIMPL_HH_
#define MAXBUFFSIZE  (getMaxMsgSize() == 0 ? 4294967295U:getMaxMsgSize())

#include <fstream>
#include <iterator>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <omnithread.h>
#include "Dagda.hh"
#include "common_types.hh"
#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif

#include "Forwarder.hh"
#include "DagdaFwdr.hh"

typedef enum {DGD_CLIENT_MNGR,
              DGD_AGENT_MNGR,
              DGD_SED_MNGR} dagda_manager_type_t;

/**
 * @brief Dagda forwarder implementation
 * @class DagdaFwdrImpl
 */
class DagdaFwdrImpl : public POA_DagdaFwdr,
                      public PortableServer::RefCountServantBase {
public:
  DagdaFwdrImpl(Forwarder_ptr fwdr, const char* objName);

  virtual void
  subscribe(const char* name);

  virtual void
  unsubscribe(const char* name);

  virtual void
  subscribeParent(const char * parentID);

  virtual void
  unsubscribeParent();

  /* ------------ */
  virtual char*
  writeFile(const SeqChar& data, const char* basename,
            CORBA::Boolean replace);

  virtual char*
  sendFile(const corba_data_t &data, const char* dest);

  virtual char*
  recordData(const SeqChar& data, const corba_data_desc_t& dataDesc,
             CORBA::Boolean replace, CORBA::Long offset);

  virtual char*
  sendData(const char* ID, const char* dest);

  virtual char*
  sendContainer(const char* containerID, const char* dest,
                CORBA::Boolean sendElements);
  virtual void
  lockData(const char* dataID);

  virtual void
  unlockData(const char* dataID);

  virtual Dagda::dataStatus
  getDataStatus(const char* dataID);

  /* ------------ */


  virtual CORBA::Boolean
  lclIsDataPresent(const char* dataID);

  virtual CORBA::Boolean
  lvlIsDataPresent(const char* dataID);

  virtual CORBA::Boolean
  pfmIsDataPresent(const char* dataID);

  virtual void
  lclAddData(const char* src, const corba_data_t& data);

  virtual void
  lvlAddData(const char* src, const corba_data_t& data);

  virtual void
  pfmAddData(const char* src, const corba_data_t& data);

  virtual void
  registerFile(const corba_data_t& data);

  virtual void
  lclAddContainerElt(const char* containerID,
                     const char* dataID,
                     CORBA::Long index,
                     CORBA::Long flag);

  virtual CORBA::Long
  lclGetContainerSize(const char* containerID);

  virtual void
  lclGetContainerElts(const char* containerID,
                      SeqString& dataIDSeq,
                      SeqLong& flagSeq,
                      CORBA::Boolean ordered);

  virtual void
  lclRemData(const char* dataID);

  virtual void
  lvlRemData(const char* dataID);

  virtual void
  pfmRemData(const char* dataID);

  virtual void
  lclUpdateData(const char* src, const corba_data_t& data);

  virtual void
  lvlUpdateData(const char* src, const corba_data_t& data);

  virtual void
  pfmUpdateData(const char* src, const corba_data_t& data);

  virtual void
  lclReplicate(const char* dataID, CORBA::Long target,
               const char* pattern, bool replace);

  virtual void
  lvlReplicate(const char* dataID, CORBA::Long target,
               const char* pattern, bool replace);

  virtual void
  pfmReplicate(const char* dataID, CORBA::Long target,
               const char* pattern, bool replace);

  virtual SeqCorbaDataDesc_t*
  lclGetDataDescList();

  virtual SeqCorbaDataDesc_t*
  lvlGetDataDescList();

  virtual SeqCorbaDataDesc_t*
  pfmGetDataDescList();

  virtual corba_data_desc_t*
  lclGetDataDesc(const char* dataID);

  virtual corba_data_desc_t*
  lvlGetDataDesc(const char* dataID);

  virtual corba_data_desc_t*
  pfmGetDataDesc(const char* dataID);

  virtual SeqString*
  lvlGetDataManagers(const char* dataID);

  virtual SeqString*
  pfmGetDataManagers(const char* dataID);

  virtual char*
  getBestSource(const char* dest, const char* dataID);

  virtual char*
  getID();

  virtual void
  checkpointState();

  virtual char*
  getHostname();

private:
  Forwarder_ptr mforwarder;
  char* mobjName;
};
#endif
