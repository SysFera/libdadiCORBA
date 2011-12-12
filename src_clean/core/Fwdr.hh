/**
* @file Fwdr.hh
*
* @brief   DIET forwarder implementation - Forwarder executable
*
* @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
*
* @section License
*   |LICENSE|
*/

#ifndef _DIETFWDR_HH_
#define _DIETFWDR_HH_


#include <string>
#include "utils/Options.hh"

/**
 * @brief Class that handle the configuration for the forwarders
 * @class FwrdConfig
 */
class FwrdConfig : public Configuration {
public:
/**
 * @brief Constructor
 */
  FwrdConfig();

/**
 * @brief To get the name
 */
  const std::string&
  getName() const;
/**
 * @brief To get the peer name
 */
  const std::string&
  getPeerName() const;
/**
 * @brief To get the IOR
 */
  const std::string&
  getPeerIOR() const;
/**
 * @brief To get the ssh host
 */
  const std::string&
  getSshHost() const;
/**
 * @brief To get the remote hostname
 */
  const std::string&
  getRemoteHost() const;
/**
 * @brief To get the remote port to where sending
 */
  const std::string&
  getRemotePortTo() const;
/**
 * @brief To get the remote port from where receiving
 */
  const std::string&
  getRemotePortFrom() const;

  const std::string&
  getLocalPortFrom() const;

  bool
  createTo() const;

  bool
  createFrom() const;

  const std::string&
  getSshPath() const;

  const std::string&
  getSshPort() const;

  const std::string&
  getSshLogin() const;

  const std::string&
  getSshKeyPath() const;

  int
  getNbRetry() const;

  unsigned int
  getWaitingTime() const;

  const std::string&
  getCfgPath() const;

  void
  setName(const std::string& name);

  void
  setPeerName(const std::string& name);

  void
  setPeerIOR(const std::string& ior);

  void
  setSshHost(const std::string& host);

  void
  setRemoteHost(const std::string& host);

  void
  setRemotePortTo(const std::string& port);

  void
  setRemotePortFrom(const std::string& port);

  void
  setLocalPortFrom(const std::string& port);

  void
  createTo(bool create);

  void
  createFrom(bool create);

  void
  setSshPath(const std::string& path);


  void
  setSshPort(const std::string& port);

  void
  setSshLogin(const std::string& login);

  void
  setSshKeyPath(const std::string& path);

  void
  setNbRetry(const int nb);

  void
  setWaitingTime(const unsigned int time);

  void
  setCfgPath(const std::string& path);

private:
  std::string mname;
  std::string mpeerName;
  std::string mpeerHost;
  std::string mpeerPort;
  std::string mpeerIOR;

  std::string msshHost;
  std::string mremoteHost;
  std::string mlocalPortFrom;
  std::string mremotePortTo;
  std::string mremotePortFrom;
  bool mcreateTunnelTo;
  bool mcreateTunnelFrom;
  std::string msshPath;
  std::string msshPort;
  std::string msshLogin;
  std::string msshKeyPath;
  int mnbRetry;
  std::string mcfgPath;
  unsigned int mwaitingTime;
};

int
connectPeer(const std::string &ior, const std::string &peerIOR,
            const std::string &newHost, const std::string &remoteHost,
            int localPortFrom, int remotePortFrom,
            CorbaForwarder *forwarder, ORBMgr* mgr);

void
name(const std::string& name, Configuration* cfg);

void
peer_name(const std::string& name, Configuration* cfg);

void
peer_ior(const std::string& ior, Configuration* cfg);

void
ssh_host(const std::string& host, Configuration* cfg);

void
remote_host(const std::string& host, Configuration* cfg);


void
remote_port_to(const std::string& port, Configuration* cfg);


void
remote_port_from(const std::string& port, Configuration* cfg);


void
local_port_from(const std::string& port, Configuration* cfg);


void
create(const std::string& create, Configuration* cfg);


void
ssh_path(const std::string& path, Configuration* cfg);


void
ssh_port(const std::string& port, Configuration* cfg);


void
ssh_login(const std::string& login, Configuration* cfg);


void
key_path(const std::string& path, Configuration* cfg);


void
nb_retry(const std::string& nb, Configuration* cfg);


void
tunnel_wait(const std::string& time, Configuration* cfg);

/* Transformation function for the host name. */
int
change(int c);

#endif  // _DIETFWDR_HH_