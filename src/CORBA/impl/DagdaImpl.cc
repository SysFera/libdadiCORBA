

#include <cerrno>

#include <list>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iterator>

#include <unistd.h>

#include "Dagda.hh"
#include "common_types.hh"

#include "ORBMgr.hh"
#include "debug.hh"

#include "DagdaImpl.hh"
#if DAGDA_PROGRESSION
#include "Transfers.hh"
#endif



/**  FNM_CASEFOLD for AIX (available only in AIX 5.1 or later). */
#if __aix__
#ifndef FNM_CASEFOLD
#define FNM_CASEFOLD FNM_IGNORECASE
#endif  // !FNM_CASEFOLD
#endif  // __aix__


/* Forwarder part*/

DagdaFwdrImpl::DagdaFwdrImpl(Forwarder_ptr fwdr, const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

void
DagdaFwdrImpl::subscribe(const char* name) {
  forwarder->subscribe(name, objName);
}

void
DagdaFwdrImpl::unsubscribe(const char* name) {
  forwarder->unsubscribe(name, objName);
}

void
DagdaFwdrImpl::subscribeParent(const char * parentID) {
  forwarder->subscribeParent(parentID, objName);
}

void
DagdaFwdrImpl::unsubscribeParent() {
  forwarder->unsubscribeParent(objName);
}


char*
DagdaFwdrImpl::writeFile(const SeqChar& data, const char* basename,
                         CORBA::Boolean replace) {
  return forwarder->writeFile(data, basename, replace, objName);
}

char*
DagdaFwdrImpl::sendFile(const corba_data_t &data, const char* dest) {
  return forwarder->sendFile(data, dest, objName);
}

char*
DagdaFwdrImpl::recordData(const SeqChar& data, const corba_data_desc_t& dataDesc,
                          CORBA::Boolean replace, CORBA::Long offset) {
  return forwarder->recordData(data, dataDesc, replace, offset, objName);
}

char*
DagdaFwdrImpl::sendData(const char* ID, const char* dest) {
  return forwarder->sendData(ID, dest, objName);
}

char*
DagdaFwdrImpl::sendContainer(const char* containerID, const char* dest,
                             CORBA::Boolean sendElements) {
  return forwarder->sendContainer(containerID, dest, sendElements, objName);
}

void
DagdaFwdrImpl::lockData(const char* dataID) {
  forwarder->lockData(dataID, objName);
}

void
DagdaFwdrImpl::unlockData(const char* dataID) {
  forwarder->unlockData(dataID, objName);
}

//Dagda::dataStatus DagdaFwdrImpl::getDataStatus(const char* dataID) {
//  return forwarder->getDataStatus(dataID, objName);
//}

CORBA::Boolean DagdaFwdrImpl::lclIsDataPresent(const char* dataID) {
  return forwarder->lclIsDataPresent(dataID, objName);
}

CORBA::Boolean DagdaFwdrImpl::lvlIsDataPresent(const char* dataID) {
  return forwarder->lvlIsDataPresent(dataID, objName);
}

CORBA::Boolean DagdaFwdrImpl::pfmIsDataPresent(const char* dataID) {
  return forwarder->pfmIsDataPresent(dataID, objName);
}

void
DagdaFwdrImpl::lclAddData(const char* src, const corba_data_t& data) {
  forwarder->lclAddData(src, data, objName);
}

void
DagdaFwdrImpl::lvlAddData(const char* src, const corba_data_t& data) {
  forwarder->lvlAddData(src, data, objName);
}

void
DagdaFwdrImpl::pfmAddData(const char* src, const corba_data_t& data) {
  forwarder->pfmAddData(src, data, objName);
}

void
DagdaFwdrImpl::registerFile(const corba_data_t& data) {
  forwarder->registerFile(data, objName);
}

void
DagdaFwdrImpl::lclAddContainerElt(const char* containerID,
                                  const char* dataID,
                                  CORBA::Long index,
                                  CORBA::Long flag)
{
  forwarder->lclAddContainerElt(containerID, dataID, index, flag, objName);
}

CORBA::Long DagdaFwdrImpl::lclGetContainerSize(const char* containerID) {
  return forwarder->lclGetContainerSize(containerID, objName);
}

void
DagdaFwdrImpl::lclGetContainerElts(const char* containerID,
                                   SeqString& dataIDSeq,
                                   SeqLong& flagSeq,
                                   CORBA::Boolean ordered)
{
  forwarder->lclGetContainerElts(containerID, dataIDSeq, flagSeq, ordered, objName);
}

void
DagdaFwdrImpl::lclRemData(const char* dataID) {
  forwarder->lclRemData(dataID, objName);
}

void
DagdaFwdrImpl::lvlRemData(const char* dataID) {
  forwarder->lvlRemData(dataID, objName);
}

void
DagdaFwdrImpl::pfmRemData(const char* dataID) {
  forwarder->pfmRemData(dataID, objName);
}

void
DagdaFwdrImpl::lclUpdateData(const char* src, const corba_data_t& data) {
  forwarder->lclUpdateData(src, data, objName);
}

void
DagdaFwdrImpl::lvlUpdateData(const char* src, const corba_data_t& data) {
  forwarder->lvlUpdateData(src, data, objName);
}

void
DagdaFwdrImpl::pfmUpdateData(const char* src, const corba_data_t& data) {
  forwarder->pfmUpdateData(src, data, objName);
}

void
DagdaFwdrImpl::lclReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  forwarder->lclReplicate(dataID, target, pattern, replace, objName);
}

void
DagdaFwdrImpl::lvlReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  forwarder->lvlReplicate(dataID, target, pattern, replace, objName);
}

void
DagdaFwdrImpl::pfmReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  forwarder->pfmReplicate(dataID, target, pattern, replace, objName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::lclGetDataDescList() {
  return forwarder->lclGetDataDescList(objName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::lvlGetDataDescList() {
  return forwarder->lvlGetDataDescList(objName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::pfmGetDataDescList() {
  return forwarder->pfmGetDataDescList(objName);
}

corba_data_desc_t* DagdaFwdrImpl::lclGetDataDesc(const char* dataID) {
  return forwarder->lclGetDataDesc(dataID, objName);
}

corba_data_desc_t*
DagdaFwdrImpl::lvlGetDataDesc(const char* dataID) {
  return forwarder->lvlGetDataDesc(dataID, objName);
}

corba_data_desc_t*
DagdaFwdrImpl::pfmGetDataDesc(const char* dataID) {
  return forwarder->pfmGetDataDesc(dataID, objName);
}

SeqString*
DagdaFwdrImpl::lvlGetDataManagers(const char* dataID) {
  return forwarder->lvlGetDataManagers(dataID, objName);
}

SeqString*
DagdaFwdrImpl::pfmGetDataManagers(const char* dataID) {
  return forwarder->pfmGetDataManagers(dataID, objName);
}

char*
DagdaFwdrImpl::getBestSource(const char* dest, const char* dataID) {
  return forwarder->getBestSource(dest, dataID, objName);
}

char*
DagdaFwdrImpl::getID() {
  return forwarder->getID(objName);
}

void
DagdaFwdrImpl::checkpointState() {
  forwarder->checkpointState(objName);
}

char*
DagdaFwdrImpl::getHostname() {
  return forwarder->getHostname(objName);
}
