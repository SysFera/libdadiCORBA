
#include "Forwarder.hh"
#include "ORBMgr.hh"
#include "SSHTunnel.hh"
#include "Options.hh"

#include "genericFwdr.hh"

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <algorithm>

#include <omniORB4/CORBA.h>

#include <unistd.h>	// For sleep function

using namespace std;

int main(int argc, char* argv[], char* envp[]) {
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

  /* Optionnal - default values are set to port 22,
   * current user login and $HOME/.ssh/id_[rsa|dsa].
   */
  opt.setOptCallback("--ssh-port", ssh_port);
  opt.setOptCallback("--ssh-login", ssh_login);
  opt.setOptCallback("--ssh-key", key_path);
	
  /* Optionnal parameters/flags. */
  opt.setOptCallback("--retry", nb_retry);
  opt.setOptCallback("--peer-ior", peer_ior);
  opt.setFlagCallback('C', create);
  //opt.setFlagCallback('f', create_from);
	
  opt.setOptCallback("--net-config", net_config);

  opt.processOptions();
	
  if (cfg.getName()=="") {
    std::ostringstream name;
    char host[256];
		
    gethostname(host, 256);
    host[255]='\0';
		
    std::transform(host, host+strlen(host), host, change);
    name << "Forwarder-" << host << "-" << getpid();
    cfg.setName(name.str());
  }
  if (cfg.getCfgPath()=="") {
  }
	  
  if (cfg.createFrom()) {
    if (cfg.getPeerName() == ""
	|| cfg.getSshHost() == "") {
    }
  }
  
	
  SSHTunnel tunnel;
  Forwarder* forwarder;
  try {
    forwarder = new Forwarder(cfg.getName());
  } catch (exception &e) {
  }
  ORBMgr::init(argc, argv);
  ORBMgr* mgr = ORBMgr::getMgr();
  string ior;
  int count = 0;
	
  mgr->activate(forwarder);
  do {
    try {
      mgr->bind(FWRDCTXT, cfg.getName(), forwarder->_this(), "log", true);
      break;
    } catch (CORBA::TRANSIENT& err) {
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
  string iorFilename("/tmp/LOG-forwarder-ior-");
  iorFilename+=cfg.getName()+".tmp";
  ofstream of(iorFilename.c_str(), ios_base::trunc);
	
  if (!of.is_open()) {
  } else {	
    if (cfg.createFrom()) { // Creating tunnel(s)
      istringstream is(cfg.getRemotePortFrom());
      int port;
			
      is >> port;
      of << ORBMgr::convertIOR(ior, cfg.getRemoteHost(), port);
    }	else {// Waiting for connexion.
      of << ior;
    }
    of << std::endl;
    of << freeTCPport(); // also write a free port
    of.close();
  }
	
  tunnel.setSshHost(cfg.getSshHost());
  tunnel.setRemoteHost(cfg.getRemoteHost());
		
  tunnel.setSshPath(cfg.getSshPath());
  tunnel.setSshPort(cfg.getSshPort());
  tunnel.setSshLogin(cfg.getSshLogin());
  tunnel.setSshKeyPath(cfg.getSshKeyPath());
	
  /* Manage the peer IOR. */
  if (cfg.getPeerIOR()=="" && cfg.createFrom()) {
    /* Try to retrieve the peer IOR. */
    SSHCopy copy(cfg.getSshHost(),
		 "/tmp/LOG-forwarder-ior-"+cfg.getPeerName()+".tmp",
		 "/tmp/LOG-forwarder-ior-"+cfg.getPeerName()+".tmp");
    copy.setSshPath("/usr/bin/scp");
    copy.setSshPort(cfg.getSshPort());
    copy.setSshLogin(cfg.getSshLogin());
    copy.setSshKeyPath(cfg.getSshKeyPath());
    try {
      if (copy.getFile()) {
        cfg.setPeerIOR("/tmp/LOG-forwarder-ior-"+cfg.getPeerName()+".tmp");
      } else {
      }
    } catch (...) {
    }
  }
  if (cfg.getPeerIOR()!="" && cfg.getPeerIOR().find("IOR:")!=0) {
    /* Extract the IOR from a file. */
    ifstream file(cfg.getPeerIOR().c_str());
    string peerIOR;
    string peerPort;
    if (!file.is_open()) {
    }
    file >> peerIOR;
    cfg.setPeerIOR(peerIOR);
    if (!file.eof() && cfg.getRemotePortFrom() == "") {
      file >> peerPort;
      cfg.setRemotePortFrom(peerPort);
    }
  }
	
  if (cfg.getPeerIOR() != "") {
    tunnel.setRemotePortTo(ORBMgr::getPort(cfg.getPeerIOR()));
  } else {
    tunnel.setRemotePortTo(cfg.getRemotePortTo());
  }
  if (cfg.getRemoteHost() == "") {
    if (cfg.getPeerIOR() != "") {
      tunnel.setRemoteHost(ORBMgr::getHost(cfg.getPeerIOR()));
    } else {
      tunnel.setRemoteHost("127.0.0.1");
    }
  } else {
    tunnel.setRemoteHost(cfg.getRemoteHost());
  }

  tunnel.setRemotePortFrom(cfg.getRemotePortFrom());
  //	tunnel.setLocalPortFrom(cfg.getLocalPortFrom());
  if (cfg.createFrom()) {
    if (cfg.getRemotePortFrom() == "") {
    }
  }

	
  tunnel.setLocalPortTo(ORBMgr::getPort(ior));

  tunnel.createTunnelTo(cfg.createTo());
  tunnel.createTunnelFrom(cfg.createFrom());
	
  tunnel.open();

  /* Try to find the peer. */
  bool canLaunch = true;
  if (cfg.getPeerIOR()!="") {
    try {
      if (connectPeer(ior, cfg.getPeerIOR(), "localhost", tunnel.getRemoteHost(),
                      tunnel.getLocalPortFrom(), tunnel.getRemotePortFrom(), forwarder, mgr)) {
        /* In this case it seems that there is a problem with the alias 'localhost', thus we
         * try to use 127.0.0.1
         */
        if (tunnel.getRemoteHost() == "localhost") {
          tunnel.setRemoteHost("127.0.0.1");
        }
        if (connectPeer(ior, cfg.getPeerIOR(), "127.0.0.1", tunnel.getRemoteHost(),
                        tunnel.getLocalPortFrom(), tunnel.getRemotePortFrom(), forwarder, mgr)) {
        }
      }
    } catch (...) {
      canLaunch = false;
    }
  }

  if (canLaunch) {
    mgr->wait();
  }

  return EXIT_SUCCESS;
}

int
connectPeer(const std::string &ior, const std::string &peerIOR,
            const std::string &newHost, const std::string &remoteHost,
            int localPortFrom, int remotePortFrom, Forwarder *forwarder, ORBMgr* mgr) {
  
  std::string newPeerIOR = ORBMgr::convertIOR(peerIOR, newHost, localPortFrom);

  CorbaForwarder_var peer;
		
  peer = mgr->resolve<CorbaForwarder, CorbaForwarder_var>(newPeerIOR);
		
  try {
    peer->connectPeer(ior.c_str(), remoteHost.c_str(), remotePortFrom);
    forwarder->setPeer(peer);
  } catch (CORBA::TRANSIENT& err) {
  }

  return 0;
}


void name(const string& name, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setName(name);
}

void peer_name(const string& name, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setPeerName(name);
}

void peer_ior(const string& ior, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setPeerIOR(ior);
}

void net_config(const string& path, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setCfgPath(path);
}

void ssh_host(const string& host, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshHost(host);
}

void remote_host(const string& host, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemoteHost(host);
}

void remote_port_to(const string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemotePortTo(port);
}

void remote_port_from(const string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setRemotePortFrom(port);
}

void local_port_from(const string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setLocalPortFrom(port);
}

void ssh_path(const string& path, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshPath(path);
}

void ssh_port(const string& port, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshPort(port);
}

void ssh_login(const string& login, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshLogin(login);
}

void key_path(const string& path, Configuration* cfg) {
  static_cast<FwrdConfig*>(cfg)->setSshKeyPath(path);
}

void tunnel_wait(const string& time, Configuration* cfg) {
  istringstream is(time);
  int n;
  is >> n;
  static_cast<FwrdConfig*>(cfg)->setWaitingTime(n);
}

void create(const string& create, Configuration* cfg) {
  (void) create;
  static_cast<FwrdConfig*>(cfg)->createTo(true);
  static_cast<FwrdConfig*>(cfg)->createFrom(true);
}

void nb_retry(const string& nb, Configuration* cfg) {
  istringstream is(nb);
  int n;
  is >> n;
  static_cast<FwrdConfig*>(cfg)->setNbRetry(n);
}
/* Fwdr configuration implementation. */
FwrdConfig::FwrdConfig(const string& pgName) : Configuration(pgName)
{
  createTunnelTo = false;
  createTunnelFrom = false;
  nbRetry = 3;
  waitingTime = 0;
}

const string& FwrdConfig::getName() const {
  return name;
}
const string& FwrdConfig::getPeerName() const {
  return peerName;
}
const string& FwrdConfig::getPeerIOR() const {
  return peerIOR;
}

const string& FwrdConfig::getSshHost() const {
  return sshHost;
}
const string& FwrdConfig::getRemoteHost() const {
  return remoteHost;
}
const string& FwrdConfig::getRemotePortTo() const {
  return remotePortTo;
}
const string& FwrdConfig::getRemotePortFrom() const {
  return remotePortFrom;
}
const string& FwrdConfig::getLocalPortFrom() const {
  return localPortFrom;
}
bool  FwrdConfig::createTo() const {
  return createTunnelTo;
}
bool  FwrdConfig::createFrom() const {
  return createTunnelFrom;
}
const string& FwrdConfig::getSshPath() const {
  return sshPath;
}
const string& FwrdConfig::getSshPort() const {
  return sshPort;
}
const string& FwrdConfig::getSshLogin() const {
  return sshLogin;
}
const string& FwrdConfig::getSshKeyPath() const {
  return sshKeyPath;
}
int  FwrdConfig::getNbRetry() const {
  return nbRetry;
}
unsigned int  FwrdConfig::getWaitingTime() const {
  return waitingTime;
}
const string& FwrdConfig::getCfgPath() const {
  return cfgPath;
}

void FwrdConfig::setName(const string& name) {
  this->name = name;
}
void FwrdConfig::setPeerName(const string& name) {
  this->peerName = name;
}
void FwrdConfig::setPeerIOR(const string& ior) {
  this->peerIOR = ior;
}

void FwrdConfig::setSshHost(const string& host) {
  this->sshHost = host;
}
void FwrdConfig::setRemoteHost(const string& host) {
  this->remoteHost = host;
}
void FwrdConfig::setRemotePortTo(const string& port) {
  this->remotePortTo = port;
}
void FwrdConfig::setRemotePortFrom(const string& port) {
  this->remotePortFrom = port;
}
void FwrdConfig::setLocalPortFrom(const string& port) {
  this->localPortFrom = port;
}
void FwrdConfig::createTo(bool create) {
  this->createTunnelTo = create;
}
void FwrdConfig::createFrom(bool create) {
  this->createTunnelFrom = create;
}
void FwrdConfig::setSshPath(const string& path) {
  this->sshPath = path;
}
void FwrdConfig::setSshPort(const string& port) {
  this->sshPort = port;
}
void FwrdConfig::setSshLogin(const string& login) {
  this->sshLogin = login;
}
void FwrdConfig::setSshKeyPath(const string& path) {
  this->sshKeyPath = path;
}
void FwrdConfig::setNbRetry(const int nb) {
  this->nbRetry = nb;
}
void FwrdConfig::setWaitingTime(const unsigned int time) {
  this->waitingTime = time;
}
void FwrdConfig::setCfgPath(const string& path) {
  this->cfgPath = path;
}

int change(int c) {
  if (c == '.') {
    return '-';
  }
  return c;
}

