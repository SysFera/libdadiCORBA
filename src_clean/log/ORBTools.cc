/**
 * @file ORBTools.cc
 *
 * @brief A class for putting some ORB functions together - IMPLEMENTATION
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "ORBTools.hh"
#include "omnithread.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
using namespace std;
#include <fcntl.h>
#include <termios.h>

CORBA::ORB_var ORBTools::orb = CORBA::ORB::_nil();
PortableServer::POA_var ORBTools::poa = PortableServer::POA::_nil();

bool
ORBTools::init(int argc, char** argv)
{
  ORBTools::orb = CORBA::ORB_init(argc, argv);
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  CORBA::Object_var obj = ORBTools::orb->resolve_initial_references("RootPOA");
  if (CORBA::is_nil(obj)) {
    return false;
  }
  ORBTools::poa = PortableServer::POA::_narrow(obj);
  if (CORBA::is_nil(ORBTools::poa)) {
    return false;
  }
  return true;
}

bool
ORBTools::init(int argc, char** argv, unsigned int tracelevel, unsigned int port)
{
  int myargc = argc;
  char** myargv = NULL;
  if (port == 0) {
    myargc += 2;
    myargv = new char*[myargc];
    for (int i = 0 ; i < myargc - 2 ; i++) {
      myargv[i] = strdup(argv[i]);
    }
    myargv[myargc - 2] = strdup("-ORBtraceLevel");
    char* s = new char[6];
    sprintf(s, "%u", tracelevel);
    myargv[myargc - 1] = s;
  } else {
    myargc += 4;
    myargv = new char*[myargc];
    for (int i = 0 ; i < myargc - 4 ; i++) {
      myargv[i] = strdup(argv[i]);
    }
    myargv[myargc - 4] = strdup("-ORBtraceLevel");
    char* s1 = new char[6];
    sprintf(s1, "%u", tracelevel);
    myargv[myargc - 3] = s1;
    myargv[myargc - 2] = strdup("-ORBendPoint");
    char* s2 = new char[6];
    sprintf(s2, "%u", port);
    myargv[myargc - 1] = s2;
  }
  ORBTools::orb = CORBA::ORB_init(myargc, myargv);
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  CORBA::Object_var obj = ORBTools::orb->resolve_initial_references("RootPOA");
  if (CORBA::is_nil(obj)) {
    return false;
  }
  ORBTools::poa = PortableServer::POA::_narrow(obj);
  if (CORBA::is_nil(ORBTools::poa)) {
    return false;
  }
  return true;
}

bool
ORBTools::init()
{
  int myargc = 0;
  char** myargv = new char*[1];
  myargv[0] = strdup("");
  ORBTools::orb = CORBA::ORB_init(myargc, myargv);
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  CORBA::Object_var obj = ORBTools::orb->resolve_initial_references("RootPOA");
  if (CORBA::is_nil(obj)) {
    return false;
  }
  ORBTools::poa = PortableServer::POA::_narrow(obj);
  if (CORBA::is_nil(ORBTools::poa)) {
    return false;
  }
  return true;
}

bool
ORBTools::registerServant(const char* contextName, const char* contextKind,
                          const char* name, const char* kind,
                          CORBA::Object* objref)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  if (objref == NULL) {
    return false;
  }
  CosNaming::NamingContext_var rootContext;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "The NamingService could not be resolved (InvalidName)\n";
    return false;
  } catch (CORBA::SystemException& ex) {
    cerr << "The NamingService could not be resolved (SystemException)\n";
    return false;
  }
  try {
    CosNaming::Name context;
    context.length(1);
    context[0].id   = new char[strlen(contextName) + 1];
    strcpy(context[0].id,contextName);
    context[0].kind = new char[strlen(contextKind) + 1];
    strcpy(context[0].kind,contextKind);
    CosNaming::NamingContext_var testContext;
    try {
      testContext = rootContext->bind_new_context(context);
    } catch (CosNaming::NamingContext::AlreadyBound& ex) {
      CORBA::Object_var obj;
      obj = rootContext->resolve(context);
      testContext = CosNaming::NamingContext::_narrow(obj);
      if (CORBA::is_nil(testContext)) {
        cerr << "Failed to narrow naming context.\n";
        return false;
      }
    }
    CosNaming::Name object;
    object.length(1);
    object[0].id   = new char[strlen(name) + 1];
    strcpy(object[0].id,name);
    object[0].kind = new char[strlen(kind) + 1];
    strcpy(object[0].kind,kind);

    try {
      testContext->bind(object, objref);
    } catch (CosNaming::NamingContext::AlreadyBound& ex) {
      testContext->rebind(object, objref);
    }
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "naming service.\n";
    return false;
  } catch (CORBA::SystemException& ex) {
    cerr << "Caught a CORBA::SystemException while using the naming service.\n";
    return false;
  }
  return true;
}

bool
ORBTools::registerServant(const char* name, const char* kind,
                          CORBA::Object* objref)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  if (objref == NULL) {
    return false;
  }
  CosNaming::NamingContext_var rootContext;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "The NamingService could not be resolved (InvalidName)\n";
    return false;
  } catch (CORBA::SystemException& ex) {
    cerr << "The NamingService could not be resolved (SystemException)\n";
    return false;
  }
  try {
    CosNaming::Name objectName;
    objectName.length(1);
    objectName[0].id   = strdup(name);
    objectName[0].kind = strdup(kind);
    try {
      rootContext->bind(objectName, objref);
    } catch (CosNaming::NamingContext::AlreadyBound& ex) {
      rootContext->rebind(objectName, objref);
    }
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "Caught system exception COMM_FAILURE -- unable to contact the "
         << "naming service.\n";
    return false;
  } catch (CORBA::SystemException& ex) {
    cerr << "Caught a CORBA::SystemException while using the naming service.\n";
    return false;
  }
  return true;
}

bool
ORBTools::activateServant(PortableServer::ServantBase* object)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  ORBTools::poa->activate_object(object);
  object->_remove_ref();
  return true;
}

bool
ORBTools::activatePOA()
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  PortableServer::POAManager_var pman = ORBTools::poa->the_POAManager();
  pman->activate();
  return true;
}

bool
ORBTools::unregisterServant(const char* contextName, const char* contextKind,
                            const char* name, const char* kind)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  CosNaming::NamingContext_var rootContext, context;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "Service required is invalid [does not exist].\n";
    return false;
  }
  CosNaming::Name cosName;
  cosName.length(1);
  cosName[0].id = strdup(contextName);
  cosName[0].kind = strdup(contextKind);
  try {
    try {
      CORBA::Object_var tmpobj;
      tmpobj = rootContext->resolve(cosName);
      context = CosNaming::NamingContext::_narrow(tmpobj);
      if (CORBA::is_nil(context)) {
        cerr << "Failed to narrow the naming context.\n";
        return false;
      }
    } catch(CosNaming::NamingContext::NotFound& ex) {
      cerr << "Cannot find object\n";
      return false;
    } catch (...) {
      cerr << "System exception caught while using the naming service\n";
      return false;
    }
    cosName[0].id   = strdup(name);
    cosName[0].kind = strdup(kind);
    context->unbind(cosName);
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "System exception caught (COMM_FAILURE), unable to connect to "
      << "the CORBA name server\n";
    return false;
  } catch (omniORB::fatalException& ex) {
    throw;
  } catch (...) {
    cerr << "System exception caught while using the naming service\n";
    return false;
  }
  return true;
}

bool
ORBTools::unregisterServant(const char* name, const char* kind)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  CosNaming::NamingContext_var rootContext, context;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "Service required is invalid [does not exist].\n";
    return false;
  }
  CosNaming::Name cosName;
  cosName.length(1);
  cosName[0].id = strdup(name);
  cosName[0].kind = strdup(kind);
  try {
    rootContext->unbind(cosName);
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "System exception caught (COMM_FAILURE), unable to connect to "
      << "the CORBA name server\n";
    return false;
  } catch (omniORB::fatalException& ex) {
    throw;
  } catch (...) {
    cerr << "System exception caught while using the naming service\n";
    return false;
  }
  return true;
}

bool
ORBTools::findServant(const char* contextName, const char* contextKind,
                      const char* name, const char* kind,
                      CORBA::Object*& objref)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  objref = NULL;
  CosNaming::NamingContext_var rootContext;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "Service required is invalid [does not exist].\n";
    return false;
  }
  CosNaming::Name cosName;
  cosName.length(2);
  cosName[0].id   = contextName;
  cosName[0].kind = contextKind;
  cosName[1].id   = name;
  cosName[1].kind = kind;
  try {
    objref = rootContext->resolve(cosName);
  } catch(CosNaming::NamingContext::NotFound& ex) {
    cerr << "Context for " << name << " not found\n";
    return false;
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "System exception caught (COMM_FAILURE): unable to connect to "
      << "the CORBA name server.\n";
    return false;
  } catch(omniORB::fatalException& ex) {
    throw;
  } catch (...) {
    cerr << "System exception caught while using the naming service\n";
    return false;
  }
  return true;
}

bool
ORBTools::findServant(const char* name, const char* kind,
                      CORBA::Object*& objref)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  objref = NULL;
  CosNaming::NamingContext_var rootContext;
  try {
    CORBA::Object_var obj;
    obj = ORBTools::orb->resolve_initial_references("NameService");
    try {
      rootContext = CosNaming::NamingContext::_narrow(obj);
    } catch(CORBA::SystemException& ex) {
      cerr << "Failed to narrow the root naming context.\n";
      return false;
    }
  } catch (CORBA::ORB::InvalidName& ex) {
    cerr << "Service required is invalid [does not exist].\n";
    return false;
  }
  CosNaming::Name cosName;
  cosName.length(1);
  cosName[0].id   = strdup(name);
  cosName[0].kind = strdup(kind);
  try {
    objref = rootContext->resolve(cosName);
  } catch(CosNaming::NamingContext::NotFound& ex) {
    cerr << "Context for " << name << " not found\n";
    return false;
  } catch (CORBA::COMM_FAILURE& ex) {
    cerr << "System exception caught (COMM_FAILURE): unable to connect to "
      << "the CORBA name server.\n";
    return false;
  } catch(omniORB::fatalException& ex) {
    throw;
  } catch (...) {
    cerr << "System exception caught while using the naming service\n";
    return false;
  }
  return true;
}



static void make_read_block()
{
  long file_flags;
  fd_set fs;
  FD_SET(STDIN_FILENO, &fs);
  file_flags = 0;
  fcntl(STDIN_FILENO, F_SETFL, file_flags);
}

static void make_read_nonblock()
{
  long file_flags;
  fd_set fs;
  FD_ZERO(&fs);
  FD_SET(STDIN_FILENO, &fs);
  file_flags = fcntl(STDIN_FILENO, F_GETFL);
  if (file_flags == -1) file_flags = 0;
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK|file_flags);
}


bool
ORBTools::listen(char stopLowercase, char stopUppercase)
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  bool again = true;
  char c = '\0';
  make_read_nonblock();
  while (again) {
    omni_thread::sleep(0, 500000000); // sleep 500ms
    c = fgetc(stdin);
    again = (c != stopLowercase && c != stopUppercase);
  }
  make_read_block();
  return true;
}

bool
ORBTools::kill()
{
  if (CORBA::is_nil(ORBTools::orb)) {
    return false;
  }
  ORBTools::orb->shutdown(true);
  ORBTools::orb->destroy();
  return true;
}


