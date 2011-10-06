/**
 * \file LogConnector.cc
 * \brief This file defines the log connector class implementation
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */


#include "LogConnector.hh"
#include "CorbaLogForwarder.hh"

LogConnector::LogConnector(ORBMgr* mgr):Connector(mgr) {

}

LogConnector::~LogConnector() {
  mmgr = NULL;
}

CORBA::Object_ptr
LogConnector::getObject(std::string ctxt, std::string name/*, CorbaForwarder_var* fwdr*/, string connectId, std::string globContext) {
  //  CorbaLogForwarder_var* fwd = (CorbaLogForwarder_var*)(fwdr);
  CorbaLogForwarder_var fwd = (CorbaLogForwarder_var)resolve<CorbaLogForwarder, CorbaLogForwarder_var>(FWRDCTXT, ctxt, connectId);
  try {
    if (globContext==LOGCOMPCTXT) {
      return (fwd)->getLogCentralComponent(name.c_str());
    }
    if (globContext==LOGTOOLCTXT) {
      return (fwd)->getLogCentralTool(name.c_str());
    }
    if (globContext==LOGTOOLMSGCTXT) {
      return (fwd)->getToolMsgReceiver(name.c_str());
    }
    if (globContext==LOGCOMPCONFCTXT) {
      return (fwd)->getCompoConf(name.c_str());
    }
  } catch(...) {
    return NULL;
  }
  return NULL;
}

std::string
LogConnector::getContext(std::string context) {
  // Enables the user to get the right context althouth the call is made with a secondary context
  if (context.compare(LOGCOMPCONFCTXT)==0) {
    return LOGCOMPCTXT;
  } else if (context.compare(LOGTOOLMSGCTXT)== 0) {
    return LOGTOOLCTXT;
  }
  return context;
}

void
LogConnector::registrer(std::string id) {
  // Register the connector in the log
  mname = id;
  mmgr->addConnector(this, id);
}

CORBA::Object_ptr 
LogConnector::resolveObject(const std::string& ctxt, const std::string& name,
	      const string& connectId,
	      const std::string& fwdName ) const {
  return mmgr->resolve<CorbaLogForwarder, 
    CorbaLogForwarder_var>(ctxt, 
			   name, 
			   connectId, 
			   fwdName); 
}
