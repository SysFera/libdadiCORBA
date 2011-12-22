/**
 * @file testComponent.cc
 *
 * @brief An example of component
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

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
#include "utils/LocalTime.hh"
#include "LogOptions.hh"

#include "ORBMgr.hh"
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
    this->lastPings = new LastPings();
    this->name = CORBA::string_dup(name);
    myLCC = ORBMgr::getMgr()->resolve<LogCentralComponent, LogCentralComponent_ptr>("LogServiceC", "LCC");
    if (CORBA::is_nil(myLCC)){
      fprintf (stderr, "Failed to narrow the LCC ! \n");
    }

    try{
      ORBMgr::getMgr()->bind("LogServiceC", this->name, _this(), true);
      ORBMgr::getMgr()->fwdsBind("LogServiceC", this->name,
                                    ORBMgr::getMgr()->getIOR(_this()));
    }
    catch (...){
      fprintf (stderr, "Bind failed  in the LogService context\n");
    }

  }

  void
  sendMsg(const log_msg_buf_t& buffer){
    myLCC->sendBuffer(buffer);
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
  void
  test() {
  }


};



int
main(int argc, char** argv)
{
  bool success = false;
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
  logmsg.msg = CORBA::string_dup("Message sent");
  CORBA::ULong l = buffer.length();
  buffer.length(l + 1);
  buffer[l] = logmsg;

  myLCB->sendMsg(buffer);

  myLCB->disconnect(name.c_str(), "Bye bye");
  cout << "All tests passed successfully !\n";
  return 0;
}

