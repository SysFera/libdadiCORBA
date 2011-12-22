/**
 * @file DIETtestTool.cc
 *
 * @brief An example of tool
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

#include "stdio.h"
#include <iostream>
#include <string>
#include <fstream>
#include "time.h"
#include <stdlib.h>

#include "ORBMgr.hh"
#include "LogCentralTool_impl.hh"


#ifdef HAVE_BACKGROUND
#include <unistd.h>
#endif

using namespace std;

class MyMsgRecv : public POA_ToolMsgReceiver,
		  public PortableServer::RefCountServantBase{
public :
  LogCentralTool_ptr myLCT;
  char* name;
  string filename;

  filter_t filter;

  MyMsgRecv(const char* name){

    this->name = CORBA::string_dup(name);

    filter.filterName = CORBA::string_dup("allFilter");
    filter.tagList.length(1);
    filter.tagList[0] = CORBA::string_dup("*");
    filter.componentList.length(1);
    filter.componentList[0] = CORBA::string_dup("*");
    filename = "DIETLogTool.log";
  }

  int disconnect (){
    return myLCT->disconnectTool (name);
  }

  void run (){

    CORBA::Object_ptr myLCTptr;

    // Connexion to the LCT to get messages
    myLCT = ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_ptr>("LogServiceT", "LCT");
    if (CORBA::is_nil(myLCT)){
      fprintf (stderr, "Failed to narrow the LCT ! \n");
    }

    try{
      ORBMgr::getMgr()->bind("LogServiceT", name, _this(), true);
      ORBMgr::getMgr()->fwdsBind("LogServiceT", name,
                                    ORBMgr::getMgr()->getIOR(_this()));
    }
    catch (...){
      fprintf (stderr, "Bind failed  in the LogService context\n");
    }

    try{
      myLCT->connectTool (name, name);
      myLCT->addFilter(name, filter);
    }
    catch (...){
      fprintf (stderr, "Caught an unknown exception\n");
    }
  }

  void
  sendMsg(const log_msg_buf_t& msg) {
    struct tm *m;
    for (CORBA::ULong i=0; i<msg.length(); i++){
      long int tempSec = msg[i].time.sec;
      m = gmtime(&tempSec);
      char s[256];
      string log;
      // componentName
      log.append(msg[i].componentName);
      log.append(":");
      //Date in the right format
      sprintf(s,"%02d:%02d:%02d:%02d:%02d:%02d:%d:",
	      m->tm_mday, m->tm_mon + 1, m->tm_year + 1900,m->tm_hour,
	      m->tm_min, m->tm_sec,(int)msg[i].time.msec);
      log.append(s);
      // Channel : LOGCENTRAL
      log.append("LOGCENTRAL:");
      // priority : 5 not important
      log.append("5:");
      //tagtype
      log.append(msg[i].tag);
      log.append(":");
      //msg
      log.append(msg[i].msg);
      ofstream dest(filename.c_str(),ios::app);
      dest << log << endl;
      dest.close();
      cout << log << endl;
    }
  }
  void setFilter(char* description_file_name){
    if (description_file_name == NULL){
      return;
    }
    char** tags = NULL;
    FILE* filter_file;
    printf("filter_file_name = %s\n",description_file_name);
    filter_file = fopen(description_file_name, "r" );
    int nb=0;
    if (filter_file != NULL){
      char line[BUFSIZ];
      while(fgets(line,BUFSIZ,filter_file)!=NULL){
        if(line[0]!='#' & line[0]!='\n' & line[0]!=' '){
          nb++;
          char tag[BUFSIZ];
          sscanf(line,"%s",tag);
          tags=(char**)realloc(tags,(nb)*sizeof(char*));
          tags[nb-1]=(char*)malloc(strlen(line)*sizeof(char));
          sprintf(tags[nb-1],"%s",tag);
        }

      }
      fclose(filter_file);
    }
    filter.filterName = CORBA::string_dup("myfilter");
    filter.componentList.length(1);
    filter.componentList[0] = CORBA::string_dup("*");
    filter.tagList.length(nb);
    for (int i =0;i<nb;i++){
      printf("add_tag : %s\n",tags[i]);
      filter.tagList[i] = CORBA::string_dup(tags[i]);
    }
  }
  void setFilename(char* name){
    if (name)
      filename = string(name);
  }
};


int
main(int argc, char** argv)
{
  try {
    ORBMgr::init(argc, argv);
  } catch (...) {
    fprintf (stderr, "ORB initialization failed");
  }

  MyMsgRecv* myLTB;
  char name[200];
  char host[50];
  srand (time (NULL));
  if (gethostname(host, 50)){
    fprintf (stderr, "Failed to build the tool name. Leaving \n");
  }
  sprintf (name, "dietTool_%s_%d", host, rand());
  myLTB = new MyMsgRecv (name);
  if (argc>=2)
    myLTB->setFilename(argv[1]);
  if (argc>=3)
    myLTB->setFilter(argv[2]);
  ORBMgr::getMgr()->activate((MyMsgRecv*)myLTB);
  myLTB->run ();


  /* The code to support interactive quit via user input of q or Q is
   * incompatible with backgrounding the LogCentral process.  Use
   * configure option --enable-background to select which you prefer.
   */
#ifdef HAVE_BACKGROUND
  try {
    ORBMgr::getMgr()->wait();
  } catch (...) {
    cerr << "Error while exiting the ORBMgr::wait() function" << endl;
  }
#else                 // HAVE_BACKGROUND
  printf("Press CRTL-D to exit\n");
  char c;
  while(cin.get(c)) {
    if (c==10) printf("Press CRTL-D to exit\n");
  }

  printf("Disconnecting from monitor\n");
  if (myLTB->disconnect() != 0) {
    printf("failed\n");
    exit(1);
  }

  printf("Done\n");
  delete myLTB;
  return 0;
#endif
}
