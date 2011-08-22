/****************************************************************************/
/* log forwarder implementation - Network configuration                     */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Gael Le Mahec      (gael.le.mahec@ens-lyon.fr)                      */
/*    - Benjamin Depardon  (benjamin.depardon@sysfera.com)                  */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/

#ifndef NETCONFIG_HH
#define NETCONFIG_HH

#include <string>
#include <map>
#include <list>

class NetConfig {
private:
  std::string filePath;
  std::list<std::string> accept;
  std::list<std::string> reject;
  void parseFile();
  void reset();
  std::string myHostname;
  void addLocalHost(std::list<std::string>& l) const;
  void addNetwork(const std::string & key, const std::string & line, bool accepted);

public:
  NetConfig();
  NetConfig(const std::list<std::string>& accept,
            const std::list<std::string>& reject,
            const std::string& hostname);
  NetConfig(const std::string& path);
  NetConfig(const NetConfig& cfg);
	
  NetConfig& operator=(const NetConfig& cfg);
	
  void addAcceptNetwork(const std::string& pattern);
  void remAcceptNetwork(const std::string& pattern);
	
  void addRejectNetwork(const std::string& pattern);
  void remRejectNetwork(const std::string& pattern);
	
  bool manage(const std::string& hostname) const;
  void updateConfig();
	
  const std::list<std::string>& getAcceptList() const;
  const std::list<std::string>& getRejectList() const;
};

#endif
