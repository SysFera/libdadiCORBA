/**
 * @file SSHTunnel.cc
 *
 * @brief  DIET forwarder implementation - SSH Tunnel implementation
 *
 * @author  Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *
 * @section Licence
 *   |LICENCE|
 */

#include "SSHTunnel.hh"

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <fstream>
#include <iterator>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <unistd.h>    // For sleep & fork functions
#include <sys/wait.h>  // For waitpid function

/* To find a free tcp port. */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/**/

#include "dadi/Logging/ConsoleChannel.hh"
#include "dadi/Logging/Logger.hh"
#include "dadi/Logging/Message.hh"


const unsigned int DEFAULT_WAITING_TIME = 10;

std::string SSHTunnel::mcmdFormat = "%p -l %u %s -p %P -N";
std::string SSHTunnel::mcmdFormatDefault = "%p %s -N";
std::string SSHTunnel::mlocalFormat = "-L%l:%h:%R";
std::string SSHTunnel::mremoteFormat = "-R%r:%h:%L";
std::string SSHTunnel::mkeyFormat = "-i %k";

/* Return the current session login. */
std::string
SSHConnection::userLogin() {
  char *result = getlogin();

// Init logger
  dadi::LoggerPtr  logger;
  dadi::ChannelPtr cc;
  logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  cc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(cc);

  if (result == NULL) {
    logger->log(dadi::Message("SSHTunnel",
                              "Unable to determine the user login.\n",
                              dadi::Message::PRIO_DEBUG));
    return "";
  }
  return result;
}

/* Get the default path to user SSH key. If the user does not use
 * a private key for connection, return empty std::string.
 */

std::string
SSHConnection::userKey() {
  char *home = getenv("HOME");
  std::string path;

  /* Try the RSA key default path. */
  path = ((home == NULL) ?
          std::string("") : std::string(home)) + "/.ssh/id_rsa";
  std::ifstream f(path.c_str());
  if (f.is_open()) {
    f.close();
    return path;
  }

  /* Try the DSA key default path. */
  path = ((home == NULL) ?
          std::string("") : std::string(home)) + "/.ssh/id_dsa";
  f.open(path.c_str());
  if (f.is_open()) {
    f.close();
    return path;
  }

  /* None of the two default keys were found */
  return "";
} // userKey

SSHConnection::SSHConnection() {
  setSshPath("/usr/bin/ssh");
// Init logger
  mlogger = dadi::Logger::getLogger("org.dadicorba");
  mlogger->setLevel(dadi::Message::PRIO_TRACE);
  mcc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  mlogger->setChannel(mcc);
}

SSHConnection::SSHConnection(const std::string &sshHost,
                             const std::string &sshPort,
                             const std::string &login,
                             const std::string &keyPath,
                             const std::string &sshPath) {
  setSshHost(sshHost);
  setSshPort(sshPort);
  setSshLogin(login);
  setSshKeyPath(keyPath);
  setSshPath(sshPath);
// Init logger
  mlogger = dadi::Logger::getLogger("org.dadicorba");
  mlogger->setLevel(dadi::Message::PRIO_TRACE);
  mcc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  mlogger->setChannel(mcc);
}

const std::string &
SSHConnection::getSshHost() const {
  return msshHost;
}

const std::string &
SSHConnection::getSshPath() const {
  return msshPath;
}

const std::string &
SSHConnection::getSshPort() const {
  return msshPort;
}

const std::string &
SSHConnection::getSshLogin() const {
  return mlogin;
}

const std::string &
SSHConnection::getSshKeyPath() const {
  return mkeyPath;
}

const std::string &
SSHConnection::getSshOptions() const {
  return moptions;
}

void
SSHConnection::setSshHost(const std::string &host) {
  if (host != "") {
    this->msshHost = host;
  }
}

void
SSHConnection::setSshPath(const std::string &path) {
  if (path != "") {
    this->msshPath = path;
  }
}

void
SSHConnection::setSshPort(const std::string &port) {
  if (port != "") {
    this->msshPort = port;
  }
}

void
SSHConnection::setSshPort(const int port) {
  std::ostringstream os;
  os << port;
  this->msshPort = os.str();
}

void
SSHConnection::setSshLogin(const std::string &login) {
  if (login != "") {
    this->mlogin = login;
  }
}

void
SSHConnection::setSshKeyPath(const std::string &path) {
  this->mkeyPath = path;
}

void
SSHConnection::setSshOptions(const std::string &options) {
  this->moptions = options;
}

/* Replace "s" by "r" in "str". */
void
replace(const std::string &s, const std::string &r, std::string &str) {
  size_t pos;
  if ((pos = str.find(s)) != std::string::npos) {
    str.erase(pos, s.length());
    str.insert(pos, r);
  }
}

/* Try to find a free TCP port. "sfd" is
 * the socket file descriptor used to find
 * the port.
 */
std::string
freeTCPport() {
  std::ostringstream os;
  struct sockaddr_in sck;

  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  sck.sin_family = AF_INET;
  sck.sin_addr.s_addr = INADDR_ANY;
  sck.sin_port = 0;
  bind(sfd, (struct sockaddr *) &sck, sizeof(sck));
  socklen_t len = sizeof(sck);
  getsockname(sfd, (struct sockaddr *) &sck, &len);
  os << sck.sin_port;
  close(sfd);

  return os.str();
} // freeTCPport

std::string
SSHTunnel::makeCmd() {
  std::string result;

  if (getSshLogin() == "" && getSshPort() == "" && getSshKeyPath() == "") {
    result = mcmdFormatDefault;
  } else {
    result = mcmdFormat;
  }
  replace("%p", getSshPath(), result);
  replace("%u", getSshLogin(), result);
  if (getSshLogin() != "" && getSshPort() == "") {
    setSshPort(22);
  }
  replace("%P", getSshPort(), result);
  replace("%s", getSshHost(), result);

  if (mcreateTo) {
    result += " ";
    result += mlocalFormat;
    if (mlocalPortFrom == "") {
      mlocalPortFrom = freeTCPport();
      replace("%l", mlocalPortFrom, result);
    }
    replace("%h", mremoteHost, result);
    replace("%R", mremotePortTo, result);
  }
  if (getSshKeyPath() != "") {
    result += " ";
    result += mkeyFormat;
    replace("%k", getSshKeyPath(), result);
  }

  if (mcreateFrom) {
    result += " ";
    result += mremoteFormat;
    replace("%L", mlocalPortTo, result);
    replace("%h", mremoteHost, result);
    replace("%r", mremotePortFrom, result);
  }

  if (getSshOptions() != "") {
    result += " " + getSshOptions();
  }

  return result;
} // makeCmd

SSHTunnel::SSHTunnel(): SSHConnection() {
  this->mcreateTo = false;
  this->mcreateFrom = false;
  this->mwaitingTime = DEFAULT_WAITING_TIME;
}

/* Constructor for bi-directionnal SSH tunnel. */
SSHTunnel::SSHTunnel(const std::string &sshHost,
                     const std::string &remoteHost,
                     const std::string &localPortFrom,
                     const std::string &remotePortTo,
                     const std::string &remotePortFrom,
                     const std::string &localPortTo,
                     const bool createTo,
                     const bool createFrom,
                     const std::string &sshPath,
                     const std::string &sshPort,
                     const std::string &login,
                     const std::string &keyPath)
  : SSHConnection(sshHost, sshPort, login, keyPath, sshPath) {
  this->mremoteHost = remoteHost;
  this->mlocalPortFrom = localPortFrom;
  this->mremotePortTo = remotePortTo;
  this->mremotePortFrom = remotePortFrom;
  this->mlocalPortTo = localPortTo;
  this->mcreateTo = createTo;
  this->mcreateFrom = createFrom;
  this->mwaitingTime = DEFAULT_WAITING_TIME;
}

/* Constructor for unidirectionnal SSH tunnel. */
SSHTunnel::SSHTunnel(const std::string &sshHost,
                     const std::string &remoteHost,
                     const std::string &localPortFrom,
                     const std::string &remotePortTo,
                     const bool createTo,
                     const std::string &sshPath,
                     const std::string &sshPort,
                     const std::string &login,
                     const std::string &keyPath)
  : SSHConnection(sshHost, sshPort, login, keyPath, sshPath) {
  this->mremoteHost = remoteHost;
  this->mlocalPortFrom = localPortFrom;
  this->mremotePortTo = remotePortTo;
  this->mcreateTo = createTo;
  this->mcreateFrom = false;
  this->mwaitingTime = DEFAULT_WAITING_TIME;
}

SSHTunnel::~SSHTunnel() {
  close();
}

void
SSHTunnel::open() {
  if (!mcreateTo && !mcreateFrom) {
    return;
  }

  std::vector<std::string> tokens;
  std::string command = makeCmd();
  std::istringstream is(command);

  std::copy(std::istream_iterator<std::string>(is),
            std::istream_iterator<std::string>(),
            std::back_inserter<std::vector<std::string> >(tokens));

  char *argv[tokens.size() + 1];
  argv[tokens.size()] = NULL;

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    argv[i] = strdup(tokens[i].c_str());
  }

  mpid = fork();
  if (mpid == -1) {
    throw std::runtime_error("Error forking process.");
  }
  if (mpid == 0) {
    if (execvp(argv[0], argv)) {
      mlogger->log(dadi::Message("SSHTunnel",
                                 "Error executing command " + command,
                                 dadi::Message::PRIO_DEBUG));
    }
  }
  for (unsigned int i = 0; i < tokens.size(); ++i) {
    free(argv[i]);
  }

  std::ostringstream intval;
  intval << this->mwaitingTime;

  mlogger->log(dadi::Message("SSHTunnel",
                             "Sleep " + intval.str()
                             + " s. waiting for tunnel\n",
                             dadi::Message::PRIO_DEBUG));
  sleep(this->mwaitingTime);

  mlogger->log(dadi::Message("SSHTunnel",
                             "Wake up!\n",
                             dadi::Message::PRIO_DEBUG));
} // open

void
SSHTunnel::close() {
  if (!mcreateTo && !mcreateFrom) {
    return;
  }
  if (mpid && kill(mpid, SIGTERM)) {
    if (kill(mpid, SIGKILL)) {
      throw std::runtime_error("Unable to stop the ssh process");
    }
  }
  mpid = 0;
} // close

const std::string &
SSHTunnel::getRemoteHost() const {
  return mremoteHost;
}

int
SSHTunnel::getLocalPortFrom() const {
  int res;
  std::istringstream is(mlocalPortFrom);

  is >> res;

  return res;
}

int
SSHTunnel::getLocalPortTo() const {
  int res;
  std::istringstream is(mlocalPortTo);

  is >> res;

  return res;
}

int
SSHTunnel::getRemotePortFrom() const {
  int res;
  std::istringstream is(mremotePortFrom);

  is >> res;

  return res;
}

int
SSHTunnel::getRemotePortTo() const {
  int res;
  std::istringstream is(mremotePortTo);

  is >> res;

  return res;
}

void
SSHTunnel::setRemoteHost(const std::string &host) {
  this->mremoteHost = host;
}

void
SSHTunnel::setLocalPortFrom(const std::string &port) {
  this->mlocalPortFrom = port;
}

void
SSHTunnel::setLocalPortFrom(const int port) {
  std::ostringstream os;
  os << port;
  this->mlocalPortFrom = os.str();
}

void
SSHTunnel::setRemotePortTo(const std::string &port) {
  this->mremotePortTo = port;
}

void
SSHTunnel::setRemotePortTo(const int port) {
  std::ostringstream os;
  os << port;
  this->mremotePortTo = os.str();
}

void
SSHTunnel::setRemotePortFrom(const std::string &port) {
  this->mremotePortFrom = port;
}

void
SSHTunnel::setRemotePortFrom(const int port) {
  std::ostringstream os;
  os << port;
  this->mremotePortFrom = os.str();
}

void
SSHTunnel::setLocalPortTo(const std::string &port) {
  this->mlocalPortTo = port;
}

void
SSHTunnel::setLocalPortTo(const int port) {
  std::ostringstream os;
  os << port;
  this->mlocalPortTo = os.str();
}

void
SSHTunnel::setWaitingTime(const unsigned int time) {
  if (time != 0) {
    this->mwaitingTime = time;
  }
}

void
SSHTunnel::createTunnelTo(const bool create) {
  this->mcreateTo = create;
}

void
SSHTunnel::createTunnelFrom(const bool create) {
  this->mcreateFrom = create;
}


SSHCopy::SSHCopy(const std::string &sshHost,
                 const std::string &remoteFilename,
                 const std::string &localFilename) {
  setSshHost(sshHost);
  this->mremoteFilename = remoteFilename;
  this->mlocalFilename = localFilename;
}

bool
SSHCopy::getFile() const {
  std::vector<std::string> tokens;
  int status;
  std::string command;

  command = getSshPath();
  if (getSshPort() != "") {
    command += " -P " + getSshPort();
  }
  if (getSshKeyPath() != "") {
    command += " -i " + getSshKeyPath();
  }
  if (getSshLogin() != "") {
    command += " " + getSshLogin() + "@" + getSshHost() + ":" + mremoteFilename;
  } else {
    command += " " + getSshHost() + ":" + mremoteFilename;
  }

  command += " " + mlocalFilename;

  std::istringstream is(command);

  std::copy(std::istream_iterator<std::string>(is),
            std::istream_iterator<std::string>(),
            std::back_inserter<std::vector<std::string> >(tokens));

  char *argv[tokens.size() + 1];
  argv[tokens.size()] = NULL;

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    argv[i] = strdup(tokens[i].c_str());
  }
  mpid = fork();
  if (mpid == -1) {
    throw std::runtime_error("Error forking process.");
  }
  if (mpid == 0) {
    fclose(stdout);
    if (execvp(argv[0], argv)) {
      mlogger->log(dadi::Message("SSHTunnel",
                                 "Error executing command " + command,
                                 dadi::Message::PRIO_DEBUG));
    }
  }

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    free(argv[i]);
  }

  if (waitpid(mpid, &status, 0) == -1) {
    throw std::runtime_error("Error executing scp command");
  }
  // FIXME: WTF ?
  return ((WIFEXITED(status) != 0) ? (WEXITSTATUS(status) == 0) : false);
} // getFile

bool
SSHCopy::putFile() const {
  std::vector<std::string> tokens;
  int status;
  std::string command;

  command = getSshPath();
  if (getSshPort() != "") {
    command += " -P " + getSshPort();
  }
  if (getSshKeyPath() != "") {
    command += " -i " + getSshKeyPath();
  }

  command += " " + mlocalFilename;
  if (getSshLogin() != "") {
    command += " " + getSshLogin() + "@" + getSshHost() + ":" + mremoteFilename;
  } else {
    command += " " + getSshHost() + ":" + mremoteFilename;
  }

  std::istringstream is(command);

  std::copy(std::istream_iterator<std::string>(is),
            std::istream_iterator<std::string>(),
            std::back_inserter<std::vector<std::string> >(tokens));

  char *argv[tokens.size() + 1];
  argv[tokens.size()] = NULL;

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    argv[i] = strdup(tokens[i].c_str());
  }

  mpid = fork();
  if (mpid == -1) {
    throw std::runtime_error("Error forking process.");
  }
  if (mpid == 0) {
    fclose(stdout);
    if (execvp(argv[0], argv)) {
      mlogger->log(dadi::Message("SSHTunnel",
                                 "Error executing command " + command,
                                 dadi::Message::PRIO_DEBUG));
    }
  }

  for (unsigned int i = 0; i < tokens.size(); ++i) {
    free(argv[i]);
  }

  if (waitpid(mpid, &status, 0) == -1) {
    throw std::runtime_error("Error executing scp command");
  }

  return ((WIFEXITED(status) != 0) ? (WEXITSTATUS(status) == 0) : false);
} // putFile
