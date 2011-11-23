

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
  this->mforwarder = Forwarder::_duplicate(fwdr);
  this->mobjName = CORBA::string_dup(objName);
}

void
DagdaFwdrImpl::subscribe(const char* name) {
  mforwarder->subscribe(name, mobjName);
}

void
DagdaFwdrImpl::unsubscribe(const char* name) {
  mforwarder->unsubscribe(name, mobjName);
}

void
DagdaFwdrImpl::subscribeParent(const char * parentID) {
  mforwarder->subscribeParent(parentID, mobjName);
}

void
DagdaFwdrImpl::unsubscribeParent() {
  mforwarder->unsubscribeParent(mobjName);
}


char*
DagdaFwdrImpl::writeFile(const SeqChar& data, const char* basename,
                         CORBA::Boolean replace) {
  return mforwarder->writeFile(data, basename, replace, mobjName);
}

char*
DagdaFwdrImpl::sendFile(const corba_data_t &data, const char* dest) {
  return mforwarder->sendFile(data, dest, mobjName);
}

char*
DagdaFwdrImpl::recordData(const SeqChar& data, const corba_data_desc_t& dataDesc,
                          CORBA::Boolean replace, CORBA::Long offset) {
  return mforwarder->recordData(data, dataDesc, replace, offset, mobjName);
}

char*
DagdaFwdrImpl::sendData(const char* ID, const char* dest) {
  return mforwarder->sendData(ID, dest, mobjName);
}

char*
DagdaFwdrImpl::sendContainer(const char* containerID, const char* dest,
                             CORBA::Boolean sendElements) {
  return mforwarder->sendContainer(containerID, dest, sendElements, mobjName);
}

void
DagdaFwdrImpl::lockData(const char* dataID) {
  mforwarder->lockData(dataID, mobjName);
}

void
DagdaFwdrImpl::unlockData(const char* dataID) {
  mforwarder->unlockData(dataID, mobjName);
}

Dagda::dataStatus DagdaFwdrImpl::getDataStatus(const char* dataID) {
  return mforwarder->getDataStatus(dataID, mobjName);
}

CORBA::Boolean DagdaFwdrImpl::lclIsDataPresent(const char* dataID) {
  return mforwarder->lclIsDataPresent(dataID, mobjName);
}

CORBA::Boolean DagdaFwdrImpl::lvlIsDataPresent(const char* dataID) {
  return mforwarder->lvlIsDataPresent(dataID, mobjName);
}

CORBA::Boolean DagdaFwdrImpl::pfmIsDataPresent(const char* dataID) {
  return mforwarder->pfmIsDataPresent(dataID, mobjName);
}

void
DagdaFwdrImpl::lclAddData(const char* src, const corba_data_t& data) {
  mforwarder->lclAddData(src, data, mobjName);
}

void
DagdaFwdrImpl::lvlAddData(const char* src, const corba_data_t& data) {
  mforwarder->lvlAddData(src, data, mobjName);
}

void
DagdaFwdrImpl::pfmAddData(const char* src, const corba_data_t& data) {
  mforwarder->pfmAddData(src, data, mobjName);
}

void
DagdaFwdrImpl::registerFile(const corba_data_t& data) {
  mforwarder->registerFile(data, mobjName);
}

void
DagdaFwdrImpl::lclAddContainerElt(const char* containerID,
                                  const char* dataID,
                                  CORBA::Long index,
                                  CORBA::Long flag)
{
  mforwarder->lclAddContainerElt(containerID, dataID, index, flag, mobjName);
}

CORBA::Long DagdaFwdrImpl::lclGetContainerSize(const char* containerID) {
  return mforwarder->lclGetContainerSize(containerID, mobjName);
}

void
DagdaFwdrImpl::lclGetContainerElts(const char* containerID,
                                   SeqString& dataIDSeq,
                                   SeqLong& flagSeq,
                                   CORBA::Boolean ordered)
{
  mforwarder->lclGetContainerElts(containerID, dataIDSeq, flagSeq, ordered, mobjName);
}

void
DagdaFwdrImpl::lclRemData(const char* dataID) {
  mforwarder->lclRemData(dataID, mobjName);
}

void
DagdaFwdrImpl::lvlRemData(const char* dataID) {
  mforwarder->lvlRemData(dataID, mobjName);
}

void
DagdaFwdrImpl::pfmRemData(const char* dataID) {
  mforwarder->pfmRemData(dataID, mobjName);
}

void
DagdaFwdrImpl::lclUpdateData(const char* src, const corba_data_t& data) {
  mforwarder->lclUpdateData(src, data, mobjName);
}

void
DagdaFwdrImpl::lvlUpdateData(const char* src, const corba_data_t& data) {
  mforwarder->lvlUpdateData(src, data, mobjName);
}

void
DagdaFwdrImpl::pfmUpdateData(const char* src, const corba_data_t& data) {
  mforwarder->pfmUpdateData(src, data, mobjName);
}

void
DagdaFwdrImpl::lclReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  mforwarder->lclReplicate(dataID, target, pattern, replace, mobjName);
}

void
DagdaFwdrImpl::lvlReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  mforwarder->lvlReplicate(dataID, target, pattern, replace, mobjName);
}

void
DagdaFwdrImpl::pfmReplicate(const char* dataID, CORBA::Long target,
                            const char* pattern, bool replace)
{
  mforwarder->pfmReplicate(dataID, target, pattern, replace, mobjName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::lclGetDataDescList() {
  return mforwarder->lclGetDataDescList(mobjName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::lvlGetDataDescList() {
  return mforwarder->lvlGetDataDescList(mobjName);
}

SeqCorbaDataDesc_t* DagdaFwdrImpl::pfmGetDataDescList() {
  return mforwarder->pfmGetDataDescList(mobjName);
}

corba_data_desc_t* DagdaFwdrImpl::lclGetDataDesc(const char* dataID) {
  return mforwarder->lclGetDataDesc(dataID, mobjName);
}

corba_data_desc_t*
DagdaFwdrImpl::lvlGetDataDesc(const char* dataID) {
  return mforwarder->lvlGetDataDesc(dataID, mobjName);
}

corba_data_desc_t*
DagdaFwdrImpl::pfmGetDataDesc(const char* dataID) {
  return mforwarder->pfmGetDataDesc(dataID, mobjName);
}

SeqString*
DagdaFwdrImpl::lvlGetDataManagers(const char* dataID) {
  return mforwarder->lvlGetDataManagers(dataID, mobjName);
}

SeqString*
DagdaFwdrImpl::pfmGetDataManagers(const char* dataID) {
  return mforwarder->pfmGetDataManagers(dataID, mobjName);
}

char*
DagdaFwdrImpl::getBestSource(const char* dest, const char* dataID) {
  return mforwarder->getBestSource(dest, dataID, mobjName);
}

char*
DagdaFwdrImpl::getID() {
  return mforwarder->getID(mobjName);
}

void
DagdaFwdrImpl::checkpointState() {
  mforwarder->checkpointState(mobjName);
}

char*
DagdaFwdrImpl::getHostname() {
  return mforwarder->getHostname(mobjName);
}
