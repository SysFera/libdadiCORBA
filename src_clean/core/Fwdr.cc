/**
* @file Fwdr.cc
*
* @brief   DIET forwarder implementation - Forwarder executable
*
* @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
*
* @section Licence
*   |LICENSE|
*/

#include "Forwarder.hh"
#include "CorbaForwarder.hh"
#include "ORBMgr.hh"
#include "utils/SSHTunnel.hh"
#include "utils/Options.hh"

#include "Fwdr.hh"


#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <omniORB4/CORBA.h>

#include <unistd.h>     // For sleep function

#include "dadi/Logging/ConsoleChannel.hh"
#include "dadi/Logging/Logger.hh"
#include "dadi/Logging/Message.hh"


int
main(int argc, char* argv[], char* envp[]) {
// Init logger
  dadi::LoggerPtr logger;
  dadi::ChannelPtr cc;
  logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  cc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(cc);
  /* Forwarder configuration. */
  FwrdConfig cfg(argv[0]);

  Options opt(&cfg, argc, argv, envp);
  /* Mandatory parameter. */
  opt.setOptCallback("--name", name);

  /* Mandatory when creating tunnels. */
  opt.setOptCallback("--peer-name", peer_name);
  opt.setOptCallback("--ssh-host", ssh_host);
  /* Optionnal, set to "localhost" by default. */
  opt.setOptCallback("--remote-host", remote_host);
  /* Optionnal, we try to determine it automatically. */
  opt.setOptCallback("--remote-port", remote_port_from);
  /* Optionnal, set waiting time for tunnel creation */
  opt.setOptCallback("--tunnel-wait", tunnel_wait);

  /* Optionnal */
  opt.setOptCallback("--ssh-port", ssh_port);
  opt.setOptCallback("--ssh-login", ssh_login);
  opt.setOptCallback("--ssh-key", key_path);

  /* Optionnal parameters/flags. */
  opt.setOptCallback("--nb-retry", nb_retry);
  opt.setOptCallback("--peer-ior", peer_ior);

  opt.processOptions();
  if (cfg.getSshHost() != "") {
    cfg.createTo(true);
    cfg.createFrom(true);
  }

  if (cfg.getName() == "") {
    std::ostringstream name;
    char host[256];

    gethostname(host, 256);
    host[255]='\0';

    std::transform(host, host+strlen(host), host, change);
    name << "Forwarder-" << host << "-" << getpid();

    logger->log(dadi::Message("Fwdr",
                              "Missing parameter: name (use --name to fix it) \n Use default name: " + name.str() + "\n",
                               dadi::Message::PRIO_DEBUG));

    cfg.setName(name.str());
  }

  if (cfg.createFrom()) {
    if (cfg.getPeerName() == ""
        || cfg.getSshHost() == "") {
      logger->log(dadi::Message("Fwdr",
                                "Missing parameter(s) to create tunnel. Mandatory parameters: \n \t - Peer name (--peer-name <name>) \n \t - SSH host (--ssh-host <host>)",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
  }

  SSHTunnel tunnel;
  CorbaForwarder* forwarder;
  try {
    forwarder = new CorbaForwarder(cfg.getName());
  } catch (std::exception &e) {
      logger->log(dadi::Message("Fwdr",
                                e.what(),
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
  }
  ORBMgr::init(argc, argv);
  ORBMgr* mgr = ORBMgr::getMgr();
  std::string ior;
  int count = 0;

  mgr->activate(forwarder);
  do {
    try {
      mgr->bind(FWRDCTXT, cfg.getName(), forwarder->_this(), true);
      break;
    } catch (CORBA::TRANSIENT& err) {
      logger->log(dadi::Message("Fwdr",
                                "Error when binding the forwarder "
                                + cfg.getName() + " \n",
                                dadi::Message::PRIO_DEBUG));
      if (count++<cfg.getNbRetry()) {
        sleep(5);
        continue;
      }
      mgr->deactivate(forwarder);
      return EXIT_FAILURE;
    }
  } while (true);

  /* Write the IOR to a file on /tmp. */
  ior = mgr->getIOR(forwarder->_this());
  std::string iorFilename("/tmp/DIET-forwarder-ior-");
  iorFilename += cfg.getName() + ".tmp";
  std::ofstream of(iorFilename.c_str(), std::ios_base::trunc);

  if (!of.is_open()) {
    logger->log(dadi::Message("Fwdr",
                              "cannot open file " + iorFilename
                              + " to store the IOR",
                              dadi::Message::PRIO_DEBUG));
  } else {
    logger->log(dadi::Message("Fwdr",
                              "Write IOR to " + iorFilename + " \n",
                              dadi::Message::PRIO_DEBUG));
    if (cfg.createFrom()) {  // Creating tunnel(s)
      std::istringstream is(cfg.getRemotePortFrom());
      int port;

      is >> port;
      of << ORBMgr::convertIOR(ior, cfg.getRemoteHost(), port);
    } else {  // Waiting for connexion.
      of << ior;
    }
    of << std::endl;
    of << freeTCPport();  // also write a free port
    of.close();
  }

    logger->log(dadi::Message("Fwdr",
                              "Forwarder: " + ior + " \n",
                              dadi::Message::PRIO_DEBUG));

  tunnel.setSshHost(cfg.getSshHost());
  tunnel.setRemoteHost(cfg.getRemoteHost());

  tunnel.setSshPath(cfg.getSshPath());
  tunnel.setSshPort(cfg.getSshPort());
  tunnel.setSshLogin(cfg.getSshLogin());
  tunnel.setSshKeyPath(cfg.getSshKeyPath());

  tunnel.setWaitingTime(cfg.getWaitingTime());

  /* Manage the peer IOR. */
  if (cfg.getPeerIOR().empty() && cfg.createFrom()) {
    /* Try to retrieve the peer IOR. */
    SSHCopy copy(cfg.getSshHost(),
                 "/tmp/DIET-forwarder-ior-" + cfg.getPeerName() + ".tmp",
                 "/tmp/DIET-forwarder-ior-" + cfg.getPeerName() + ".tmp");
    copy.setSshPath("/usr/bin/scp");
    copy.setSshPort(cfg.getSshPort());
    copy.setSshLogin(cfg.getSshLogin());
    copy.setSshKeyPath(cfg.getSshKeyPath());
    try {
      if (copy.getFile()) {
        logger->log(dadi::Message("Fwdr",
                                  "Got remote IOR file",
                                  dadi::Message::PRIO_DEBUG));
        cfg.setPeerIOR("/tmp/DIET-forwarder-ior-" + cfg.getPeerName() + ".tmp");
      } else {
        logger->log(dadi::Message("Fwdr",
                                  "Could not get remote IOR file.\n" \
                                  "Please check that you can scp files" \
                                  "between the ssh host and this host, _n" \
                                  "or specify the remote IOR with the following option:\n" \
                                  "\t- Remote IOR (--peer-ior <IOR>)",
                                  dadi::Message::PRIO_DEBUG));
      }
    } catch (...) {
        logger->log(dadi::Message("Fwdr",
                                  "Got an exception while retrieving IOR file",
                                  dadi::Message::PRIO_DEBUG));
    }
  }
  if (cfg.getPeerIOR() != "" && cfg.getPeerIOR().find("IOR:") != 0) {
    /* Extract the IOR from a file. */
    std::ifstream file(cfg.getPeerIOR().c_str());
    std::string peerIOR;
    std::string peerPort;
    if (!file.is_open()) {
      logger->log(dadi::Message("Fwdr",
                                "Error: Invalid peer-ior parameter",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
    file >> peerIOR;
    cfg.setPeerIOR(peerIOR);
    if (!file.eof() && (cfg.getRemotePortFrom() == "")) {
      file >> peerPort;
      cfg.setRemotePortFrom(peerPort);
    }
  }

  if (cfg.getPeerIOR() != "") {
    tunnel.setRemotePortTo(ORBMgr::getPort(cfg.getPeerIOR()));
  } else {
    tunnel.setRemotePortTo(cfg.getRemotePortTo());
  }
  if (cfg.getRemoteHost() == "auto") {
    if (cfg.getPeerIOR() != "") {
      tunnel.setRemoteHost(ORBMgr::getHost(cfg.getPeerIOR()));
    } else {
      tunnel.setRemoteHost("127.0.0.1");
    }
  } else {
    if (cfg.getRemoteHost() != "") {
      tunnel.setRemoteHost(cfg.getRemoteHost());
    } else {
      tunnel.setRemoteHost("localhost");
    }
  }

  tunnel.setRemotePortFrom(cfg.getRemotePortFrom());
  //    tunnel.setLocalPortFrom(cfg.getLocalPortFrom());
  if (cfg.createFrom()) {
    if (cfg.getRemotePortFrom() == "") {
      logger->log(dadi::Message("Fwdr",
                                "Failed to automatically determine a remote free port.\n" \
                                " You need to specify the remote port:\n" \
                                "\t - Remote port (--remote-port <port>)",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
  }


  tunnel.setLocalPortTo(ORBMgr::getPort(ior));

  if (cfg.getSshHost()!="") {
    tunnel.createTunnelTo(cfg.createTo());
    tunnel.createTunnelFrom(cfg.createFrom());
  }
  tunnel.open();

  /* Try to find the peer. */
  bool canLaunch = true;
  if (cfg.getPeerIOR()!="") {
    try {
      if (connectPeer(ior, cfg.getPeerIOR(),
                      "localhost", tunnel.getRemoteHost(),
                      tunnel.getLocalPortFrom(), tunnel.getRemotePortFrom(),
                      forwarder, mgr)) {
        /* In this case it seems that there is a problem with
         * the alias 'localhost', thus try to use 127.0.0.1
         */
        if (tunnel.getRemoteHost() == "localhost") {
          tunnel.setRemoteHost("127.0.0.1");
        }
        if (connectPeer(ior, cfg.getPeerIOR(),
                        "127.0.0.1", tunnel.getRemoteHost(),
                        tunnel.getLocalPortFrom(), tunnel.getRemotePortFrom(),
                        forwarder, mgr)) {
          logger->log(dadi::Message("Fwdr",
                                    "Unable to contact remote peer." \
                                    "Waiting for connection...\n",
                                    dadi::Message::PRIO_DEBUG));
        }
      }
    } catch (...) {
          logger->log(dadi::Message("Fwdr",
                                    "Error while connecting to remote peer\n",
                                    dadi::Message::PRIO_DEBUG));
      canLaunch = false;
    }
  }

  if (canLaunch) {
    try {
      ORBMgr::getMgr()->wait();
      mgr->unbind(FWRDCTXT, cfg.getName());
    } catch (...) {
      logger->log(dadi::Message("Fwdr",
                                "Error while exiting the ORBMgr::wait() function",
                               dadi::Message::PRIO_DEBUG));
    }
  }

  /* shutdown and destroy the ORB
   * Servants will be deactivated and deleted automatically
   */
  delete ORBMgr::getMgr();

    logger->log(dadi::Message("Fwdr",
                              "Forwarder is now terminated",
                               dadi::Message::PRIO_DEBUG));

  return EXIT_SUCCESS;
}

int
connectPeer(const std::string &ior, const std::string &peerIOR,
            const std::string &newHost, const std::string &remoteHost,
            int localPortFrom, int remotePortFrom,
            CorbaForwarder *forwarder, ORBMgr* mgr) {
  std::string newPeerIOR = ORBMgr::convertIOR(peerIOR, newHost, localPortFrom);
// Init logger
  dadi::LoggerPtr logger;
  dadi::ChannelPtr cc;
  logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  cc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(cc);

  Forwarder_var peer;

  peer = mgr->resolve<Forwarder, Forwarder_var>(newPeerIOR);

  try {
    peer->connectPeer(ior.c_str(), remoteHost.c_str(), remotePortFrom);
    forwarder->setPeer(peer);

    // Get the existing contexts except the Forwarders one
    std::list<std::string> contexts = mgr->contextList();
    contexts.remove(FWRDCTXT);
    // Get the other forwarder list
    std::list<std::string> fwds = forwarder->otherForwarders();
    std::string fwdName = forwarder->getName();
    std::string fwdTag = "@" + fwdName;

    // For each context
    for (std::list<std::string>::const_iterator it = contexts.begin();
         it != contexts.end(); ++it) {
      // Get the local objects
      std::list<std::string> objects = mgr->localObjects(*it);
      // Get the object from other forwarders
      for (std::list<std::string>::const_iterator jt = fwds.begin();
           jt != fwds.end(); ++jt) {
        std::list<std::string> fwdObjects = mgr->forwarderObjects(*jt, *it);
        objects.insert(objects.end(), fwdObjects.begin(), fwdObjects.end());
      }

      // Bind them on the peer
      for (std::list<std::string>::const_iterator jt = objects.begin();
           jt != objects.end(); ++jt) {
        std::string objName = *it +"/" + *jt;
        std::string ior = mgr->getIOR(*it, *jt);
        forwarder->bind(objName.c_str(), ior.c_str());
      }
      // Then, get the objects binded on the peer
      SeqString* remoteObjs = peer->getBindings(it->c_str());
      // And bind them locally and on others forwarders if they are not yet
      for (unsigned int i = 0; i < remoteObjs->length(); i += 2) {
        std::string name((*remoteObjs)[i]);
        std::string ior((*remoteObjs)[i + 1]);
        if (find(objects.begin(), objects.end(), name) != objects.end()) {
          continue;
        }
        std::string newIOR = ORBMgr::convertIOR(ior, fwdTag, 0);
        mgr->bind(*it, name, newIOR, true);
        mgr->fwdsBind(*it, name, newIOR, fwdName);
      }
    }
  } catch (CORBA::TRANSIENT& err) {
    logger->log(dadi::Message("Fwdr",
                              "Unable to contact remote peer using '" + newHost
                              + "' as a \"new remote host\"",
                              dadi::Message::PRIO_DEBUG));
    return EXIT_FAILURE;
  }

    logger->log(dadi::Message("Fwdr",
                              "Contacted remote peer using '"
                              + newHost + "' as new remote host\n",
                              dadi::Message::PRIO_DEBUG));
  return 0;
}


void
name(const std::string& name, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setName(name);
}

void
peer_name(const std::string& name, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setPeerName(name);
}

void
peer_ior(const std::string& ior, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setPeerIOR(ior);
}

void
ssh_host(const std::string& host, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshHost(host);
}

void
remote_host(const std::string& host, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemoteHost(host);
}

void
remote_port_to(const std::string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemotePortTo(port);
}

void
remote_port_from(const std::string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemotePortFrom(port);
}

void
local_port_from(const std::string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setLocalPortFrom(port);
}

void
ssh_path(const std::string& path, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshPath(path);
}

void
ssh_port(const std::string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshPort(port);
}

void
ssh_login(const std::string& login, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshLogin(login);
}

void
key_path(const std::string& path, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshKeyPath(path);
}

void
tunnel_wait(const std::string& time, Configuration* cfg) {
  std::istringstream is(time);
  int n;
  is >> n;
  static_cast<FwrdConfig*>(cfg)->setWaitingTime(n);
}

void
nb_retry(const std::string& nb, Configuration* cfg) {
  std::istringstream is(nb);
  int n;
  is >> n;
  static_cast<FwrdConfig*>(cfg)->setNbRetry(n);
}

/* Fwdr configuration implementation. */
FwrdConfig::FwrdConfig(const std::string& pgName) : Configuration(pgName) {
  mcreateTunnelTo = false;
  mcreateTunnelFrom = false;
  mnbRetry = 3;
  mwaitingTime = 0;
}

const std::string&
FwrdConfig::getName() const {
  return mname;
}
const std::string&
FwrdConfig::getPeerName() const {
  return mpeerName;
}

const std::string&
FwrdConfig::getPeerIOR() const {
  return mpeerIOR;
}

const std::string&
FwrdConfig::getSshHost() const {
  return msshHost;
}

const std::string&
FwrdConfig::getRemoteHost() const {
  return mremoteHost;
}

const std::string&
FwrdConfig::getRemotePortTo() const {
  return mremotePortTo;
}

const std::string&
FwrdConfig::getRemotePortFrom() const {
  return mremotePortFrom;
}

const std::string&
FwrdConfig::getLocalPortFrom() const {
  return mlocalPortFrom;
}

bool
FwrdConfig::createTo() const {
  return mcreateTunnelTo;
}

bool
FwrdConfig::createFrom() const {
  return mcreateTunnelFrom;
}

const std::string&
FwrdConfig::getSshPath() const {
  return msshPath;
}

const std::string&
FwrdConfig::getSshPort() const {
  return msshPort;
}
const std::string&
FwrdConfig::getSshLogin() const {
  return msshLogin;
}

const std::string&
FwrdConfig::getSshKeyPath() const {
  return msshKeyPath;
}

int
FwrdConfig::getNbRetry() const {
  return mnbRetry;
}

unsigned int
FwrdConfig::getWaitingTime() const {
  return mwaitingTime;
}

const std::string&
FwrdConfig::getCfgPath() const {
  return mcfgPath;
}

void
FwrdConfig::setName(const std::string& name) {
  this->mname = name;
}

void
FwrdConfig::setPeerName(const std::string& name) {
  this->mpeerName = name;
}

void
FwrdConfig::setPeerIOR(const std::string& ior) {
  this->mpeerIOR = ior;
}

void
FwrdConfig::setSshHost(const std::string& host) {
  this->msshHost = host;
}

void
FwrdConfig::setRemoteHost(const std::string& host) {
  this->mremoteHost = host;
}

void
FwrdConfig::setRemotePortTo(const std::string& port) {
  this->mremotePortTo = port;
}

void
FwrdConfig::setRemotePortFrom(const std::string& port) {
  this->mremotePortFrom = port;
}

void
FwrdConfig::setLocalPortFrom(const std::string& port) {
  this->mlocalPortFrom = port;
}

void
FwrdConfig::createTo(bool create) {
  this->mcreateTunnelTo = create;
}

void
FwrdConfig::createFrom(bool create) {
  this->mcreateTunnelFrom = create;
}

void
FwrdConfig::setSshPath(const std::string& path) {
  this->msshPath = path;
}

void
FwrdConfig::setSshPort(const std::string& port) {
  this->msshPort = port;
}

void
FwrdConfig::setSshLogin(const std::string& login) {
  this->msshLogin = login;
}

void
FwrdConfig::setSshKeyPath(const std::string& path) {
  this->msshKeyPath = path;
}

void
FwrdConfig::setNbRetry(const int nb) {
  this->mnbRetry = nb;
}

void
FwrdConfig::setWaitingTime(const unsigned int time) {
  this->mwaitingTime = time;
}

void
FwrdConfig::setCfgPath(const std::string& path) {
  this->mcfgPath = path;
}

int
change(int c) {
  if (c == '.') {
    return '-';
  }
  return c;
}

