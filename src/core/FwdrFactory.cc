#include "FwdrFactory.hh"

#ifdef DIETFWDR
#include "DIETForwarder.hh"
#endif
#ifdef LOGFWDR
#include "LOGForwarder.hh"
#endif
#ifdef DAGDAFWDR
#include "DAGDAForwarder.hh"
#endif


FwdrFactory* FwdrFactory::mfac = NULL;

FwdrFactory::~FwdrFactory() {
  mfac = NULL;
}

FwdrFactory*
FwdrFactory::getInstance() {
  if (!mfac) {
    mfac = new FwdrFactory();
  }
  return mfac;
}

Forwarder*
FwdrFactory::getFwdr(fwd_t id) {
  switch (id) {
  case DIET :
#ifdef DIETFWDR
    return new DIETForwarder();
#endif
    break;
  case LOG :
#ifdef LOGFWDR
    return new LOGForwarder();
#endif
    break;
  case DAGDA :
#ifdef DAGDAFWDR
    return new DAGDAForwarder();
#endif
    break;

  }
}


FwdrFactory::FwdrFactory();
