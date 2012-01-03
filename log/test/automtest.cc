
/**
 * @file automtest.cc
 * @brief This file implements the libdadicorba log test
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

#include <boost/test/unit_test.hpp>
#include "test/fixtures.hpp"
#include "ORBMgr.hh"
#include "LogCentralTool_impl.hh"
#include "ORBTools.hh"

class dummyTool : public POA_ToolMsgReceiver,
		  public PortableServer::RefCountServantBase{
public :
  LogCentralTool_ptr myLCT;
  dummyTool (){}
  int
  disconnect (){}
  void
  run (){}
  void
  sendMsg(const log_msg_buf_t& msg) {}
  void
  setFilter(char* description_file_name){}




};


BOOST_FIXTURE_TEST_SUITE(test_suite, DietMAFixture)

using namespace std;

BOOST_AUTO_TEST_CASE(init)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_MESSAGE("test init finished\n");
}

BOOST_AUTO_TEST_CASE(activateServant)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  tag_list_t tag;
  LogCentralTool_impl* myLCT = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT));
  BOOST_MESSAGE("test ActivateServant finished\n");
}



BOOST_AUTO_TEST_CASE(registerServant)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  tag_list_t tag;
  LogCentralTool_impl* myLCT = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT));
  BOOST_REQUIRE(ORBTools::registerServant("testORBT", "", "LCT",
                                          "", myLCT->_this()));
  BOOST_MESSAGE("test RegisterServant finished\n");
}


BOOST_AUTO_TEST_CASE(unregisterServant)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  tag_list_t tag;
  LogCentralTool_impl* myLCT = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT));
  BOOST_REQUIRE(ORBTools::registerServant("testORBT", "", "LCTu",
                                          "", myLCT->_this()));
  BOOST_REQUIRE(ORBTools::unregisterServant("testORBT", "", "LCTu",
                                            ""));
  BOOST_MESSAGE("test unregisterServant finished\n");
}

/**
 * In this test:
 * 1) Initialiaze the ORB
 * 2) Create a CORBA object
 * 3) Register and activate the object
 * 4) Resolve the object to see if we get it right
 */
BOOST_AUTO_TEST_CASE(resolve)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  LogCentralTool_ptr myLCT;

  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);

  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT2));
  BOOST_REQUIRE(ORBTools::registerServant("testORBT", "", "LCT2",
                                          "", myLCT2->_this()));


  myLCT = ORBMgr::getMgr()->resolve<LogCentralTool, LogCentralTool_ptr>("testORBT", "LCT2");
  BOOST_REQUIRE(CORBA::is_nil(myLCT)==0);
  BOOST_MESSAGE("test Resolve finished\n");
}// test resolve

BOOST_AUTO_TEST_CASE(islocal)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT2));
  BOOST_REQUIRE(ORBTools::registerServant("testORBT", "", "LCT3",
                                          "", myLCT2->_this()));

  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->isLocal("testORBT", "LCT3"));
  BOOST_MESSAGE("Test isLocal finished");
}

BOOST_AUTO_TEST_CASE(getior)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  BOOST_REQUIRE(ORBTools::init(argc, argv, 0, 0));
  BOOST_REQUIRE(ORBTools::activateServant(myLCT2));
  BOOST_REQUIRE(ORBTools::registerServant("testORBT", "", "LCT4",
                                          "", myLCT2->_this()));

  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->getIOR("testORBT", "LCT4")!="");
  BOOST_MESSAGE("Test getior finished");
}

BOOST_AUTO_TEST_CASE(bind)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  dummyTool tool = dummyTool();
  ORBMgr::getMgr()->bind("LogServiceT", "toto", tool._this(), true);
  BOOST_MESSAGE("Test bind finished");
}

BOOST_AUTO_TEST_CASE(rebind)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  dummyTool tool = dummyTool();
  ORBMgr::getMgr()->rebind("LogServiceT", "toto", tool._this());
  BOOST_MESSAGE("Test rebind finished");
}

BOOST_AUTO_TEST_CASE(unbind)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);
  tag_list_t tag;
  LogCentralTool_impl* myLCT2 = new LogCentralTool_impl(NULL, NULL, NULL, NULL, &tag);
  dummyTool tool = dummyTool();
  ORBMgr::getMgr()->bind("LogServiceT", "toto2", tool._this(), true);
  ORBMgr::getMgr()->unbind("LogServiceT", "toto2");
  BOOST_MESSAGE("Test unbind finished");
}

BOOST_AUTO_TEST_SUITE_END()


