/**
* @file  LocalAgentImpl.hh
*
* @brief  DIET local agent implementation header
*
* @author - Philippe COMBES (Philippe.Combes@ens-lyon.fr)
*         - Sylvain DAHAN (Sylvain.Dahan@lifc.univ-fcomte.fr)
*         - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.11  2011/02/15 16:21:50  bdepardo
 * Added a new method: removeElementClean
 *
 * Revision 1.10  2010/12/17 09:47:59  kcoulomb
 * * Set diet to use the new log with forwarders
 * * Fix a CoRI problem
 * * Add library version remove DTM flag from ccmake because deprecated
 *
 * Revision 1.9  2010/11/24 15:18:08  bdepardo
 * searchData is now available on all agents. SeDs are now able to retrieve
 * a DAGDA data from an alias specified by a client.
 * Currently a SeD cannot declare an alias.
 *
 * Revision 1.8  2010/07/12 16:14:11  glemahec
 * DIET 2.5 beta 1 - Use the new ORB manager and allow the use of SSH-forwarders for all DIET CORBA objects
 *
 * Revision 1.7  2009/10/26 09:17:37  bdepardo
 * Added methods for dynamic hierarchy management:
 * - bindParent(const char * parentName)
 * - disconnect()
 * - removeElement(bool recursive)
 * Renamed serverRemoveService(...) into childRemoveService(...)
 *
 * Revision 1.6  2008/11/18 10:15:23  bdepardo
 * - Added the possibility to dynamically create and destroy a service
 *   (even if the SeD is already started). An example is available.
 *   This feature only works with DAGDA.
 * - Added policy commands for CMake 2.6
 * - Added the name of the service in the profile. It was only present in
 *   the profile description, but not in the profile. Currently, the name is
 *   copied in each solve function, but this should certainly be moved
 *   somewhere else.
 *
 * Revision 1.5  2005/05/15 15:51:15  alsu
 * to indicate sucess/failure, addServices not returns a value
 *
 * Revision 1.4  2003/09/22 21:19:49  pcombes
 * Set all the modules and their interfaces for data persistency.
 *
 * Revision 1.2  2003/05/10 08:53:34  pcombes
 * New format for configuration files, new Parsers.
 *
 * Revision 1.1  2003/04/10 13:01:51  pcombes
 * Replace LocalAgent_impl.hh. Apply CS. Remove createData.
 ****************************************************************************/


#ifndef _LOCALAGENTIMPL_HH_
#define _LOCALAGENTIMPL_HH_

#include "Agent.hh"

#include "Forwarder.hh"
#include "LocalAgentFwdr.hh"


class LocalAgentFwdrImpl : public POA_LocalAgentFwdr,
                           public PortableServer::RefCountServantBase {
public:
  LocalAgentFwdrImpl(Forwarder_ptr fwdr, const char* objName);

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

  CORBA::Long
  bindParent(const char * parentName);

  CORBA::Long
  disconnect();

  virtual void
  getRequest(const corba_request_t& req);

  virtual void
  getResponse(const corba_response_t& resp);

  virtual CORBA::Long
  ping();

  virtual char*
  getHostname();

  virtual CORBA::Long
  addServices(CORBA::ULong myID, const SeqCorbaProfileDesc_t& services);

  virtual CORBA::Long
  childRemoveService(CORBA::ULong childID,
                     const corba_profile_desc_t& profile);

  virtual char*
  getDataManager();

  virtual SeqString*
  searchData(const char* request);

private:
  Forwarder_ptr forwarder;
  char* objName;
};

#endif  // _LOCALAGENTIMPL_HH_