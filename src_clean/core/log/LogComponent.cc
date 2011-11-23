/**
* @file LogComponent.cc
*
* @brief  DIET forwarder implementation - Log component forwarder implementation
*
* @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/
/* $Id$
 * $Log$
 * Revision 1.4  2010/07/27 16:16:49  glemahec
 * Forwarders robustness
 *
 * Revision 1.3  2010/07/14 23:45:30  bdepardo
 * Header corrections
 *
 * Revision 1.2  2010/07/13 15:24:13  glemahec
 * Warnings corrections and some robustness improvements
 *
 * Revision 1.1  2010/07/12 16:08:56  glemahec
 * DIET 2.5 beta 1 - Forwarder implementations
 ****************************************************************************/

#include "CorbaForwarder.hh"
#include "ORBMgr.hh"
#include <string>

void
CorbaForwarder::setTagFilter(const ::tag_list_t& tagList, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->setTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->
    resolve<ComponentConfigurator, ComponentConfigurator_var>(LOGCOMPCTXT,
                                                              name,
                                                              this->name);
  return cfg->setTagFilter(tagList);
}

void
CorbaForwarder::addTagFilter(const ::tag_list_t& tagList, const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->addTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->
    resolve<ComponentConfigurator, ComponentConfigurator_var>(LOGCOMPCTXT,
                                                              name,
                                                              this->name);
  return cfg->addTagFilter(tagList);
}

void
CorbaForwarder::removeTagFilter(const ::tag_list_t& tagList,
                               const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->removeTagFilter(tagList, objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->
    resolve<ComponentConfigurator, ComponentConfigurator_var>(LOGCOMPCTXT,
                                                              name,
                                                              this->name);
  return cfg->removeTagFilter(tagList);
}

void
CorbaForwarder::test(const char* objName) {
  std::string objString(objName);
  std::string name;

  if (!remoteCall(objString)) {
    return getPeer()->test(objString.c_str());
  }

  name = getName(objString);

  ComponentConfigurator_var cfg =
    ORBMgr::getMgr()->
    resolve<ComponentConfigurator, ComponentConfigurator_var>(LOGCOMPCTXT,
                                                              name,
                                                              this->name);
  return cfg->test();
}
