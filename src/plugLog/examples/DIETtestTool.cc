/****************************************************************************/
/* A test for the liblogtoolbase.a                                          */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.7  2011/05/13 08:17:52  bdepardo
 * Update ORB manager with changes made in DIET ORB manager.
 *
 * Revision 1.6  2011/04/22 11:44:22  bdepardo
 * Use a signal handler to handle background option.
 * This handler catches SIGINT and SIGTERM.
 *
 * Revision 1.5  2011/03/03 14:56:35  bdepardo
 * Fixed compilation warning
 *
 * Revision 1.4  2011/02/04 15:25:22  bdepardo
 * Removed unused variables.
 * Removed resource leak.
 *
 * Revision 1.3  2010/12/24 09:47:07  kcoulomb
 * Remove deprecated tools
 * tool gets generated name
 *
 * Revision 1.2  2010/12/03 12:40:26  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.1  2010/11/10 05:59:44  kcoulomb
 * Add a tool for DIET, the generated files can be used with VizDIET
 *
 * Revision 1.11  2010/11/10 04:32:51  kcoulomb
 * Fix the rebind problem
 *
 * Revision 1.10  2010/11/10 02:27:43  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.9  2008/11/04 08:21:05  bdepardo
 * Added #include <stdlib.h>.
 * Now compiles with gcc 4.3.2
 *
 * Revision 1.8  2005/07/01 12:55:01  rbolze
 * message recieve from LogCentral can be bigger now !
 *
 * Revision 1.7  2004/10/08 11:20:49  hdail
 * Corrected problem seen under 64-bit opteron: can not cast CORBA Long to
 * time_t directly.
 *
 * Revision 1.6  2004/07/08 20:32:15  rbolze
 * make som changes :
 *  - create a tool named DIETLogTool that is specific for DIET and VizDIET
 *  - create README to explain what is DIETLogTool
 *  - modify testTool to be an example independant from DIET
 *  - modify Makefile.am to take account of all this changes
 *
 * Revision 1.5  2004/05/25 12:02:39  hdail
 * Sleep isn't found on some systems with current inclusions.  Added
 * inclusion of unistd.h when HAVE_BACKGROUND is true.
 *
 * Revision 1.4  2004/05/12 12:39:04  hdail
 * Add support for running services in the background (at a loss of clean
 * exit with interactive 'Q').  Behavior can be controlled via configure.
 *
 * Revision 1.3  2004/03/16 17:57:22  rbolze
 * Now  you can tell to write log messages into a specific filname
 * the default filename is Dietlog.log
 * the command line to launch testTool should be >./testTool <filname>
 *
 * Revision 1.2  2004/03/02 08:41:21  rbolze
 * print message in VizDiet readable format, save in a file named logtest.log
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "LogToolBase.hh"
#include "stdio.h"
#include <iostream>
#include <string>
#include <fstream>
#include "time.h"
#include <stdlib.h>

#include "ORBMgr.hh"
#include "LogCentralTool_impl.hh"

#include "LogToolBase.hh"
#include "LogConnector.hh"


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
  LogConnector* mcon;

  filter_t filter;//ADD

  MyMsgRecv(const char* name, ORBMgr* mgr){
    mcon = new LogConnector(mgr);
    mcon->registrer("log");
    this->name = CORBA::string_dup(name);

    filter.filterName = CORBA::string_dup("allFilter");//ADD
    filter.tagList.length(1);			       //ADD
    filter.tagList[0] = CORBA::string_dup("*");	       //ADD
    filter.componentList.length(1);		       //ADD
    filter.componentList[0] = CORBA::string_dup("*");  //ADD
    filename = "DIETLogTool.log";
    //    CORBA::Object_ptr myLCTptr;
    //
    //    myLCT = LogORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_ptr>("LogServiceT", "LCT");
    ////    try {
    ////      fprintf (stderr, "Recherche %s/%s \n", LOGCOMPCTXT, "LCT"),
    ////      myLCTptr = LogORBMgr::getMgr()->resolveObject(LOGCOMPCTXT, "LCT");
    ////    } catch(CORBA::SystemException &e) {
    ////      fprintf (stderr, "Could not resolve 'LogService./LogComponent (LCT).' from the NS");
    ////    }
    ////    if (CORBA::is_nil(myLCTptr)) {
    ////      fprintf (stderr, "Could not resolve 'LogService./LogComponent (LCT).' from the NS");
    ////    }
    ////    try {
    ////      myLCT = LogCentralTool::_narrow(myLCTptr);
    ////    } catch(CORBA::SystemException &e) {
    ////      fprintf (stderr, "Could not narrow the LogCentralComponent");
    ////    }
    //    if (CORBA::is_nil(myLCT)){
    //      fprintf (stderr, "You failed ! Nil narrow ! \n");
    //    }
    //    try{
    //      LogORBMgr::getMgr()->bind("LogServiceT", name, _this(), false);
    //      LogORBMgr::getMgr()->fwdsBind("LogServiceT", name,
    //				 LogORBMgr::getMgr()->getIOR(_this()));
    //    }
    //    catch (...){
    //      fprintf (stderr, "Bind FAILED  in the LogService context\n");
    //    }

  }

  int disconnect (){
    return myLCT->disconnectTool (name);
  }

  void run (){
    
    CORBA::Object_ptr myLCTptr;

    // Connexion to the LCT to get messages
    myLCT = mcon->resolve<LogCentralTool, LogCentralTool_ptr>("LogServiceT", "LCT", "log");
    if (CORBA::is_nil(myLCT)){
      fprintf (stderr, "Failed to narrow the LCT ! \n");
    }

    try{
      mcon->bind("LogServiceT", name, _this(), "log", true);
      mcon->fwdsBind("LogServiceT", name,
		     mcon->getIOR(_this()), "log");
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
  LogConnector* getConnector() {
    return mcon;
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
  myLTB = new MyMsgRecv (name, ORBMgr::getMgr());
  if (argc>=2)
    myLTB->setFilename(argv[1]);
  if (argc>=3)
    myLTB->setFilter(argv[2]);
  myLTB->getConnector()->activate((MyMsgRecv*)myLTB);
  myLTB->run ();


  /* The code to support interactive quit via user input of q or Q is
   * incompatible with backgrounding the LogCentral process.  Use
   * configure option --enable-background to select which you prefer.
   */
#ifdef HAVE_BACKGROUND
  try {
    mcon->wait();
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
