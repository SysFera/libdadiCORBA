/**
 * @file SSHTunnel.hh
 *
 * @brief  DIET forwarder implementation - SSH Tunnel implementation
 *
 * @author  Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *
 * @section Licence
 *   |LICENCE|
 */

#ifndef SSHTUNNEL_HH
#define SSHTUNNEL_HH

#include <csignal>
#include <string>
#include <unistd.h>

#include "dadi/Logging/Logger.hh"

/**
 * @brief The generic class to handle ssh connection
 * @class SSHConnection
 */
class SSHConnection {
public:
/**
 * @brief Constructor
 */
  SSHConnection();
/**
 * @brief Destructor
 */
  ~SSHConnection();

/**
 * @brief Constructor
 * @param sshHost The ssh host
 * @param sshPort The port for the ssh tunnel
 * @param login The login to establish the connection
 * @param keyPath The path to the private ssh key
 * @param sshPath The path to the ssh command
 */
  SSHConnection(const std::string & sshHost, const std::string & sshPort,
                const std::string & login, const std::string & keyPath,
                const std::string & sshPath);

/**
 * @brief To get the ssh host
 * @return The ssh host
 */
  const std::string &
  getSshHost() const;

/**
 * @brief To get the ssh command path
 * @return The ssh command path
 */
  const std::string &
  getSshPath() const;

/**
 * @brief To get the ssh port
 * @return The ssh port
 */
  const std::string &
  getSshPort() const;

/**
 * @brief To get the ssh login
 * @return The ssh login
 */
  const std::string &
  getSshLogin() const;

/**
 * @brief To get the path to the private ssh key
 * @return The ssh key path
 */
  const std::string &
  getSshKeyPath() const;

/**
 * @brief To get the ssh options
 * @return The ssh options
 */
  const std::string &
  getSshOptions() const;

/**
 * @brief To set the ssh host
 * @param host The ssh host
 */
  void
  setSshHost(const std::string &host);

/**
 * @brief To set the ssh command path
 * @param path The ssh command path
 */
  void
  setSshPath(const std::string &path);

/**
 * @brief To set the ssh port
 * @param port The ssh port
 */
  void
  setSshPort(const std::string &port);

/**
 * @brief To set the ssh port
 * @param port The ssh port
 */
  void
  setSshPort(const int port);

/**
 * @brief To set the ssh login
 * @param login The login
 */
  void
  setSshLogin(const std::string &login);

/**
 * @brief To set the path to the private shh key
 * @param path The path to the ssh key
 */
  void
  setSshKeyPath(const std::string &path);

/**
 * @brief To set the ssh options
 * @param options THe ssh options
 */
  void
  setSshOptions(const std::string &options);

protected:
/**
 * @brief Logger
 */
  dadi::LoggerPtr mlogger;
/**
 * Channel for logger
 */
  dadi::ChannelPtr mcc;

  /**
   * @brief Get the default user login.
   */
  static std::string
  userLogin();

  /**
   * @brief Get the default user private key.
   */
  static std::string
  userKey();


private:
  /**
   * @brief SSH executable path.
   */
  std::string msshPath;
  /**
   * @brief SSH login.
   */
  std::string mlogin;
  /**
   * @brief SSH key path.
   */
  std::string mkeyPath;
  /**
   * @brief SSH host.
   */
  std::string msshHost;
  /**
   * @brief SSH port.
   */
  std::string msshPort;
  /**
   * @brief SSH options.
   */
  std::string moptions;


};

/**
 * @brief The generic class to handle ssh tunnels
 * @class SSHTunnel
 */
class SSHTunnel : public SSHConnection {
public:
/**
 * @brief Constructor
 */
  SSHTunnel();
  /**
   * @brief Constructor for bi-directionnal SSH tunnel.
   * @param sshHost The ssh host
   * @param remoteHost The remote host
   * @param localPortFrom The local port in receiption
   * @param remotePortTo The remote port in receiption
   * @param remotePortFrom The remote port in emission
   * @param localPortTo The local port in emission
   * @param createTo To create a tunnel from local to remote
   * @param createFrom To create a tunnel from remote to local
   * @param sshPath The ssh command path
   * @param sshPort The ssh port
   * @param login The remote user login
   * @param keyPath The ssh key path
   */
  SSHTunnel(const std::string & sshHost,
            const std::string & remoteHost,
            const std::string & localPortFrom,
            const std::string & remotePortTo,
            const std::string & remotePortFrom,
            const std::string & localPortTo,
            const bool createTo = true,
            const bool createFrom = true,
            const std::string &sshPath = "/usr/bin/ssh",
            const std::string &sshPort = "22",
            const std::string &login = userLogin(),
            const std::string &keyPath = userKey());

  /**
   * @brief Constructor for unidirectionnal SSH tunnel.
   * @param sshHost The ssh host
   * @param remoteHost The remote host
   * @param localPortFrom The local port in receiption
   * @param remotePortTo The remote port in receiption
   * @param createTo To create a tunnel from local to remote
   * @param sshPath The ssh command path
   * @param sshPort The ssh port
   * @param login The remote user login
   * @param keyPath The ssh key path
   */
  SSHTunnel(const std::string & sshHost,
            const std::string & remoteHost,
            const std::string & localPortFrom,
            const std::string & remotePortTo,
            const bool createTo = true,
            const std::string &sshPath = "/usr/bin/ssh",
            const std::string &serverPort = "22",
            const std::string &login = userLogin(),
            const std::string &keyPath = userKey());

  /**
   * @brief Destructor
   */
  ~SSHTunnel();

/**
 * @brief To open the ssh connection
 */
  void
  open();
/**
 * @brief To close the ssh connection
 */
  void
  close();

/**
 * @brief To get the remote host
 * @return The remote host
 */
  const std::string &
  getRemoteHost() const;

/**
 * @brief To get the local port from
 * @return The local port from
 */
  int
  getLocalPortFrom() const;

/**
 * @brief To get the local port to
 * @return The local port to
 */
  int
  getLocalPortTo() const;

/**
 * @brief To get the remote port from
 * @return The remote port from
 */
  int
  getRemotePortFrom() const;

/**
 * @brief To get the remote port to
 * @return The remote port to
 */
  int
  getRemotePortTo() const;

/**
 * @brief To set the remote host
 * @param host The remote host
 */
  void
  setRemoteHost(const std::string &host);

/**
 * @brief To set the local port from
 * @param port The local port from
 */
  void
  setLocalPortFrom(const std::string &port);

/**
 * @brief To set the remote port from
 * @param port The local port from
 */
  void
  setLocalPortFrom(const int port);

/**
 * @brief To set the remote port to
 * @param port The remote port to
 */
  void
  setRemotePortTo(const std::string &port);

/**
 * @brief To set the remote port to
 * @param port The remote port to
 */
  void
  setRemotePortTo(const int port);

/**
 * @brief To set the remote port from
 * @param port The remote port from
 */
  void
  setRemotePortFrom(const std::string &port);

/**
 * @brief To set the remote port from
 * @param port The remote port from
 */
  void
  setRemotePortFrom(const int port);

/**
 * @brief To set the local port to
 * @param port The local port to
 */
  void
  setLocalPortTo(const std::string &port);

/**
 * @brief To set the local port to
 * @param port The local port to
 */
  void
  setLocalPortTo(const int port);

/**
 * @brief To set the waiting time
 * @param time The waiting time
 */
  void
  setWaitingTime(const unsigned int time);

/**
 * @brief To set if create a tunnel from local to remote
 * @param create if create a tunnel from local to remote
 */
  void
  createTunnelTo(const bool create);

/**
 * @brief To set if create a tunnel from remote to local
 * @param create if create a tunnel from remote to local
 */
  void
  createTunnelFrom(const bool create);
private:
  /* Format strings for ssh commands. */
/**
 * @brief Command format
 */
  static std::string mcmdFormat;
/**
 * @brief Command default format
 */
  static std::string mcmdFormatDefault;
/**
 * @brief Command local format
 */
  static std::string mlocalFormat;
/**
 * @brief Command remote format
 */
  static std::string mremoteFormat;
/**
 * @brief Command for ssh private key format
 */
  static std::string mkeyFormat;
  /* Tunnel configuration. */
/**
 * @brief if create a tunnel from remote to local
 */
  bool mcreateFrom;
/**
 * @brief if create a tunnel from local to remote
 */
  bool mcreateTo;
/**
 * @brief The timeout to wait for ssh establishment
 */
  unsigned int mwaitingTime;
/**
 * @brief The local port in emission
 */
  std::string mlocalPortTo;
/**
 * @brief The local port in reception
 */
  std::string mlocalPortFrom;
/**
 * @brief The remote host
 */
  std::string mremoteHost;
/**
 * @brief The remote port in reception
 */
  std::string mremotePortTo;
/**
 * @brief The remote port in emission
 */
  std::string mremotePortFrom;

  /**
   * @brief Process pid.
   */
  pid_t mpid;

  /**
   * @brief Create the right ssh command
   * @return The ssh command
   */
  std::string
  makeCmd();
};

/* Copy a file using scp. */
/**
 * @brief The generic class to handle scp
 * @class SSHCopy
 */
class SSHCopy : public SSHConnection {
public:
/**
 * @brief Constructor
 */
  SSHCopy(const std::string & sshHost,
          const std::string & remoteFilename,
          const std::string & localFilename);

/**
 * @brief To get a file
 * @return true if success
 */
  bool
  getFile() const;

/**
 * @brief To put a file
 * @return true if success
 */
  bool
  putFile() const;

private:
/**
 * @brief The remote name of the file
 */
  std::string mremoteFilename;
/**
 * @brief The local name of the file
 */
  std::string mlocalFilename;

  /**
   * @brief Process pid.
   */
  mutable pid_t mpid;
};

/**
 * @brief To free the TCP ports
 */
std::string
freeTCPport();

#endif /* ifndef SSHTUNNEL_HH */
