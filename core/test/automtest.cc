/**
 * @file automtest.cc
 * @brief This file implements the libdadicorba tests
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */

#include <boost/test/unit_test.hpp>

#include <iostream>

#include "test/fixtures.hpp"
#include "ORBMgr.hh"
#include "monitor/LogCentralToolFwdr_impl.hh"



static const std::string ior = "IOR:010000002b00000049444c3a6f6d672e6f72672f436f734e616d696e672f4e616d696e67436f6e746578744578743a312e300000010000000000000074000000010102000d0000003139322e3136382e312e31320000f90a0e000000ff00973cf34e010017410000000200000300000000000000080000000100000000545441010000001c000000010000000100010001000000010001050901010001000000090101000354544108000000973cf34e01001741 MA0  nobject IOR:010000001400000049444c3a4d61737465724167656e743a312e3000010000000000000068000000010102000d0000003139322e3136382e312e31320000138414000000ff6269646972fe9e3cf34e0100176100000000000200000000000000080000000100000000545441010000001c00000001000000010001000100000001000105090101000100000009010100";


BOOST_FIXTURE_TEST_SUITE(test_suite, DietMAFixture)

using namespace std;

BOOST_AUTO_TEST_CASE(getORBMgr)
{
  std::cout << "debut" << std::endl;
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
}


BOOST_AUTO_TEST_CASE(getHost)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  std::cout << "Host gotten: " << ORBMgr::getMgr()->getHost(ior) << std::endl;
  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->getHost(ior)=="192.168.1.12");
}

BOOST_AUTO_TEST_CASE(getPort)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  std::cout << "Port gotten: " << ORBMgr::getMgr()->getPort(ior) << std::endl;
  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->getPort(ior)==2809);
}

BOOST_AUTO_TEST_CASE(getTypeID)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  std::cout << "TypeID gotten: " << ORBMgr::getMgr()->getTypeID(ior) << std::endl;
  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->getTypeID(ior)=="IDL:omg.org/CosNaming/NamingContextExt:1.0");
}

BOOST_AUTO_TEST_CASE(listctxtobj)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  std::cout << "Nb objects in testORBT gotten: " << ORBMgr::getMgr()->list("dietAgent").size() << std::endl;
  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->list("dietAgent").size()==1);
}

BOOST_AUTO_TEST_CASE(listctxt)
{
  int argc = 1;
  char** argv = (char **) malloc (sizeof (char*));
  std::string prog = "test";
  argv[0] = (char *) malloc (sizeof(char)*prog.length());
  memcpy(argv[0], prog.c_str(), prog.length());
  ORBMgr::init(argc, argv);

  BOOST_REQUIRE(ORBMgr::getMgr()!=0);
  BOOST_REQUIRE(ORBMgr::getMgr()->contextList().size()==2);
}


BOOST_AUTO_TEST_SUITE_END()


