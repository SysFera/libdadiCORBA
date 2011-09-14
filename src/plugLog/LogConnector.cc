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
LogConnector::getObject(std::string ctxt, std::string name, CorbaForwarder_var* fwdr) {
  CorbaLogForwarder_var* fwd = (CorbaLogForwarder_var*)(fwdr);
  try {
    if (ctxt==LOGCOMPCTXT) {
      return (*fwd)->getLogCentralComponent(name.c_str());
    }
    if (ctxt==LOGTOOLCTXT) {
      return (*fwd)->getLogCentralTool(name.c_str());
    }
    if (ctxt==LOGTOOLMSGCTXT) {
      return (*fwd)->getToolMsgReceiver(name.c_str());
    }
    if (ctxt==LOGCOMPCONFCTXT) {
      return (*fwd)->getCompoConf(name.c_str());
    }
  } catch(...) {
    return NULL;
  }
}

std::string
LogConnector::getContext(std::string context) {
  if (context.compare(LOGCOMPCONFCTXT)==0) {
    return LOGCOMPCTXT;
  } else if (context.compare(LOGTOOLMSGCTXT)== 0) {
    return LOGTOOLCTXT;
  }
  return context;
}

void
LogConnector::registrer(std::string id) {
  mname = id;
  mmgr->addConnector(this, id);
}
