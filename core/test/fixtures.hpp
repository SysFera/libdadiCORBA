/*
 * fixtures.hpp
 *
 * Author: hguemar
 *
 */

#ifndef FIXTURES_HPP_
#define FIXTURES_HPP_

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <csignal>

#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/filesystem.hpp>
#include <boost/process/all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include "utils.hpp"

namespace ba = boost::assign;
namespace bf = boost::filesystem;
namespace bp = boost::process;
namespace bs = boost::system;

#include "config_test.hpp"


template <const char *omniORBConfig>
class setDietEnvFixture {
public:
  setDietEnvFixture() {
    BOOST_TEST_MESSAGE("== Test setup [BEGIN]: setting environment ==");

    // Set env regarding omniORB
    setenv("OMNIORB_CONFIG", omniORBConfig, 1);

    // Set env regarding DIET compiled libraries
    bp::environment::iterator i_c;
    if (getenv(ENV_LIBRARY_PATH_NAME)) {
      std::string dietLibPath = std::string(ENV_LIBRARY_PATH)
        + std::string(getenv(ENV_LIBRARY_PATH_NAME));
      setenv(ENV_LIBRARY_PATH_NAME, dietLibPath.c_str(), 1);


      BOOST_TEST_MESSAGE("== Test setup [END]: setting environment ==");
    }

    // std::string dietPath = std::string(DIETAGENT_DIR)
    // + std::string(getenv("PATH"));
    // setenv("PATH", dietPath.c_str(), 1);
  }

  ~setDietEnvFixture() {
    BOOST_TEST_MESSAGE("== Test teardown [BEGIN]: unsetting environment ==");
    BOOST_TEST_MESSAGE("== Test teardown [END]: unsetting environment ==");
  }
};



/* Diet test fixture (aka test context)
 * basically setup omniNames before starting our test
 * and then cleanup after test has been executed
 */
template <const char *config, const char *endPoint, const char *initRef>
class OmniNamesFixture: public setDietEnvFixture<config> {
boost::scoped_ptr<bp::child> processNamingService;

public:
OmniNamesFixture(): processNamingService(NULL) {
  BOOST_TEST_MESSAGE(
    "== Test setup [BEGIN]: Launching OmniNames (" << config << ", " <<
    endPoint << ", " << initRef << ") ==");

  std::string exec;
  try {
    exec = bp::find_executable_in_path(OMNINAMES_COMMAND, OMNINAMES_PATH);
  } catch (bs::system_error &e) {
    BOOST_TEST_MESSAGE("can't find " << OMNINAMES_COMMAND << ": " << e.what());
    return;
  }

  BOOST_TEST_MESSAGE(OMNINAMES_COMMAND << " found: " << exec);
  logdir = bf::unique_path(OMNINAMES_LOGDIR "%%%%-%%%%-%%%%-%%%%").native();
  bf::create_directory(logdir);
  BOOST_TEST_MESSAGE("OmniNames log directory: " + logdir);


  // setup omniNames environment
  bp::context ctx;
  ctx.process_name = OMNINAMES_COMMAND;
  ctx.env["OMNINAMES_LOGDIR"] = logdir;
  ctx.env["OMNIORB_CONFIG"] = config;
  ctx.env["ORBsupportBooststrapAgent"] = "1";
  ctx.env["ORBInitRef"] = initRef;

  // redirect output to /dev/null
  ctx.streams[bp::stdout_id] = bp::behavior::null();
  ctx.streams[bp::stderr_id] = bp::behavior::null();

  // setup omniNames arguments
  std::vector<std::string> args = ba::list_of("-always")
                                    ("-start") ("2815")
                                    ("-ignoreport")
                                    ("-ORBendPoint") (endPoint);
  // launch Naming Service
  bp::child c = bp::create_child(exec, args, ctx);
  processNamingService.reset(utils::copy_child(c));
  for(long long i = 0; i <= 9999; ++i);

  boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEP_TIME));
  BOOST_TEST_MESSAGE("== Test setup [END]:  Launching OmniNames ==");
}

~OmniNamesFixture() {
  BOOST_TEST_MESSAGE("== Test teardown [BEGIN]: Stopping OmniNames ==");
  bf::remove_all(logdir);

  if (processNamingService) {
    try {
     processNamingService->terminate();
     processNamingService->wait();
    } catch (...) {
      BOOST_TEST_MESSAGE("== Problem while ending OmniNames ==");
    }
  }
  boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEP_TIME));
  BOOST_TEST_MESSAGE("== Test teardown [END]: Stopping OmniNames ==");
}

private:
std::string logdir;
};


template <const char *config, const char *omniORBConfig, class parentFixture>
class DietAgentFixture: public parentFixture {
boost::scoped_ptr<bp::child> processAgent;

public:
DietAgentFixture(): processAgent(NULL) {
  BOOST_TEST_MESSAGE(
    "== Test setup [BEGIN]:  Launching DIET Agent (config file: "
    << config << ", omniORB config: " << omniORBConfig <<
    ") ==");


  std::string exec;
  try {
    exec = bp::find_executable_in_path("dietAgent", DIETAGENT_DIR);
  } catch (bs::system_error &e) {
    BOOST_TEST_MESSAGE("can't find dietAgent: " << e.what());
    return;
  }

  BOOST_TEST_MESSAGE("dietAgent found: " << exec);

  // setup dietAgent environment
  bp::context ctx;
  ctx.process_name = "dietAgent";
  bp::environment::iterator i_c;
  i_c = ctx.env.find(ENV_LIBRARY_PATH_NAME);
  if (i_c != ctx.env.end()) {
    i_c->second = std::string(ENV_LIBRARY_PATH) + i_c->second;
  } else {
    ctx.env[ENV_LIBRARY_PATH_NAME] = ENV_LIBRARY_PATH;
  }
  ctx.env["OMNIORB_CONFIG"] = omniORBConfig;

  // redirect output to /dev/null
  ctx.streams[bp::stdout_id] = bp::behavior::null();
  ctx.streams[bp::stderr_id] = bp::behavior::null();


  // setup dietAGent arguments
  std::vector<std::string> args = ba::list_of(config);

  // launch diet Agent
  const bp::child c = bp::create_child(exec, args, ctx);

  processAgent.reset(utils::copy_child(c));
  boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEP_TIME));
  BOOST_TEST_MESSAGE("== Test setup [END]: Launching DIET Agent ==");
}

~DietAgentFixture() {
  BOOST_TEST_MESSAGE("== Test teardown [BEGIN]: Stopping DIET Agent ==");
  if (processAgent) {
    processAgent->terminate();
    processAgent->wait();
  }

  BOOST_TEST_MESSAGE("== Test teardown [END]: Stopping DIET Agent ==");
}
};


// must not be static
// should be a primitive type with an identifier name
char ConfigOmniORB[] = OMNIORB_CONFIG;
char OmniORBEndPoint[] = OMNINAMES_ENDPOINT;
char OmniORBInitRef[] = ORB_INIT_REF;
typedef OmniNamesFixture<ConfigOmniORB, OmniORBEndPoint,
                         OmniORBInitRef> omniNamesFixture;


char ConfigMasterAgent[] = "/home/keo/Bureau/MA.cfg";
typedef DietAgentFixture<ConfigMasterAgent, ConfigOmniORB,
                         omniNamesFixture> DietMAFixture;






#endif /* FIXTURES_HPP_ */
