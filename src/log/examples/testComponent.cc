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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <iostream>

#include "LogCentralComponent_impl.hh"
#include "ComponentList.hh"
#include "FilterManagerInterface.hh"
#include "TimeBuffer.hh"
#include "LocalTime.hh"
#include "LogOptions.hh"

//#include "LogComponentBase.hh"
#include "ORBMgr.hh"
//#include "TimeBuffer.hh"
//#include "LogCentralComponent_impl.hh"
#include <stdio.h>

using namespace std;

class MyMsgSender : public POA_ComponentConfigurator,
                    public PortableServer::RefCountServantBase{
private:
  LogCentralComponent_ptr myLCC;
  char* name;
  LastPings* lastPings;
  TimeBuffer* timeBuffer;
public:


  MyMsgSender(const char* name){
    this->name = CORBA::string_dup(name);
    myLCC = ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_ptr>("LogServiceC", "LCC");
    if (CORBA::is_nil(myLCC)){
      fprintf (stderr, "Failed to narrow the LCC ! \n");
    }

    try{
      ORBMgr::getMgr()->bind("LogServiceC", name, _this(), true);
      ORBMgr::getMgr()->fwdsBind("LogServiceC", name,
                                    ORBMgr::getMgr()->getIOR(_this()));
    }
    catch (...){
      fprintf (stderr, "Bind failed  in the LogService context\n");
    }

  }

  void
  sendMsg(const log_msg_buf_t& buffer){

    // for each message, correction of its time and the message is sent to the
    // TimeBuffer.
    log_time_t td;

    LastPing* lp = NULL;
    if (buffer.length() != 0) {
      const char* name = buffer[0].componentName;
      LastPings::ReadIterator* it = this->lastPings->getReadIterator();

      bool compExists = false;
      while (it->hasCurrent()) {
        lp = it->nextRef();
        if (strcmp(lp->name, name) == 0) {
          td = lp->timeDifference;
          compExists = true;
          break;
        }
      }
      delete it;

      if (compExists == false) {
        std::cout << "Discarded messageBuffer from unknown component "
                  << name << std::endl;
        return;
      }

      for (unsigned int i = 0; i < buffer.length(); i++) {
        log_msg_t msg = buffer[i];
        // Correct the time derivation
        msg.time.sec += td.sec;
        msg.time.msec += td.msec;
        while (msg.time.msec < 0) {
          msg.time.msec += 1000;
          msg.time.sec -= 1;
        }
        while (msg.time.msec >= 1000) {
          msg.time.msec -= 1000;
          msg.time.sec += 1;
        }
        // FIXME: manage overflows here
        this->timeBuffer->put(&msg);
      }
    }
  }



  int
  connect(const char* componentName,
          const char* componentHostname,
          const char* message,
          const char* compConfigurator,
          const log_time_t& componentTime,
          tag_list_t& initialConfig){
    CORBA::String_var corbaname = CORBA::string_dup(componentName);
    return myLCC->connectComponent(corbaname,
                                   componentHostname,
                                   message,
                                   compConfigurator,
                                   componentTime,
                                   initialConfig);
  }


  int
  disconnect(const char* componentName,
             const char* message){
    return myLCC->disconnectComponent(componentName, message);
  }

  void
  setTagFilter(const tag_list_t& tagList) {
    return;
  }
  void
  addTagFilter(const tag_list_t& tagList) {
  }
  void
  removeTagFilter(const tag_list_t& tagList) {
  }

};



int
main(int argc, char** argv)
{
  cout << "Test of liblogcomponentbase.a\n\n";
  bool success = false;
  cout << "Init \n" << endl;
  try {
    ORBMgr::init(argc, argv);
  } catch (...) {
    fprintf (stderr, "ORB initialization failed");
  }
  MyMsgSender* myLCB;
  std::string name("hermes");
  std::string hostname("ukalerk");

  myLCB = new MyMsgSender(name.c_str());
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  log_time_t lt;
  lt.sec = tv.tv_sec;
  lt.msec = tv.tv_usec / 1000;
  tag_list_t tag;

  myLCB->connect(name.c_str(),
                 hostname.c_str(),
                 "Message to all the people",
                 name.c_str(),
                 lt,
                 tag);

  log_msg_buf_t buffer;
  log_msg_t logmsg;
  std::string tag2("tag2");
  logmsg.componentName = CORBA::string_dup(name.c_str());
  logmsg.time = getLocalTime();
  logmsg.warning = false;
  logmsg.tag = CORBA::string_dup(tag2.c_str());
  logmsg.msg = CORBA::string_dup("Message sentttttttttt");
  CORBA::ULong l = buffer.length();
  buffer.length(l + 1);
  buffer[l] = logmsg;


  myLCB->sendMsg(buffer);

  myLCB->disconnect(name.c_str(), "Bye bye");


//  LogComponentBase* logComponentBase =
//    new LogComponentBase(&success, argc, argv, 0, "testComponent");
//  cout << "Inited \n" << endl;
//  if (!success) {
//    cerr << "Cannot innitialize the LogComponentBase !\n"
//      << "Are LogCentral or Omninames not running ?\n";
//    return 1;
//  }
//  short ret = 0;
//  ret = logComponentBase->connect("testCompo connected \\o/");
//  if (ret == LS_COMPONENT_CONNECT_BADNAME) {
//    cerr << "Component : bad name !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_ALREADYEXISTS) {
//    cerr << "Component : already exist !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR) {
//    cerr << "Component : bad component configurator !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_INTERNALERROR) {
//    cerr << "Component : cannot connect !\n";
//    return ret;
//  }
//
//  char* s;
//  s = logComponentBase->getName();
//  cout << "name:" << s << endl;
//  delete s;
//  s = logComponentBase->getHostname();
//  cout << "hostname:" << s << endl;
//  delete s;
//
//  ret = logComponentBase->connect("test connect 2");
//  if (ret == LS_COMPONENT_CONNECT_BADNAME) {
//    cerr << "Component : bad name !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_ALREADYEXISTS) {
//    cerr << "Component : already exists !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_BADCOMPONENTCONFIGURATOR) {
//    cerr << "Component : bad component configurator !\n";
//    return ret;
//  }
//  if (ret == LS_COMPONENT_CONNECT_INTERNALERROR) {
//    cerr << "Component : cannot connect !\n";
//    return ret;
//  }
//
//  logComponentBase->sendMsg("TEST", "test message");
//  sleep(1);  // make sure the message is sent
//
//  ret = logComponentBase->disconnect("test disconnection of component");
//  if (ret == LS_COMPONENT_DISCONNECT_NOTEXISTS) {
//    cerr << "Component : cannot disconnect, not exists !\n";
//    return ret;
//  }
//
//  //  logComponentBase->setName("testComponent_C++");
//  ret = logComponentBase->connect("test connect 3");
//  if (ret != LS_OK) {
//    return ret;
//  }
//
//  logComponentBase->sendMsg("ANY", "another test message");
//  sleep(1);  // make sure this message is sent
//
//  cout << "'IN' tag wanted ? " << logComponentBase->isLog("IN") << endl;
//  cout << "'TEST' tag wanted ? " << logComponentBase->isLog("TAG") << endl;
//  ret = logComponentBase->disconnect("test disconnect 2");
//  if (ret != LS_OK) {
//    return ret;
//  }
//
//  delete logComponentBase;
  cout << "All tests passed successfully !\n";
  return 0;
}

