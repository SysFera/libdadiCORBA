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
#include "dadi/Config.hh"
#include "dadi/Options.hh"

#define NBRETRY 5

int
change(int c) {
  if (c == '.') {
    return '-';
  }
  return c;
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




int
main(int argc, char* argv[], char* envp[]) {
// Init logger
  dadi::LoggerPtr logger;
  dadi::ChannelPtr cc;
  logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  cc = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(cc);

  dadi::Options opt;
  dadi::Config& config = dadi::Config::instance();
  std::string usa = "\n [-h] --name <name> \n or the second: \n [-h] [options] --peer-name <peer-name> --ssh-host <host> --remote-host <localhost>";

  opt.setName("Fwdr");
  opt.setUsage(usa);
  boost::function0<void> fHelp(boost::bind(dadi::help, boost::cref(opt)));
  boost::function1<void, std::string> fName(
    boost::bind(dadi::setPropertyString, "name", _1));
  boost::function1<void, std::string> fpeern(
    boost::bind(dadi::setPropertyString, "peer-name", _1));
  boost::function1<void, std::string> fsshh(
    boost::bind(dadi::setPropertyString, "ssh-host", _1));
  boost::function1<void, std::string> fremoteh(
    boost::bind(dadi::setPropertyString, "remote-host", _1));
  boost::function1<void, std::string> fsshl(
    boost::bind(dadi::setPropertyString, "ssh-login", _1));
  boost::function1<void, std::string> fremotep(
    boost::bind(dadi::setPropertyString, "remote-port", _1));
  boost::function1<void, std::string> fsshp(
    boost::bind(dadi::setPropertyString, "ssh-port", _1));
  boost::function1<void, std::string> fpior(
    boost::bind(dadi::setPropertyString, "peer-ior", _1));
  boost::function1<void, std::string> fret(
    boost::bind(dadi::setPropertyString, "nb-retry", _1));


  opt.addSwitch("help,h", "display help message", fHelp);
  opt.addOption("name,n", "the name of the forwarder", fName, true)->default_value("");
  opt.addOption("peer-name,p", "the name of the peer", fpeern)->default_value("");
  opt.addOption("ssh-host,s", "the ssh host", fsshh)->default_value("");
  opt.addOption("ssh-port,m", "the ssh port", fsshp)->default_value("");
  opt.addOption("remote-host,r", "should be localhost", fremoteh)->default_value("");
  opt.addOption("ssh-login,l", "if the ssh login is different on the other machine to change it", fsshl)->default_value("");
  opt.addOption("remote-port,z", "to use a specific port", fremotep)->default_value("");
  opt.addOption("peer-ior,i", "to use a specific ior for the peer", fpior)->default_value("");
  opt.addOption("nb-retry,a", "the number of time to retry again", fret)->default_value("");

  std::cout << "PARSING " << std::endl;
  opt.parseCommandLine(argc, argv);
  std::cout << "PARSED " << std::endl;
  opt.notify();
  std::cout << "NOTIFIED " << std::endl;
  bool createTo = false;
  bool createFrom = false;

  if(config.get<std::string>("name")=="") {
    std::cout << "Missing Parameter. Usage: " << std::endl << "\n [-h] --name <name> \n or the second: \n [-h] [options] --peer-name <peer-name> --ssh-host <host> --remote-host <localhost>" << std::endl;
    return 0;
  }


  if(config.get<std::string>("ssh-host")!="") {
    createTo = true;
    createFrom = true;
  }
  std::cout << "NOTIFIED2 " << std::endl;


//  opt.processOptions();
//  if (config.getSshHost() != "") {
//    config.createTo(true);
//    config.createFrom(true);
//  }


//  if (!config.get_optional<std::string>("name") config.get<std::string>("name") == "") {
//    std::cout << "NOTIFIED3 " << std::endl;
//    std::ostringstream name;
//    char host[256];
//
//    gethostname(host, 256);
//    host[255]='\0';
//
//    std::transform(host, host+strlen(host), host, change);
//    name << "Forwarder-" << host << "-" << getpid();
//
//    logger->log(dadi::Message("Fwdr",
//                              "Missing parameter: name (use --name to fix it) \n Use default name: " + name.str() + "\n",
//                               dadi::Message::PRIO_DEBUG));

//    config.setName(name.str());
//  }

  if (createFrom) {
    std::cout << "NOTIFIED3 " << std::endl;
    if (config.get<std::string>("peer-name")==""
        || config.get<std::string>("ssh-host")=="") {
      logger->log(dadi::Message("Fwdr",
                                "Missing parameter(s) to create tunnel. Mandatory parameters: \n \t - Peer name (--peer-name <name>) \n \t - SSH host (--ssh-host <host>)",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
  }

  std::cout << "NOTIFIED4 " << std::endl;
  SSHTunnel tunnel;
  CorbaForwarder* forwarder;
  try {
    std::cout << "NOTIFIED5 " << std::endl;
    forwarder = new CorbaForwarder(config.get<std::string>("name"));
    std::cout << "NOTIFIED6 " << std::endl;
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
      std::cout << "NOTIFIED7 " << std::endl;
      mgr->bind(FWRDCTXT, config.get<std::string>("name"), forwarder->_this(), true);
      break;
    } catch (CORBA::TRANSIENT& err) {
      logger->log(dadi::Message("Fwdr",
                                "Error when binding the forwarder "
                                + config.get<std::string>("name") + " \n",
                                dadi::Message::PRIO_DEBUG));
      if (count++<NBRETRY) {
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
  iorFilename += config.get<std::string>("name") + ".tmp";
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
    if (createFrom) {  // Creating tunnel(s)
      std::string tmp;
      std::cout << "NOTIFIED8 " << std::endl;
//      if (config.get<std::string>("remote-port")!="") {
        tmp = config.get<std::string>("remote-port");
//      } else {
//        tmp = "";
//      }
      std::istringstream is(tmp);

      int port;

      is >> port;
      std::cout << "NOTIFIED9 " << std::endl;
//      if (config.get_optional<std::string>("remote-host")) {
        tmp = config.get<std::string>("remote-host");
//      } else {
//        tmp = "";
//      }
      of << ORBMgr::convertIOR(ior, "", port);
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

  std::cout << "NOTIFIED10 " << std::endl;
  tunnel.setSshHost(config.get<std::string>("ssh-host"));
  tunnel.setRemoteHost(config.get<std::string>("remote-host"));
  std::cout << "NOTIFIED11 " << std::endl;

//  tunnel.setSshPath(config.get<std::string>SshPath());
  tunnel.setSshPort(config.get<std::string>("remote-port"));
  tunnel.setSshLogin(config.get<std::string>("ssh-login"));
//  tunnel.setSshKeyPath(config.get<std::string>SshKeyPath());

//  tunnel.setWaitingTime(config.getWaitingTime());

  /* Manage the peer IOR. */
//  if (config.getPeerIOR().empty() && createFrom) {
  std::cout << "NOTIFIED12 " << std::endl;
  if (createFrom) {
    /* Try to retrieve the peer IOR. */
    SSHCopy copy(config.get<std::string>("ssh-host"),
                 "/tmp/DIET-forwarder-ior-" + config.get<std::string>("peer-name") + ".tmp",
                 "/tmp/DIET-forwarder-ior-" + config.get<std::string>("peer-name") + ".tmp");
    std::cout << "NOTIFIED13 " << std::endl;
    copy.setSshPath("/usr/bin/scp");
    std::cout << "NOTIFIED13-1 " << std::endl;
    copy.setSshPort(config.get<std::string>("ssh-port"));
    std::cout << "NOTIFIED13-2 " << std::endl;
    copy.setSshLogin(config.get<std::string>("ssh-login"));
    std::cout << "NOTIFIED14 " << std::endl;
//    copy.setSshKeyPath(config.getSshKeyPath());
    copy.setSshKeyPath("");
    try {
      if (copy.getFile()) {
        logger->log(dadi::Message("Fwdr",
                                  "Got remote IOR file",
                                  dadi::Message::PRIO_DEBUG));
//        config.setPeerIOR("/tmp/DIET-forwarder-ior-" + config.getPeerName() + ".tmp");
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
  std::cout << "NOTIFIED15 " << std::endl;
  if (config.get<std::string>("peer-ior").find("IOR:") == 0) {
    std::cout << "NOTIFIED16 " << std::endl;
    /* Extract the IOR from a file. */
    std::ifstream file(config.get<std::string>("peer-ior").c_str());
    std::string peerIOR;
    std::string peerPort;
    if (!file.is_open()) {
      logger->log(dadi::Message("Fwdr",
                                "Error: Invalid peer-ior parameter",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
    file >> peerIOR;
    config.put<std::string>("peer-ior",peerIOR);
    if (!file.eof() && (config.get<std::string>("remote-port") == "")) {
      file >> peerPort;
      config.put<std::string>("remote-port", peerPort);
    }
  }

  if (config.get<std::string>("peer-ior") != "") {
    tunnel.setRemotePortTo(ORBMgr::getPort(config.get<std::string>("peer-ior")));
  } else {
    tunnel.setRemotePortTo(config.get<std::string>("peer-ior"));
  }
  if (config.get<std::string>("remote-host") == "auto") {
    if (config.get<std::string>("peer-ior") != "") {
      tunnel.setRemoteHost(ORBMgr::getHost(config.get<std::string>("peer-ior")));
    } else {
      tunnel.setRemoteHost("127.0.0.1");
    }
  } else {
    if (config.get<std::string>("remote-host") != "") {
      tunnel.setRemoteHost(config.get<std::string>("remote-host"));
    } else {
      tunnel.setRemoteHost("localhost");
    }
  }

  tunnel.setRemotePortFrom(config.get<std::string>("remote-port"));
  //    tunnel.setLocalPortFrom(config.getLocalPortFrom());
  if (createFrom) {
    if (config.get<std::string>("remote-port") == "") {
      logger->log(dadi::Message("Fwdr",
                                "Failed to automatically determine a remote free port.\n" \
                                " You need to specify the remote port:\n" \
                                "\t - Remote port (--remote-port <port>)",
                                dadi::Message::PRIO_DEBUG));
      return EXIT_FAILURE;
    }
  }


  tunnel.setLocalPortTo(ORBMgr::getPort(ior));

  if (config.get<std::string>("ssh-host")!="") {
    tunnel.createTunnelTo(createTo);
    tunnel.createTunnelFrom(createFrom);
  }
  tunnel.open();

  /* Try to find the peer. */
  bool canLaunch = true;
  if (config.get<std::string>("peer-ior")!="") {
    try {
      if (connectPeer(ior, config.get<std::string>("peer-ior"),
                      "localhost", tunnel.getRemoteHost(),
                      tunnel.getLocalPortFrom(), tunnel.getRemotePortFrom(),
                      forwarder, mgr)) {
        /* In this case it seems that there is a problem with
         * the alias 'localhost', thus try to use 127.0.0.1
         */
        if (tunnel.getRemoteHost() == "localhost") {
          tunnel.setRemoteHost("127.0.0.1");
        }
        if (connectPeer(ior, config.get<std::string>("peer-ior"),
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
      mgr->unbind(FWRDCTXT, config.get<std::string>("name"));
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
