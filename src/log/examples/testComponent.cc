/****************************************************************************/
/* A sample component based on libLogComponentBase.a                        */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: testComponent.cc,v 1.3 2011/02/04 15:52:22 bdepardo Exp $
 * $Log: testComponent.cc,v $
 * Revision 1.3  2011/02/04 15:52:22  bdepardo
 * System headers before our own headers
 *
 * Revision 1.2  2010/12/03 12:40:26  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include <unistd.h>
#include <iostream>

#include "LogComponentBase.hh"
#include "LogORBMgr.hh"

using namespace std;

int
main(int argc, char** argv)
{
  cout << "Test of liblogcomponentbase.a\n\n";
  bool success = false;
  cout << "Init \n" << endl;
  try {
    LogORBMgr::init(argc, argv, false);
	} catch (...) {
    fprintf (stderr, "ORB initialization failed");
  }
  LogComponentBase* logComponentBase =
    new LogComponentBase(&success, argc, argv, 0, "testComponent");
  cout << "Inited \n" << endl;
  if (!success) {
    cerr << "Cannot innitialize the LogComponentBase !\n"
      << "Are LogCentral or Omninames not running ?\n";
    return 1;
  }
  short ret = 0;
  ret = logComponentBase->connect("testCompo connected \\o/");
  if (ret == LS_COMPONENT_CONNECT_BADNAME) {
    cerr << "Component : bad name !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_ALREADYEXISTS) {
    cerr << "Component : already exist !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR) {
    cerr << "Component : bad component configurator !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_INTERNALERROR) {
    cerr << "Component : cannot connect !\n";
    return ret;
  }

  char* s;
  s = logComponentBase->getName();
  cout << "name:" << s << endl;
  delete s;
  s = logComponentBase->getHostname();
  cout << "hostname:" << s << endl;
  delete s;

  ret = logComponentBase->connect("test connect 2");
  if (ret == LS_COMPONENT_CONNECT_BADNAME) {
    cerr << "Component : bad name !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_ALREADYEXISTS) {
    cerr << "Component : already exists !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR) {
    cerr << "Component : bad component configurator !\n";
    return ret;
  }
  if (ret == LS_COMPONENT_CONNECT_INTERNALERROR) {
    cerr << "Component : cannot connect !\n";
    return ret;
  }

  logComponentBase->sendMsg("TEST", "test message");
  sleep(1);  // make sure the message is sent

  ret = logComponentBase->disconnect("test disconnection of component");
  if (ret == LS_COMPONENT_DISCONNECT_NOTEXISTS) {
    cerr << "Component : cannot disconnect, not exists !\n";
    return ret;
  }

  //  logComponentBase->setName("testComponent_C++");
  ret = logComponentBase->connect("test connect 3");
  if (ret != LS_OK) {
    return ret;
  }

  logComponentBase->sendMsg("ANY", "another test message");
  sleep(1);  // make sure this message is sent

  cout << "'IN' tag wanted ? " << logComponentBase->isLog("IN") << endl;
  cout << "'TEST' tag wanted ? " << logComponentBase->isLog("TAG") << endl;
  ret = logComponentBase->disconnect("test disconnect 2");
  if (ret != LS_OK) {
    return ret;
  }

  delete logComponentBase;
  cout << "All tests passed successfully !\n";
  return ret;
}

