/**
 * @file automtest_sshtunnel.cpp
 * @brief This file implements the libdadi tests for ssh tunnel classes
 * @author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * @section Licence
 *  |LICENCE|
 */


#include <boost/test/unit_test.hpp>

#include <iostream>
#include "SSHTunnel.hh"

BOOST_AUTO_TEST_SUITE(test_suite)


using namespace std;

static const std::string sshHost = "localhost";
static const std::string sshPort = "22";
static const std::string login   = "toto";
static const std::string keyPath = "/home/toto/.ssh";
static const std::string sshPath = "/home/toto";
static const std::string sshOpt  = "option";

static const std::string rHost   = "rhost";
static const std::string lHost   = "lhost";
static const      int    rPortf  = 1;
static const      int    lPortt  = 2;
static const std::string rPortfs = "1";
static const std::string lPortts = "2";
static const      int    rPortt  = 3;
static const      int    lPortf  = 4;
static const std::string rPortts = "3";
static const std::string lPortfs = "4";


BOOST_AUTO_TEST_CASE(gettersSSHConnection)
{
  SSHConnection conn = SSHConnection(sshHost, sshPort,
                                     login, keyPath,
                                     sshPath);
  BOOST_REQUIRE(conn.getSshHost()==sshHost);
  BOOST_REQUIRE(conn.getSshPort()==sshPort);
  BOOST_REQUIRE(conn.getSshLogin()==login);
  BOOST_REQUIRE(conn.getSshKeyPath()==keyPath);
  BOOST_REQUIRE(conn.getSshPath()==sshPath);
}

BOOST_AUTO_TEST_CASE(settersSSHConnection)
{
  SSHConnection conn = SSHConnection("", "",
                                     "", "",
                                     "");
  conn.setSshHost(sshHost);
  conn.setSshPort(sshPort);
  conn.setSshLogin(login);
  conn.setSshKeyPath(keyPath);
  conn.setSshPath(sshPath);

  BOOST_REQUIRE(conn.getSshHost()==sshHost);
  BOOST_REQUIRE(conn.getSshPort()==sshPort);
  BOOST_REQUIRE(conn.getSshLogin()==login);
  BOOST_REQUIRE(conn.getSshKeyPath()==keyPath);
  BOOST_REQUIRE(conn.getSshPath()==sshPath);
}



BOOST_AUTO_TEST_CASE(settersSSHOpt)
{
  SSHConnection conn = SSHConnection("", "",
                                     "", "",
                                     "");
  conn.setSshOptions(sshOpt);

  BOOST_REQUIRE(conn.getSshOptions()==sshOpt);
}


BOOST_AUTO_TEST_CASE(gettersSSHTunnel)
{
  SSHTunnel conn = SSHTunnel(sshHost, rHost, lPortfs, rPortts, rPortfs, lPortts, false, false, sshPath, sshPort, login, keyPath);

  BOOST_REQUIRE(conn.getRemoteHost()==rHost);
  BOOST_REQUIRE(conn.getLocalPortFrom()==lPortf);
  BOOST_REQUIRE(conn.getLocalPortTo()==lPortt);
  BOOST_REQUIRE(conn.getRemotePortTo()==rPortt);
  BOOST_REQUIRE(conn.getRemotePortFrom()==rPortf);
}

BOOST_AUTO_TEST_CASE(settersSSHTunnel)
{
  SSHTunnel conn = SSHTunnel("", "", "", "", "", "", false, false, "", "", "", "");

// Native type setter
  conn.setRemoteHost(rHost);
  conn.setLocalPortFrom(lPortf);
  conn.setLocalPortTo(lPortt);
  conn.setRemotePortTo(rPortt);
  conn.setRemotePortFrom(rPortf);

  BOOST_REQUIRE(conn.getRemoteHost()==rHost);
  BOOST_REQUIRE(conn.getLocalPortFrom()==lPortf);
  BOOST_REQUIRE(conn.getLocalPortTo()==lPortt);
  BOOST_REQUIRE(conn.getRemotePortTo()==rPortt);
  BOOST_REQUIRE(conn.getRemotePortFrom()==rPortf);

// String type setter
  conn.setLocalPortFrom(lPortfs);
  conn.setLocalPortTo(lPortts);
  conn.setRemotePortTo(rPortts);
  conn.setRemotePortFrom(rPortfs);

  BOOST_REQUIRE(conn.getLocalPortTo()==lPortt);
  BOOST_REQUIRE(conn.getRemotePortTo()==rPortt);
  BOOST_REQUIRE(conn.getRemotePortFrom()==rPortf);
  BOOST_REQUIRE(conn.getLocalPortFrom()==lPortf);


}

BOOST_AUTO_TEST_SUITE_END()

// THE END
