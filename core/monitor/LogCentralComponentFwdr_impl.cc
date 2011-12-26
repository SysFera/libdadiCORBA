/**
 * @file LogCentralComponent_impl.cc
 *
 * @brief Implementation corresponding to the LogComponentComponent interface
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "LogCentralComponentFwdr_impl.hh"

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "utils/LocalTime.hh"
#include "LogOptions.hh"
#include "ORBMgr.hh"



using namespace std;


////////////////////////////////// FORWARDER IMPL


/****************************************************************************
 * LogCentralComponent_impl inplementation
 ****************************************************************************/

LogCentralComponentFwdrImpl::LogCentralComponentFwdrImpl(Forwarder_ptr fwdr,
							 const char* objName)
{
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

LogCentralComponentFwdrImpl::~LogCentralComponentFwdrImpl()
{
}

CORBA::Short
LogCentralComponentFwdrImpl::connectComponent(
  char*& componentName,
  const char* componentHostname,
  const char* message,
  const char* compConfigurator,
  const log_time_t& componentTime,
  tag_list_t& initialConfig)
{
  return forwarder->connectComponent(componentName, componentHostname, message,
				     compConfigurator, componentTime, initialConfig, objName);
}

CORBA::Short
LogCentralComponentFwdrImpl::disconnectComponent(const char* componentName,
						 const char* message)
{
  return forwarder->disconnectComponent(componentName, message, objName);
}

void
LogCentralComponentFwdrImpl::sendBuffer(const log_msg_buf_t& buffer)
{
  return forwarder->sendBuffer(buffer, objName);
}

void
LogCentralComponentFwdrImpl::ping(const char* componentName)
{
}

void
LogCentralComponentFwdrImpl::synchronize(const char* componentName,
					 const log_time_t& componentTime)
{
  return forwarder->synchronize(componentName, componentTime, objName);
}

void
LogCentralComponentFwdrImpl::test(){
}


ComponentConfiguratorFwdr_impl::ComponentConfiguratorFwdr_impl(Forwarder_ptr fwdr,
							       const char* objName){
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

ComponentConfiguratorFwdr_impl::~ComponentConfiguratorFwdr_impl(){

}

void
ComponentConfiguratorFwdr_impl::setTagFilter(const tag_list_t& tagList){
  return forwarder->setTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::addTagFilter(const tag_list_t& tagList){
  return forwarder->addTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::removeTagFilter(const tag_list_t& tagList){
  return forwarder->removeTagFilter (tagList, objName);
}

void
ComponentConfiguratorFwdr_impl::test(){
}
