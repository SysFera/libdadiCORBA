/**
 * @file LogComponent.cc
 *
 * @brief  DIET forwarder implementation - Log component forwarder implementation
 *
 * @author - Gael Le Mahec   (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *
 * @section Licence
 *   |LICENSE|
 */

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
