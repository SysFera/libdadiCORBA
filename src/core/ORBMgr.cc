/**
 * \file ORBMgr.cc
 * \brief This file defines the ORBMgr class implementation
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 03/08/11
 */

#include <string>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <csignal>

#include <omniORB4/CORBA.h>
#include <omniORB4/omniURI.h>

#include "ORBMgr.hh"
#include "CorbaForwarder.hh"

using namespace std;

ORBMgr* ORBMgr::theMgr = NULL;

#ifndef __cygwin__
omni_mutex ORBMgr::waitLock;
#else
sem_t ORBMgr::waitLock;
#endif

/* Maximum number of retries for communication failures
 * TODO: should be a parameter in the configuration file
 */
static const unsigned int maxNbRetries = 3;

/* Handler for call resubmission when TRANSIENT exceptions occur */
CORBA::Boolean transientHandler (void* cookie,
                                 CORBA::ULong retries,
                                 const CORBA::TRANSIENT& ex) {
  /* We only retry if the number of retries is lower than maxNbRetries */
  if (retries < maxNbRetries) {
    return 1;           // retry immediately.
  }
  return 0;
}

/* Handler for call resubmission when COMM_FAILURE exceptions occur */
CORBA::Boolean commFailureHandler (void* cookie,
                                   CORBA::ULong retries,
                                   const CORBA::COMM_FAILURE& ex) {
  /* We only retry if the number of retries is lower than maxNbRetries */
  if (retries < maxNbRetries) {
    return 1;           // retry immediately.
  }
  return 0;
}


/* Manager initialization. */
void ORBMgr::init(CORBA::ORB_ptr ORB) {
  CORBA::Object_var object;
  PortableServer::POA_var initPOA;
  PortableServer::POAManager_var manager;
  CORBA::PolicyList policies;
  CORBA::Any policy;
  CosNaming::NamingContext_var rootContext, context;
  CosNaming::Name cosName;
  
  if (CORBA::is_nil(ORB)) {
    throw runtime_error("ORB not initialized");
  }
  object = ORB->resolve_initial_references("RootPOA");
  initPOA = PortableServer::POA::_narrow(object);
  manager = initPOA->the_POAManager();
  policies.length(1);
  policy <<= BiDirPolicy::BOTH;
  policies[0] = ORB->create_policy(BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE,
                                   policy);

  // If poa already exist, use the same
  try {
    POA = initPOA->create_POA("bidirService", manager, policies);
  } catch (PortableServer::POA::AdapterAlreadyExists &e) {
    POA = initPOA->find_POA("bidirService", false);    
  }
  
  manager->activate();

  /* Install handlers for automatically handle call resubmissions */
  omniORB::installTransientExceptionHandler(0, transientHandler);
  omniORB::installCommFailureExceptionHandler(0, commFailureHandler);
}

ORBMgr::ORBMgr(int argc, char* argv[]) {
  const char* opts[][2]= {{0,0}};
  mconn = map<string,Connector*>();
  std::cout << "In the constructor " << std::endl;  
  ORB = CORBA::ORB_init(argc, argv, "omniORB4", opts);
  std::cout << "In the constructor B4 init " << std::endl;  
  init(ORB);
  std::cout << "In the constructor, not down " << std::endl;  
  down = false;
}

ORBMgr::ORBMgr(CORBA::ORB_ptr ORB) {
  this->ORB = ORB;
  init(ORB);
  down = false;
}

ORBMgr::ORBMgr(CORBA::ORB_ptr ORB, PortableServer::POA_var POA) {
  this->ORB=ORB;
  this->POA=POA;
  down = false;
}

ORBMgr::~ORBMgr() {
  shutdown(true);
  ORB->destroy();
  theMgr = NULL;
}

void ORBMgr::bind(const string& ctxt, const string& name,
                     CORBA::Object_ptr object, const string& connectId,
		  const bool rebind) const {
  CORBA::Object_var obj;
  CosNaming::NamingContext_var rootContext, context;
  CosNaming::Name cosName;
	
  obj = ORB->resolve_initial_references("NameService");
  if (CORBA::is_nil(obj)) {
    throw runtime_error("Error resolving initial references");
  }  
  rootContext = CosNaming::NamingContext::_narrow(obj);
  
  if (CORBA::is_nil(rootContext)) {
    throw runtime_error("Error initializing root context");
  }

  cosName.length(1);
  cosName[0].id = ctxt.c_str();
  cosName[0].kind = "";
  try {
    context = rootContext->bind_new_context(cosName);
  } catch (CosNaming::NamingContext::AlreadyBound& err) {
    obj = rootContext->resolve(cosName);
    context = CosNaming::NamingContext::_narrow(obj);
    if (CORBA::is_nil(context)) {
      throw runtime_error(string("Error creating context ")+ctxt);
    }
  }
  cosName[0].id = name.c_str();
  cosName[0].kind = "";
  try {
    context->bind(cosName, object);
  } catch (CosNaming::NamingContext::AlreadyBound& err) {
    if (rebind) {
      context->rebind(cosName, object);
    } else{
      throw runtime_error("Already bound!");
    }
  }
}

void ORBMgr::bind(const string& ctxt, const string& name,
                     const string& IOR, const string& connectId, const bool rebind) const {
  CORBA::Object_ptr object = ORB->string_to_object(IOR.c_str());
	
  bind(ctxt, name, object, connectId, rebind);
}

void ORBMgr::rebind(const string& ctxt, const string& name,
		    CORBA::Object_ptr object, const string& connect) const {
  bind(ctxt, name, object, connect, true);
}

void ORBMgr::rebind(const string& ctxt, const string& name,
                       const string& IOR, const string& connect) const {
  CORBA::Object_ptr object = ORB->string_to_object(IOR.c_str());
  
  rebind(ctxt, name, object, connect);
}

void ORBMgr::unbind(const string& ctxt, const string& name) const {
  CORBA::Object_var obj;
  CosNaming::NamingContext_var rootContext, context;
  CosNaming::Name cosName;
  std::list<pair<string,string> >::iterator it;
  
  obj = ORB->resolve_initial_references("NameService");
  rootContext = CosNaming::NamingContext::_narrow(obj);
  
  cosName.length(1);
  cosName[0].id = ctxt.c_str();
  cosName[0].kind = "";
  
  obj = rootContext->resolve(cosName);
  context = CosNaming::NamingContext::_narrow(obj);
  if (CORBA::is_nil(context))
    throw runtime_error(string("Error retrieving context ")+ctxt);
  cosName[0].id = name.c_str();
  cosName[0].kind = "";
  try {
    removeObjectFromCache(ctxt, name);
    context->unbind(cosName);
  } catch (CosNaming::NamingContext::NotFound& err) {
    throw runtime_error("Object "+name+" not found in " + ctxt +" context");
  }
}

void ORBMgr::fwdsBind(const string& ctxt, const string& name,
		      const string& ior, const string& connectId,
		      const string& fwName) const {
  std::list<string> forwarders = ORBMgr::list(FWRDCTXT);
  std::list<string>::const_iterator it;
	
  for (it=forwarders.begin(); it!=forwarders.end(); ++it) {
    if (fwName==*it) continue;
    CorbaForwarder_var fwd = resolve<CorbaForwarder, CorbaForwarder_var>(FWRDCTXT, *it, connectId);
    string objName = ctxt+"/"+name;
    try {
      fwd->bind(objName.c_str(), ior.c_str(), connectId.c_str());
    } catch (const CORBA::TRANSIENT& err) {
      continue;
    } catch (BadNameException& err) {
    }
  }
}

void ORBMgr::fwdsUnbind(const string& ctxt, const string& name,
                           const string& fwName) const {
  std::list<string> forwarders = ORBMgr::list(FWRDCTXT);
  std::list<string>::const_iterator it;
	
  for (it=forwarders.begin(); it!=forwarders.end(); ++it) {
    if (fwName==*it) continue;
    CorbaForwarder_var fwd = resolve<CorbaForwarder, CorbaForwarder_var>(FWRDCTXT, *it);
    string objName = ctxt+"/"+name;
    try {
      fwd->unbind(objName.c_str());
    } catch (const CORBA::TRANSIENT& err) {
      continue;
    }
  }
}

CORBA::Object_ptr ORBMgr::resolveObject(const string& IOR) const {
  return ORB->string_to_object(IOR.c_str());
}

CORBA::Object_ptr ORBMgr::resolveObject(const string& context, const string& name,
					const string& connectId, const string& fwdName) const {
  fprintf (stderr, "Z1 \n");
  string ctxt = context;
  string ctxt2 = context;

  fprintf (stderr, "Z1 \n");
  Connector* c = getConnector(connectId);
  fprintf (stderr, "Z2 \n");
  if (!c) {
    fprintf (stderr, "Z3 \n");
    std::cerr << "failure" << std::cerr;
    throw std::exception();
  }
  fprintf (stderr, "Z4 \n");

  ctxt = c->getContext(context);

  CORBA::Object_ptr object = ORB->resolve_initial_references("NameService");
  CosNaming::NamingContext_var rootContext =
  CosNaming::NamingContext::_narrow(object);
  CosNaming::Name cosName;
  
  cosName.length(2);
  cosName[0].id   = ctxt.c_str();
  cosName[0].kind = "";
  cosName[1].id   = name.c_str();
  cosName[1].kind = "";
  
  try {
    object = rootContext->resolve(cosName);
  } catch (CosNaming::NamingContext::NotFound& err) {
    throw runtime_error("Error resolving "+ctxt+"/"+name);
  }


  return CORBA::Object::_duplicate(object);
}

std::list<string> ORBMgr::list(CosNaming::NamingContext_var& ctxt) const {
  std::list<string> result;
  CosNaming::BindingList_var ctxtList;
  CosNaming::BindingIterator_var it;
  CosNaming::Binding_var bv;

  ctxt->list(256, ctxtList, it);
  for (unsigned int i=0; i<ctxtList->length(); ++i)
    if (ctxtList[i].binding_type==CosNaming::nobject)
      for (unsigned int j=0; j<ctxtList[i].binding_name.length(); ++j) {
        result.push_back(string(ctxtList[i].binding_name[j].id));
      }
  if (CORBA::is_nil(it)) return result;
  while (it->next_one(bv)) {
    if (bv->binding_type==CosNaming::nobject)
      for (unsigned int j=0; j<bv->binding_name.length(); ++j) {
        result.push_back(string(bv->binding_name[j].id));
      }
  }
  return result;
}

std::list<string> ORBMgr::list(const std::string& ctxtName) const {
  std::list<string> result;
	
  CORBA::Object_ptr object = ORB->resolve_initial_references("NameService");
  CosNaming::NamingContext_var rootContext =
    CosNaming::NamingContext::_narrow(object);
  CosNaming::BindingList_var bindingList;
  CosNaming::BindingIterator_var it;
	
  rootContext->list(256, bindingList, it);

  for (unsigned int i=0; i<bindingList->length(); ++i) {
    if (bindingList[i].binding_type==CosNaming::ncontext)
      if (string(bindingList[i].binding_name[0].id)==ctxtName) {
        std::list<string> tmpRes;
        CORBA::Object_ptr ctxtObj = rootContext->resolve(bindingList[i].binding_name);
        CosNaming::NamingContext_var ctxt =
          CosNaming::NamingContext::_narrow(ctxtObj);
			
        tmpRes = list(ctxt);
        result.insert(result.end(), tmpRes.begin(), tmpRes.end());
      }
  }
  if (CORBA::is_nil(it)) return result;
  CosNaming::Binding_var bv;
  while (it->next_one(bv))
    if (bv->binding_type==CosNaming::ncontext)
      if (string(bv->binding_name[0].id)==ctxtName) {
        std::list<string> tmpRes;
        CORBA::Object_ptr ctxtObj = rootContext->resolve(bv->binding_name);
        CosNaming::NamingContext_var ctxt =
          CosNaming::NamingContext::_narrow(ctxtObj);
				
        tmpRes = list(ctxt);
        result.insert(result.end(), tmpRes.begin(), tmpRes.end());
      }
  return result;
}


string ORBMgr::getIOR(CORBA::Object_ptr object) const {
  return ORB->object_to_string(object);
}

string ORBMgr::getIOR(const string& ctxt, const string& name, std::string id) const {
  return ORB->object_to_string(resolveObject(ctxt, name, id));
}

void ORBMgr::activate(PortableServer::ServantBase* object) const {
  POA->activate_object(object);
  object->_remove_ref();
}

void ORBMgr::deactivate(PortableServer::ServantBase* object) const {
  PortableServer::ObjectId* id = POA->servant_to_id(object);
  POA->deactivate_object(*id);
}

void
ORBMgr::shutdown(bool waitForCompletion) {
  if (!down) {
    ORB->shutdown(waitForCompletion);
    down = true;
  }
}


void
ORBMgr::sigIntHandler(int sig) {
  /* Prevent from raising a new SIGINT handler */
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
#ifndef __cygwin__  
  ORBMgr::getMgr()->waitLock.unlock();
#else
  sem_post(&(ORBMgr::getMgr()->waitLock));
#endif
  signal(SIGINT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
}


void
ORBMgr::wait() const {
  /* FIXME: this is pretty ugly,
   * but currently I do not see how to do so properly
   */
  signal(SIGINT, ORBMgr::sigIntHandler);
  signal(SIGTERM, ORBMgr::sigIntHandler);
  try {
#ifdef __cygwin__
    sem_init(&waitLock,0,1);
    sem_wait(&waitLock);
    sem_wait(&waitLock);
    sem_post(&waitLock);
#else
    waitLock.lock();
    waitLock.lock();
    waitLock.unlock();
#endif
  } catch (...) {
  }
}


ORBMgr* ORBMgr::getMgr() {
  if (theMgr==NULL) {
    throw runtime_error("ORB manager not initialized!");
  } else {
    return theMgr;
  }
}

void ORBMgr::init(int argc, char* argv[]) {
  std::cout << "In the init " << std::endl;
  if (!theMgr) {
    std::cout << "In the constructor " << std::endl;
    //delete theMgr;
    theMgr = new ORBMgr(argc, argv);
  }
}


/* Translate the string passed as first argument in bytes and
 * record them into the buffer.
 */
void ORBMgr::hexStringToBuffer(const char* ptr, const size_t size,
                                  cdrMemoryStream& buffer) {
  stringstream ss;
  int value;
  CORBA::Octet c;
	
  for (unsigned int i=0; i<size; i+=2) {
    ss << ptr[i] << ptr[i+1];
    ss >> hex >> value;
    c = value;
    buffer.marshalOctet(c);
    ss.flush();
    ss.clear();
  }
}

/* Make an IOP::IOR object using a stringified IOR. */
void ORBMgr::makeIOR(const string& strIOR, IOP::IOR& ior) {
  /* An IOR must start with "IOR:" or "ior:" */
  if (strIOR.find("IOR:")!=0 && strIOR.find("ior:")!=0)
    throw runtime_error("Bad IOR: "+strIOR);

  const char* tab = strIOR.c_str();
  size_t size = (strIOR.length()-4);
  cdrMemoryStream buffer(size, false);
  CORBA::Boolean byteOrder;
	
  /* Convert the hex bytes string into buffer. */
  hexStringToBuffer(tab+4, size, buffer);
  buffer.rewindInputPtr();
  /* Get the endianness and init the buffer flag. */
  byteOrder = buffer.unmarshalBoolean();
  buffer.setByteSwapFlag(byteOrder);
	
  /* Get the object type id. */
  ior.type_id = IOP::IOR::unmarshaltype_id(buffer);
  /* Get the IOR profiles. */
  ior.profiles <<= buffer;
}

/* Convert IOP::IOR to a stringified IOR. */
void ORBMgr::makeString(const IOP::IOR& ior, string& strIOR) {
  strIOR = "IOR:";
  cdrMemoryStream buffer(0, true);
  stringstream ss;
  unsigned char* ptr;
		
  buffer.marshalBoolean(omni::myByteOrder);
  buffer.marshalRawString(ior.type_id);
  ior.profiles >>= buffer;
	
  buffer.rewindInputPtr();
  ptr = static_cast<unsigned char*>(buffer.bufPtr());
	
  for (unsigned long i=0; i < buffer.bufSize(); ++ptr, ++i)
    {
      string str;
      unsigned char c = *ptr;
      if (c<16) ss << '0';
      ss << (unsigned short) c;
      ss >> hex  >> str;
      ss.flush();
      ss.clear();
      strIOR+=str;
    }
}

/* Get the hostname of the first profile in this IOR. */
string ORBMgr::getHost(IOP::IOR& ior) {
  IIOP::ProfileBody body;
	
  if (ior.profiles.length()==0)
    return "nohost";
	
  IIOP::unmarshalProfile(ior.profiles[0], body);
  return string(body.address.host);
}

/* Get the hostname of the first profile in IOR passed
 * as a string.
 */
string ORBMgr::getHost(const string& strIOR) {
  IOP::IOR ior;
  makeIOR(strIOR, ior);
  return getHost(ior);
}

/* Get the port of the first profile in this IOR. */
unsigned int ORBMgr::getPort(IOP::IOR& ior) {
  IIOP::ProfileBody body;
	
  if (ior.profiles.length()==0)
    return 0;
	
  IIOP::unmarshalProfile(ior.profiles[0], body);
  return body.address.port;
}

/* Get the port of the first profile in IOR passed
 * as a string.
 */
unsigned int ORBMgr::getPort(const string& strIOR) {
  IOP::IOR ior;
  makeIOR(strIOR, ior);
  return getPort(ior);
}

/* Get the type id of the IOR. */
string ORBMgr::getTypeID(IOP::IOR& ior) {
  return string(ior.type_id);
}

/* Get the type id of the IOR passed as a string. */
std::string ORBMgr::getTypeID(const string& strIOR) {
  IOP::IOR ior;
  makeIOR(strIOR, ior);
  return getTypeID(ior);
}

std::string ORBMgr::convertIOR(IOP::IOR& ior, const std::string& host,
                                  const unsigned int port)
{
  IIOP::ProfileBody body;
  IOP::TaggedProfile profile;
  CORBA::ULong max_data;
  CORBA::ULong nb_data;
  CORBA::Octet* buffer;
  std::string result;

  if (ior.profiles.length()==0)
    throw runtime_error("Invalid IOR");
	
  for (unsigned int i=0; i<ior.profiles.length(); ++i)
    if (ior.profiles[i].tag==IOP::TAG_INTERNET_IOP) {
      IIOP::unmarshalProfile(ior.profiles[i], body);
			
      body.address.host = host.c_str();
      body.address.port = port;
			
      IIOP::encodeProfile(body, profile);
			
      max_data = profile.profile_data.maximum();
      nb_data = profile.profile_data.length();
      buffer = profile.profile_data.get_buffer(true);
      ior.profiles[i].profile_data.replace(max_data, nb_data, buffer, true);
    }
  makeString(ior, result);
  return result;
}

std::string ORBMgr::convertIOR(const std::string& strIOR, const std::string& host,
                                  const unsigned int port)
{
  IOP::IOR ior;
  makeIOR(strIOR, ior);
  return convertIOR(ior, host, port);
}

/* Object cache management functions. */
void ORBMgr::resetCache() const {
  cacheMutex.lock();
  cache.clear();
  cacheMutex.unlock();
}

void ORBMgr::removeObjectFromCache(const string& name) const {
  map<string, CORBA::Object_ptr>::iterator it;
  cacheMutex.lock();
  if ((it=cache.find(name))!=cache.end())	cache.erase(it);
  cacheMutex.unlock();
}

void ORBMgr::removeObjectFromCache(const string& ctxt,
                                      const string& name) const {
  removeObjectFromCache(ctxt+"/"+name);
}

void ORBMgr::cleanCache() const {
  map<string, CORBA::Object_ptr>::iterator it;
  std::list<string> toRemove;
  std::list<string>::const_iterator jt;

  cacheMutex.lock();
  for (it = cache.begin(); it != cache.end(); ++it) {
    try {
      if (it->second->_non_existent()) {
        toRemove.push_back(it->first);
      }
    } catch (const CORBA::OBJECT_NOT_EXIST& err) {
      toRemove.push_back(it->first);
    } catch (...) {
      toRemove.push_back(it->first);
    }
  }
  cacheMutex.unlock();
  for (jt = toRemove.begin(); jt != toRemove.end(); ++jt) {
    removeObjectFromCache(*jt);
  }
}


void
ORBMgr::addConnector(Connector* conn, string id) {
  // Do not add an existing connector twice
  if (mconn.find(id) == mconn.end()) {
    mconn.insert(pair<string, Connector*>(id, conn));
  }
}

Connector*
ORBMgr::getConnector(string id) const{
  if (mconn.find(id) != mconn.end()) {
    return mconn.find(id)->second;
  }
  return NULL;
}

