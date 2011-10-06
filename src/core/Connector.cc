/**
 * \file Connector.cc
 * \brief This file defines the root connector class
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */

#include "Connector.hh"
#include "ORBMgr.hh"
#include "Forwarder.hh"
#include <stdexcept>

using namespace std;


Connector::Connector(ORBMgr* mgr) {
  mmgr = mgr;
}

Connector::~Connector() {
  mmgr = NULL;
}

CORBA::Object_ptr
Connector::getObject(std::string ctxt, std::string name/*, /*CorbaForwarder_var* v*/, string connectId, std::string globContext) {
  return NULL;
}

std::string
Connector::getContext(std::string context) {
  return "";
}

void
Connector::registrer(std::string id) {
  mmgr->addConnector(this, id);
  mname = id;
}

void
Connector::bind(const std::string& ctxt, const std::string& name,
		CORBA::Object_ptr object, const std::string& connect, const bool rebind ) const {
  return mmgr->bind(ctxt, name, object, connect, rebind);
}

void 
Connector::bind(const std::string& ctxt, const std::string& name,
	  const std::string& IOR, const std::string& connect, const bool rebind ) const {
  return mmgr->bind(ctxt, name, IOR, connect, rebind);
}

void 
Connector::rebind(const std::string& ctxt, const std::string& name,
		  CORBA::Object_ptr object, const std::string& connect) const {
  return mmgr->rebind(ctxt, name, object, connect);
}

void 
Connector::rebind(const std::string& ctxt, const std::string& name,
		  const std::string& IOR, const std::string& connect) const {
  return mmgr->rebind(ctxt, name, IOR, connect);
}

void 
Connector::unbind(const std::string& ctxt, const std::string& name) const {
  return mmgr->unbind(ctxt, name);
}
	
void
Connector::fwdsBind(const std::string& ctxt, const std::string& name,
		    const std::string& ior, const std::string& connectId,
		    const std::string& fwName ) const {
  return mmgr->fwdsBind(ctxt, name, ior, connectId, fwName);
}

void 
Connector::fwdsUnbind(const std::string& ctxt, const std::string& name,
	   const std::string& fwName ) const {
  return mmgr->fwdsUnbind(ctxt, name, fwName);
}
	
CORBA::Object_ptr 
Connector::resolveObject(const std::string& IOR) const {
  return mmgr->resolveObject(IOR);
}

	
std::list<std::string> 
Connector::list(CosNaming::NamingContext_var& ctxt) const {
  return mmgr->list(ctxt);
}


std::list<std::string> 
Connector::list(const std::string& ctxtName) const {
  return mmgr->list(ctxtName);
}
  
std::string 
Connector::getIOR(CORBA::Object_ptr object) const {
  return mmgr->getIOR(object);
}

std::string 
Connector::getIOR(const std::string& ctxt, const std::string& name) const {
  return mmgr->getIOR(ctxt, name, mname);
}
  
void 
Connector::activate(PortableServer::ServantBase* object) const {
  return mmgr->activate(object);
}

void
Connector::deactivate(PortableServer::ServantBase* object) const {
  return mmgr->deactivate(object);
}
  
void
Connector::wait() const{
  return mmgr->wait();
}

void 
Connector::shutdown(bool waitForCompletion) {
  return mmgr->shutdown(waitForCompletion);
}

void 
Connector::init(int argc, char* argv[]) {
  return mmgr->init(argc, argv);
}
  
ORBMgr* 
Connector::getMgr() {
  return mmgr->getMgr();
}
  
  /* IOR management functions. */
void 
Connector::makeIOR(const std::string& strIOR, IOP::IOR& ior) {
  return mmgr->makeIOR(strIOR, ior);
}

void 
Connector::makeString(const IOP::IOR& ior, std::string& strIOR) {
  return mmgr->makeString(ior, strIOR);
}

std::string 
Connector::getHost(IOP::IOR& ior) {
  return mmgr->getHost(ior);
}

std::string 
Connector::getHost(const std::string& strIOR) {
  return mmgr->getHost(strIOR);
}

unsigned int 
Connector::getPort(IOP::IOR& ior) {
  return mmgr->getPort(ior);
}

unsigned int 
Connector::getPort(const std::string& strIOR) {
  return mmgr->getPort(strIOR);
}


std::string 
Connector::getTypeID(IOP::IOR& ior) {
  return mmgr->getTypeID(ior);
}


std::string 
Connector::getTypeID(const std::string& strIOR) {
  return mmgr->getTypeID(strIOR);
}

std::string 
Connector::convertIOR(IOP::IOR& ior, const std::string& host,
	   const unsigned int port) {
  return mmgr->convertIOR(ior, host, port);
}


std::string 
Connector::convertIOR(const std::string& ior, const std::string& host,
	   const unsigned int port) {
  return mmgr->convertIOR(ior, host, port);  
}
  
void 
Connector::resetCache() const {
  return mmgr->resetCache();
}

void 
Connector::removeObjectFromCache(const std::string& name) const {
  return mmgr->removeObjectFromCache(name);
}

void 
Connector::removeObjectFromCache(const std::string& ctxt,
		      const std::string& name) const {
  return mmgr->removeObjectFromCache(ctxt, name);
}
 
void 
Connector::cleanCache() const {
  return mmgr->cleanCache();
}


void 
Connector::hexStringToBuffer(const char* ptr, const size_t size,
		  cdrMemoryStream& buffer) {
  return mmgr->hexStringToBuffer(ptr, size, buffer);
}
