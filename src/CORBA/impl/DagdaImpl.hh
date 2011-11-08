/**
* @file DagdaImpl.hh
*
* @brief Dagda component implementation
*
* @author Gael Le Mahec (lemahec@clermont.in2p3.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.35  2011/03/18 08:43:32  hguemar
 * fix memory leak in DagdaImpl: getID() method returns a CORBA::String which is not always deallocated (patch from Gael Le Mahec)
 *
 * Revision 1.34  2011/03/16 22:02:34  bdepardo
 * Unbind dagda element in destructor
 *
 * Revision 1.33  2011/03/03 00:29:46  bdepardo
 * Add missing include
 *
 * Revision 1.32  2011/03/03 00:23:10  bdepardo
 * Resolved a few fix me
 *
 * Revision 1.31  2010/12/17 09:48:01  kcoulomb
 * * Set diet to use the new log with forwarders
 * * Fix a CoRI problem
 * * Add library version remove DTM flag from ccmake because deprecated
 *
 * Revision 1.30  2010/10/15 02:38:55  bdepardo
 * Bug correction in INOUT data
 *
 * Revision 1.29  2010/08/04 09:06:20  glemahec
 * Parallel compilation
 *
 * Revision 1.28  2010/07/12 16:14:12  glemahec
 * DIET 2.5 beta 1 - Use the new ORB manager and allow the use of SSH-forwarders for all DIET CORBA objects
 *
 * Revision 1.27  2009/10/26 09:11:26  bdepardo
 * Added method for dynamically managing the hierarchy:
 * - void subscribeParent(const char * parentID)
 * - void unsubscribeParent()
 *
 * Revision 1.26  2009/04/17 08:50:49  bisnard
 * added handling of container empty elements
 *
 * Revision 1.25  2009/03/27 09:09:41  bisnard
 * replace container size attr by dynamic value
 *
 * Revision 1.24  2009/01/16 13:36:13  bisnard
 * changed Container constructor signature
 * modified scope of getDataRelationMgr to protected
 * lock container during element addition
 * check lock before sending container
 *
 * Revision 1.23  2008/12/09 12:06:21  bisnard
 * changed container download method to transfer only the list of IDs
 * (each container element must be downloaded separately)
 *
 * Revision 1.22  2008/11/07 14:32:14  bdepardo
 * Headers correction
 *
 *
 ***********************************************************/

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
#include "DIET_data.h"
#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif

#include "Forwarder.hh"
#include "DagdaFwdr.hh"
#include "debug.hh"

typedef enum {DGD_CLIENT_MNGR,
              DGD_AGENT_MNGR,
              DGD_SED_MNGR} dagda_manager_type_t;


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
  Forwarder_ptr forwarder;
  char* objName;
};
#endif
