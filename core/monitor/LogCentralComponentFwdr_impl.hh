/**
 * @file LogCentralComponent_impl.cc
 *
 * @brief Implementation corresponding to the LogComponentComponent interface
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _LOGCENTRALCOMPONENT_IMPL_HH_
#define _LOGCENTRALCOMPONENT_IMPL_HH_

#include "LogTypes.hh"
#include "LogComponent.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "utils/FullLinkedList.hh"

#include "CorbaForwarder.hh"


/**
 * Errorlevel constants for connectComponent (defined in the idl)
const short LS_OK
const short LS_COMPONENT_CONNECT_BADNAME
const short LS_COMPONENT_CONNECT_ALREADYEXISTS
const short LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR
const short LS_COMPONENT_CONNECT_INTERNALERROR
const short LS_COMPONENT_DISCONNECT_NOTEXISTS
 */


/**
 * @brief The implementation of the log component interface
 * @class LogCentralComponentFwdrImpl
 */
class LogCentralComponentFwdrImpl: public POA_LogCentralComponentFwdr,
				   public PortableServer::RefCountServantBase {
public:
  LogCentralComponentFwdrImpl(Forwarder_ptr fwdr,
			      const char *objName);

  ~LogCentralComponentFwdrImpl();

 CORBA::Short
  connectComponent(char*& componentName,
                   const char* componentHostname,
                   const char* message,
                   const char* compConfigurator,
                   const log_time_t& componentTime,
                   tag_list_t& initialConfig);

  CORBA::Short
  disconnectComponent(const char* componentName, const char* message);

  void
  sendBuffer(const log_msg_buf_t& buffer);

  void
  ping(const char* componentName);

  void
  test();


  void
  synchronize(const char* componentName, const log_time_t& componentTime);


protected:
	Forwarder_ptr forwarder;
	char* objName;

private:

}; // end class LogCentralComponentFwdr_impl


/**
 * @brief The implementation of the log component configurator
 * @class ComponentConfiguratorFwdr_impl
 */
class ComponentConfiguratorFwdr_impl: public POA_ComponentConfiguratorFwdr,
				   public PortableServer::RefCountServantBase {
public:
  ComponentConfiguratorFwdr_impl(Forwarder_ptr fwdr,
				 const char* objName);

  ~ComponentConfiguratorFwdr_impl();

  void setTagFilter(const tag_list_t& tagList);

  void addTagFilter(const tag_list_t& tagList);

  void removeTagFilter(const tag_list_t& tagList);

  void test();

protected:
	Forwarder_ptr forwarder;
	char* objName;

}; // end class LogCentralComponentFwdr_impl

#endif
