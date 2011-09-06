/****************************************************************************/
/* LogCentral                                                               */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.9  2011/05/13 08:17:52  bdepardo
 * Update ORB manager with changes made in DIET ORB manager.
 *
 * Revision 1.8  2011/04/22 11:44:22  bdepardo
 * Use a signal handler to handle background option.
 * This handler catches SIGINT and SIGTERM.
 *
 * Revision 1.7  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/LogOptions due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.6  2010/12/03 12:40:27  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.5  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.4  2008/07/17 01:03:11  rbolze
 * make some change to avoid gcc warning
 *
 * Revision 1.3  2004/05/12 12:39:04  hdail
 * Add support for running services in the background (at a loss of clean
 * exit with interactive 'Q').  Behavior can be controlled via configure.
 *
 * Revision 1.2  2004/01/13 15:11:28  ghoesch
 * added stdlib.h to compile with omniORB3
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

// idl
#include "LogTypes.hh"
#include "LogTool.hh"
#include "LogComponent.hh"

// monitor components
#include "LogCentralTool_impl.hh"
#include "LogCentralComponent_impl.hh"
#include "ToolList.hh"
#include "ComponentList.hh"
#include "SimpleFilterManager.hh"
#include "FilterManagerInterface.hh"
#include "StateManager.hh"
#include "Options.hh"
#include "ReadConfig.hh"
#include "LocalTime.hh"
#include "TimeBuffer.hh"

// threads
#include "SendThread.hh"
#include "CoreThread.hh"

// helpers
#include "ORBTools.hh"
#include "ORBMgr.hh"
#include "LogConnector.hh"

int main(int argc, char** argv) 
{
  printf("\n");
  printf("LogCentral system\n");
  printf("\n");

  /**
   * Acquire configFileName
   * - check the environment and the arguments to find the name
   *   of the configfile for ReadConfig
   */
  char* configFileNameDefault = strdup("config.cfg");
  char* configFileName;
  configFileName = NULL;
  for (int i=0; i < argc; i++) {
    if (strcmp(argv[i], "-config") == 0) {
      // user tries to specify -config
      if ((i+1) < argc) {
        configFileName = argv[i+1];
      } else {
        printf("Cannot parse parameter of -config. Please use -help\n");
      }
    }
    if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0) {
      // any idea how to write this better ?
      printf(" -config FILENAME    Reads the config file specified in FILENAME. ");
      printf("Overrides\n                     the filename specified in ");
      printf("$LOGCENTRAL_CONFIG\n");
      printf(" -ORBparameter       Sets parameters of omniORB as specified ");
      printf("in the manual\n");
      printf(" -help or --help     Display this message and exit\n");
      printf("\n");
      exit(0);
    }
  }
  if (configFileName == NULL) {
    configFileName = getenv("LOGCENTRAL_CONFIG");
  }
  if (configFileName == NULL) {
    configFileName = configFileNameDefault;
  } else {
    // just print this, if we use a non-default configFile
    printf("Using config file '%s'\n", configFileName);
  }
  // Note: configFileName must not be deleted

  /**
   * Systemcomponents
   */
  ToolList* toolList;
  ComponentList* componentList;
  ReadConfig* readConfig;
  tag_list_t* stateTags;
  tag_list_t* allTags;

  StateManager* stateManager;
  SimpleFilterManager* simpleFilterManager;
  TimeBuffer* timeBuffer;

  LogCentralTool_impl* myLCT;
  LogCentralComponent_impl* myLCC;

  SendThread* sendThread;
  CoreThread* coreThread;

  // build system
  bool success=true;

  toolList = new ToolList;
  componentList = new ComponentList;

  readConfig = new ReadConfig(configFileName, &success);
  if (!success) {
    printf("Could not read the config file\n");
    exit(1);
  }
  if (readConfig->parse() != 0) {
    printf("Could not parse the config file\n");
    exit(1);
  }
  stateTags = readConfig->getStateTags();
  allTags = readConfig->getAllTags();
  // and we need those two for the ORB-initialization
  unsigned int tracelevel = readConfig->getTracelevel();
  unsigned int port = readConfig->getPort();

  stateManager = new StateManager(readConfig, &success);
  if (!success) {
    printf("Could not initialize the StateManager.\n");
    printf("Please check the config file to correct this problem.\n");
    exit(1);
  }
  simpleFilterManager = new SimpleFilterManager(toolList, componentList, stateTags);
  timeBuffer = new TimeBuffer();

  sendThread = new SendThread(toolList);
  coreThread = new CoreThread(timeBuffer, stateManager,
                              simpleFilterManager, toolList);

  myLCT = new LogCentralTool_impl(toolList, componentList,
                                  simpleFilterManager, stateManager, allTags);
  myLCC = new LogCentralComponent_impl(componentList, simpleFilterManager, timeBuffer);

  delete allTags;
  delete stateTags;
  delete readConfig;
  
  // ADD *******
  std::cout << "Before init orb mgr " << endl;
  ORBMgr::init(argc, argv);
  std::cout << "DONE init orb mgr " << endl;

  /**
   * Start ORB and initialize Servants
   * - check if port/tracelevel are set and initialize ORB correctly
   */
  if (!ORBTools::init(argc, argv, tracelevel, port)) {
    printf("Could not initialize ORB\n");
    exit(1);
  }

  if (!ORBTools::activateServant(myLCT)) {
    printf("Could not activate servants\n");
    exit(1);
  }
    CORBA::Object* myLCTref = myLCT->_this();
    if (!ORBTools::registerServant("LogServiceT", "", "LCT", "", myLCT->_this())) {
      printf("Could not register the servants at the NamingService\n");
      exit(1);
    }

//  // Inscription in the naming service
//  ORBMgr::getMgr()->bind("LogServiceT", "LCT",
//			 ORBMgr::getMgr()->getIOR(myLCT->_this()), true);
//
    LogConnector* conn = new LogConnector(ORBMgr::getMgr());
    conn->registrer("log");
    // Inscription in the others fwdr
    conn->fwdsBind("LogServiceT", "LCT",
		   ORBMgr::getMgr()->getIOR(myLCT->_this()), "log");
  
  if (!ORBTools::activateServant(myLCC)) {
    printf("Could not activate servants\n");
    exit(1);
  }
  CORBA::Object* myLCCref = myLCC->_this();
  if (!ORBTools::registerServant("LogServiceC", "", "LCC", "", myLCCref)) {
    printf("Could not register the servants at the NamingService\n");
    exit(1);
  }
//  // Inscription in the nameing service
//  ORBMgr::getMgr()->bind("LogServiceC", "LCC",
//			 ORBMgr::getMgr()->getIOR(myLCC->_this()), true);
  // Inscription in the others fwdr
  conn->fwdsBind("LogServiceC", "LCC",
		 ORBMgr::getMgr()->getIOR(myLCC->_this()), "log");

  if (!ORBTools::activatePOA()) {
    printf("Could not activate the POA\n");
    exit(1);
  }

  coreThread->startThread();
  sendThread->startThread();


  /**
   * Run
   */
  printf("LogCentral is running\n");

  /* The code to support interactive quit via user input of q or Q is
   * incompatible with backgrounding the LogCentral process.  Use
   * configure option --enable-background to select which you prefer.
   */
#ifdef HAVE_BACKGROUND
  printf("Got background option.\n");
  try {
    ORBMgr::getMgr()->wait();
  } catch (...) {
    cerr << "Error while exiting the ORBMgr::wait() function" << endl;
  }
# else                 // HAVE_BACKGROUND
  printf("Q to quit\n\n");
  ORBTools::listen('q', 'Q');

  /**
   * Unregister Servants
   */
  printf("Shutting down...\n\n");

  ORBTools::unregisterServant("LogService", "", "LCT", "");
  ORBTools::unregisterServant("LogService", "", "LCC", "");

  /**
   * Stop threads
   */
  coreThread->stopThread();
  sendThread->stopThread();

  /**
   * Shutdown and delete the orb
   */
  ORBTools::kill();

  /**
   * Delete internal components (Servants are deleted by the ORB)
   */
  delete timeBuffer;
  delete simpleFilterManager;
  delete stateManager;
  delete componentList;
  delete toolList;
# endif              // HAVE_BACKGROUND
}
