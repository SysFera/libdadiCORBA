/**
 * @file LogCentral.cc
 *
 * @brief LogCentral binary
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>

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
#include "LogOptions.hh"
#include "ReadConfig.hh"
#include "utils/LocalTime.hh"
#include "TimeBuffer.hh"

// threads
#include "SendThread.hh"
#include "CoreThread.hh"

// helpers
#include "ORBTools.hh"
#include "ORBMgr.hh"

int
main(int argc, char** argv) {
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
  for (int i = 0; i < argc; i++) {
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
      printf(" -config FILENAME    "
             "Reads the config file specified in FILENAME. "
             "Overrides\n                     the filename specified in "
             "$LOGCENTRAL_CONFIG\n"
             " -ORBparameter       Sets parameters of omniORB as specified "
             "in the manual\n"
             " -help or --help     Display this message and exit\n\n");
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
  bool success = true;

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
  simpleFilterManager =
    new SimpleFilterManager(toolList, componentList, stateTags);
  timeBuffer = new TimeBuffer();

  sendThread = new SendThread(toolList);
  coreThread = new CoreThread(timeBuffer, stateManager,
                              simpleFilterManager, toolList);

  myLCT = new LogCentralTool_impl(toolList, componentList,
                                  simpleFilterManager, stateManager, allTags);
  myLCC =
    new LogCentralComponent_impl(componentList, simpleFilterManager,
                                 timeBuffer);

  delete allTags;
  delete stateTags;
  delete readConfig;

  // ADD *******
  ORBMgr::init(argc, argv);

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
  if (!ORBTools::registerServant("LogServiceT", "", "LCT",
                                 "", myLCT->_this())) {
    printf("Could not register the servants at the NamingService\n");
    exit(1);
  }

  // Inscription in the others fwdr
  ORBMgr::getMgr()->fwdsBind("LogServiceT", "LCT",
                                ORBMgr::getMgr()->getIOR(myLCT->_this()));

  if (!ORBTools::activateServant(myLCC)) {
    printf("Could not activate servants\n");
    exit(1);
  }
  CORBA::Object* myLCCref = myLCC->_this();
  if (!ORBTools::registerServant("LogServiceC", "", "LCC", "", myLCCref)) {
    printf("Could not register the servants at the NamingService\n");
    exit(1);
  }
  // Inscription in the others fwdr
  ORBMgr::getMgr()->fwdsBind("LogServiceC", "LCC",
                                ORBMgr::getMgr()->getIOR(myLCC->_this()));

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
  printf("Got background option.\n");
  try {
    ORBMgr::getMgr()->wait();
  } catch (...) {
    std::cerr << "Error while exiting the ORBMgr::wait() function\n";
  }
  ORBTools::unregisterServant("LogServiceT", "", "LCT", "");
  ORBTools::unregisterServant("LogServiceC", "", "LCC", "");
}
