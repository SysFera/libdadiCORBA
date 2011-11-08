/**
* @file AgentImpl.hh
*
* @brief  DIET agent implementation source code
*
* @author
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.30  2011/02/15 16:21:27  bdepardo
 * removeElementClean is now virtual
 *
 * Revision 1.29  2010/12/17 09:47:59  kcoulomb
 * * Set diet to use the new log with forwarders
 * * Fix a CoRI problem
 * * Add library version remove DTM flag from ccmake because deprecated
 *
 * Revision 1.28  2010/11/24 15:18:08  bdepardo
 * searchData is now available on all agents. SeDs are now able to retrieve
 * a DAGDA data from an alias specified by a client.
 * Currently a SeD cannot declare an alias.
 *
 * Revision 1.27  2010/07/12 16:14:11  glemahec
 * DIET 2.5 beta 1 - Use the new ORB manager and allow the use of SSH-forwarders for all DIET CORBA objects
 *
 * Revision 1.26  2009/11/30 17:57:47  bdepardo
 * New methods to remove the agent in a cleaner way when killing it.
 *
 * Revision 1.25  2009/10/26 09:18:57  bdepardo
 * Added method for dynamic hierarchy management:
 * - childUnsubscribe(...)
 * - removeElement(bool recursive)
 * Renamed serverRemoveService(...) into childRemoveService(...)
 *
 * Revision 1.24  2008/11/18 10:15:22  bdepardo
 * - Added the possibility to dynamically create and destroy a service
 *   (even if the SeD is already started). An example is available.
 *   This feature only works with DAGDA.
 * - Added policy commands for CMake 2.6
 * - Added the name of the service in the profile. It was only present in
 *   the profile description, but not in the profile. Currently, the name is
 *   copied in each solve function, but this should certainly be moved
 *   somewhere else.
 *
 * Revision 1.23  2008/07/16 00:45:09  ecaron
 * Remove HAVE_ALTPREDICT (deprecated code)
 *
 * Revision 1.22  2008/04/28 07:08:30  glemahec
 * The DAGDA API.
 *
 * Revision 1.21  2008/04/19 09:16:45  ycaniou
 * Check that pathToTmp and pathToNFS exist
 * Check and eventually correct if pathToTmp or pathToNFS finish or not by '/'
 * Rewrite of the propagation of the request concerning job parallel_flag
 * Rewrite (and addition) of the propagation of the response concerning:
 *   job parallel_flag and serverType (batch or serial for the moment)
 * Complete debug info with batch stuff
 *
 * Revision 1.20  2008/04/07 15:33:43  ycaniou
 * This should remove all HAVE_BATCH occurences (still appears in the doc, which
 *   must be updated.. soon :)
 * Add the definition of DIET_BATCH_JOBID wariable in batch scripts
 *
 * Revision 1.19  2008/01/14 09:35:48  glemahec
 * AgentImpl.cc/hh modifications to allow the use of DAGDA.
 *
 * Revision 1.18  2007/04/16 22:43:43  ycaniou
 * Make all necessary changes to have the new option HAVE_ALT_BATCH operational.
 * This is indented to replace HAVE_BATCH.
 *
 * First draw to manage batch systems with a new Cori plug-in.
 *
 * Revision 1.17  2006/09/11 11:09:12  ycaniou
 * Call ServiceTable::getChildren(corba_pb_desc) in findServer, in order to
 *   call both parallel and sequential server for a default request that can
 *   possibly be executed in both modes.
 *
 * Revision 1.16  2005/09/07 07:41:02  hdail
 * Cleanup of alternative prediction handling
 *
 * Revision 1.15  2005/09/05 16:06:09  hdail
 * - Addition of method for aggregating parameter location data.
 *
 * Revision 1.14  2005/05/18 14:18:10  mjan
 * Initial adding of JuxMem support inside DIET. dmat_manips examples tested without JuxMem and with JuxMem
 *
 * Revision 1.13  2005/05/15 15:49:40  alsu
 * to indicate sucess/failure, addServices not returns a value
 *
 * Revision 1.12  2005/04/13 08:49:11  hdail
 * Beginning of adoption of new persistency model: DTM is enabled by default and
 * JuxMem will be supported via configure flags.  DIET will always provide at
 * least one type of persistency.  As a first step, persistency across DTM and
 * JuxMem is not supported so all persistency handling should be surrounded by
 *     #if HAVE_JUXMEM
 *         // JuxMem code
 *     #else
 *         // DTM code
 *     #endif
 * This check-in prepares for the JuxMem check-in by cleaning up old
 * DEVELOPPING_DATA_PERSISTENCY flags and surrounding DTM code with
 * #if ! HAVE_JUXMEM / #endif flags to be replaced by above format by Mathieu's
 * check-in.  Currently the HAVE_JUXMEM flag is set in AgentImpl.hh - to be
 * replaced by Mathieu's check-in of a configure system for JuxMem.
 *
 * Revision 1.11  2005/04/08 13:02:43  hdail
 * The code for LogCentral has proven itself stable and it seems bug free.
 * Since no external libraries are required to compile in LogCentral, its now
 * going to be compiled in by default always ... its usage is easily controlled by
 * configuration file.
 *
 * Revision 1.10  2004/09/29 13:35:31  sdahan
 * Add the Multi-MAs feature.
 *
 * Revision 1.9  2004/09/13 14:11:06  hdail
 * Cleaned up memory management for local variables localHostName and myName.
 *
 * Revision 1.8  2004/06/11 15:45:39  ctedesch
 * add DIET/JXTA
 *
 * Revision 1.7  2004/03/01 18:43:57  rbolze
 * add logservice
 ****************************************************************************/


#ifndef _AGENTIMPL_HH_
#define _AGENTIMPL_HH_

#include "Agent.hh"
#include "LocalAgent.hh"


#ifdef USE_LOG_SERVICE
#include "DietLogComponent.hh"
#endif


// Forwarder part
#include "Forwarder.hh"
#include "AgentFwdr.hh"


class AgentFwdrImpl : public POA_AgentFwdr,
                      public PortableServer::RefCountServantBase {
public:
  AgentFwdrImpl(Forwarder_ptr fwdr, const char* objName);

  virtual CORBA::Long
  agentSubscribe(const char* me, const char* hostName,
                 const SeqCorbaProfileDesc_t& services);

  virtual CORBA::Long
  serverSubscribe(const char* me, const char* hostName,
                  const SeqCorbaProfileDesc_t& services);
  virtual CORBA::Long
  childUnsubscribe(CORBA::ULong childID,
                   const SeqCorbaProfileDesc_t& services);

//  virtual CORBA::Long
//  removeElement(bool recursive);

  virtual CORBA::Long
  childRemoveService(CORBA::ULong childID,
                     const corba_profile_desc_t& profile);

  virtual char*
  getDataManager();

  SeqString*
  searchData(const char* request);

  virtual CORBA::Long
  addServices(CORBA::ULong myID, const SeqCorbaProfileDesc_t& services);

  virtual void
  getResponse(const corba_response_t& resp);

  virtual CORBA::Long
  ping();

  virtual char*
  getHostname();

protected:
  Forwarder_ptr forwarder;
  char* objName;
};
#endif  // _AGENTIMPL_HH_
