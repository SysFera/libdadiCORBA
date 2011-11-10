/****************************************************************************/
/* LogToolBase implementation class                                         */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogToolBase.cc,v 1.4 2010/12/03 12:40:26 kcoulomb Exp $
 * $Log: LogToolBase.cc,v $
 * Revision 1.4  2010/12/03 12:40:26  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.3  2010/11/10 04:24:41  bdepardo
 * LogServiceT instead of LogService
 *
 * Revision 1.2  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "LogToolBase.hh"
#include "ORBTools.hh"
#include <iostream>
using namespace std;

ToolMsgReceiver_impl::ToolMsgReceiver_impl(LogToolBase* LTB)
{
  this->LTB = LTB;
}

ToolMsgReceiver_impl::~ToolMsgReceiver_impl()
{
}

void
ToolMsgReceiver_impl::sendMsg(const log_msg_buf_t& msgBuf)
{
  log_msg_t m;
  for (CORBA::ULong i = 0; i < msgBuf.length(); i++) {
    m = msgBuf[i];
    this->LTB->sendMsg(m);
  }
}

/****************************************************************************/

LogToolBase::LogToolBase(bool* success, int argc, char** argv,
                         unsigned int tracelevel, unsigned int port)
{
  *success = false;
  this->name = new char[1];
  *(this->name) = '\0';
  if (!ORBTools::init(argc, argv, tracelevel, port)) {
    return;
  }
  CORBA::Object* object;
  if (!ORBTools::findServant("LogServiceT", "", "LCT", "", object)) {
    cout << "Cannot contact the LogTool servant of LogService!\n";
    return;
  }
  this->LCTref = LogCentralTool::_narrow(object);
  this->TMRimpl = new ToolMsgReceiver_impl(this);
  if (!ORBTools::activateServant(this->TMRimpl)) {
    cout << "Cannot activate the ToolMsgReceiver servant !\n";
    return;
  }
  if (!ORBTools::activatePOA()) {
    return;
  }
  this->TMRref = this->TMRimpl->_this();
  //  this->TMRimpl = NULL;
  try { // To make sure that the object is callable
//    this->LCTref->test();
  } catch (...) {
    return;
  }
  *success = true;
}

LogToolBase::~LogToolBase()
{
  delete this->name;
  ORBTools::kill();
}

void
LogToolBase::setName(const char* name)
{
  delete this->name;
  this->name = strdup(name);
}

char*
LogToolBase::getName()
{
  return strdup(this->name);
}

short
LogToolBase::connect()
{
  short ret;
  try {
    ret = this->LCTref->connectTool(this->name, this->getName()); // Modif TMRref
  } catch (...) {
    ret = 1;
  }
  return ret;
}

short
LogToolBase::disconnect()
{
  short ret;
  try {
    ret = this->LCTref->disconnectTool(this->name);
  } catch (...) {
    ret = 1;
  }
  return ret;
}

short
LogToolBase::addFilter(const filter_t& filter)
{
  short ret;
  try {
    ret = this->LCTref->addFilter(this->name, filter);
  } catch (...) {
    ret = 1;
  }
  return ret;
}

short
LogToolBase::removeFilter(const char* filterName)
{
  short ret;
  try {
    ret = this->LCTref->removeFilter(this->name, filterName);
  } catch (...) {
    ret = 1;
  }
  return ret;
}

short
LogToolBase::flushAllFilters()
{
  short ret;
  try {
    ret = this->LCTref->flushAllFilters(this->name);
  } catch (...) {
    ret = 1;
  }
  return ret;
}

tag_list_t*
LogToolBase::getDefinedTags()
{
  tag_list_t* ret;
  try {
    ret = this->LCTref->getDefinedTags();
  } catch (...) {
    ret = NULL;
  }
  return ret;
}

component_list_t*
LogToolBase::getDefinedComponents()
{
  component_list_t* ret;
  try {
    ret = this->LCTref->getDefinedComponents();
  } catch (...) {
    ret = NULL;
  }
  return ret;
}

