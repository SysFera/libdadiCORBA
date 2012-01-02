// diet_config_tests.h.in

#ifndef _DIET_CONFIG_H_IN_
#define _DIET_CONFIG_H_IN_

#define BIN_DIR "/home/keo/Bureau/depot_git_edf/vishnu/build/bin"
#define CONFIG_DIR "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg"
#define DIETAGENT_DIR "/usr/local/bin"
#define DIETMADAG_DIR ""

#define ENV_LIBRARY_PATH_NAME "LD_LIBRARY_PATH"
#define ENV_LIBRARY_PATH "/usr/local/lib:/home/keo/Bureau/depot_git_edf/vishnu/build/src/client/:/home/keo/Bureau/depot_git_edf/vishnu/build/src/SeD/:"

// Sleep time in milliseconds
#define SLEEP_TIME 2000

// omniNames specific options
#define LOCAL_TEST_PORT "2809"
#define OMNINAMES_LOGDIR "/home/keo/Bureau/depot_git_edf/vishnu/build/core/test/log/"
#define OMNIORB_CONFIG "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/omniORB4_testing.cfg"
#define ORB_INIT_REF "NameService=corbaname::127.0.0.1:2809"
#define OMNINAMES_ENDPOINT "giop:tcp:127.0.0.1:2809"
#define OMNINAMES_COMMAND "omniNames"
#define OMNINAMES_PATH "/usr/local/bin"
#define OMNINAMES_LIBRARY_PATH "/usr/local/lib"

// LogService specific options
#define LOGCENTRAL_PATH "/home/keo/install/logService/bin/LogCentral"
#define LOGSERVICE_COMMAND "LogCentral"
#define LOGSERVICE_CONFIG "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/logservice_testing.cfg"
#define DIETLOGTOOL_PATH ""
#define DIETLOGTOOL_COMMAND "DIETtestTool"

// dietAgent specific options
#define MASTER_AGENT_CONFIG "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/master_agent_testing.cfg"
#define LOCAL_AGENT_CONFIG "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/local_agent_testing.cfg"
#define MADAG_CONFIG ""

// SeD specific options
#define SIMPLE_ADD_SED_CONFIG "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/SeD_testing.cfg"
#define SIMPLE_ADD_SED_CONFIG_LA "/home/keo/Bureau/depot_git_edf/vishnu/build/test_files/cfg/SeD_testingLA.cfg"
#define EXAMPLES_DIR ""


// DAGDA working directory
#define MA_DAGDA_DIR     "/tmp/MA_test"
#define LA_DAGDA_DIR     "/tmp/LA_test"
#define SED_DAGDA_DIR    "/tmp/SeD_test"
#define CLIENT_DAGDA_DIR "/tmp/Client_test"

// Files that are used for testing transfers
#define TRANSFER_FILE_TEST1 ""
#define TRANSFER_FILE_TEST2 ""

// Workflows
#define WF_DIRECTORY ""

#endif /* _DIET_CONFIG_H_IN_ */

