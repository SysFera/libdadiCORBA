/**
 * \file LogConnector.hh
 * \brief This file defines the log connector class realization
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */

#ifndef LOGCONNECTOR_HH
#define LOGCONNECTOR_HH

#include "ORBMgr.hh"
#include "Connector.hh"

#define LOGCOMPCTXT "LogServiceC" //"LogService component"
#define LOGTOOLCTXT "LogServiceT" //"LogService tool"
#define LOGCOMPCONFCTXT "LogServiceC2" //"LogService component"
#define LOGTOOLMSGCTXT "LogServiceT2" //"LogService tool"

class LogConnector;

class LogConnector: public Connector {

public :
  /**
   * \brief Constructor
   */
  LogConnector(ORBMgr* mgr);
  /**
   * \brief Destructor
   */
  ~LogConnector();
  /**
   * \brief To register the connector in an ORBMgr
   * \param id: The id to use to register
   */
  void
  registrer(std::string id);
  /**
   * \brief To get the object corresponding to a context and a name in the omninames, using the log forwarder item
   * \param ctxt: The context
   * \param name: The name of the object
   * \param connectId: The id of the connector from the call
   * \return The corresponding corba object
   */
  virtual CORBA::Object_ptr
  getObject(std::string ctxt, std::string name/*, CorbaForwarder_var* fwdr*/, string connectId);
  /**
   * \brief To get the context from a given context (sometimes a change of context is needed)
   */
  virtual std::string
  getContext(std::string context);

  virtual CORBA::Object_ptr resolveObject(const std::string& ctxt, const std::string& name,
				  const string& connectId,
				  const std::string& fwdName = "") const;

protected :
private :

} ; //end class connector

#endif
