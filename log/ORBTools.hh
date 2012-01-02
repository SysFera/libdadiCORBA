/**
 * @file ORBTools.hh
 * @brief A class for putting some ORB functions together - HEADER
 *
 * @author
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _ORBTOOLS_HH_
#define _ORBTOOLS_HH_

#include "LogTypes.hh"

/**
 * @brief The clas to encapsulate some ORB functionalities
 * @class ORBTools
 */
class ORBTools
{
public:
  /**
   * @brief Initialize the CORBA ORB.
   * See CORBA documentation for all possibles parameters.
   * @param argc Number of parameters to pass to the ORB
   * @param argv Array of parameters to pass to the ORB
   * @return true if the operation succeed
   */
  static
  bool
  init(int argc, char** argv);

  /**
   * @brief Initialize the CORBA ORB.
   * @param argc Number of parameters to pass to the ORB
   * @param argv Array of parameters to pass to the ORB
   * @param tracelevel The tracelevel of the ORB (from 0 to 50)
   * @param port the port to use for listenning servants (0 = default random)
   * @return true if the operation succeed
   */
  static
  bool
  init(int argc, char** argv, unsigned int tracelevel, unsigned int port = 0);

  /**
   * @brief Initialize the CORBA ORB.
   * None argument is set, usefull for tests
   * @return true if the operation succeed
   */
  static
  bool
  init();

  /**
   * @brief Register a servant to the Namming Service within a specific context and
   * name. Do not activate the servant.
   * @param contextName Name of the new context
   * @param contextKind Kind of the new context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The implementation of a servant (result of the _this()
   * function)
   * @return true if the operation succeed
   */
  static
  bool
  registerServant(const char* contextName, const char* contextKind,
                  const char* name, const char* kind,
                  CORBA::Object* objref);

  /**
   * @brief Register a servant to the Namming Service within the default context and
   * with a specific name. Do not activate the servant.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The implementation of a servant (result of the _this()
   * function)
   * @return true if the operation succeed
   */
  static
  bool
  registerServant(const char* name, const char* kind,
                  CORBA::Object* objref);

  /**
   * @brief Activate a servant.
   * After this, the IOR of the servant is known by the POA.
   * @param object The implementation of a servant
   * @return true if the operation succeed
   */
  static
  bool
  activateServant(PortableServer::ServantBase* object);

  /**
   * @brief Activate the POA (Portable Object Adaptor).
   * After this, all servants registered are accessible
   * @return true if the operation succeed
   */
  static
  bool
  activatePOA();

  /**
   * @brief Unregister a servant to the Namming Service within a specific context and
   * name.
   * @param contextName Name of the context
   * @param contextKind Kind of the context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @return true if the operation succeed
   */
  static
  bool
  unregisterServant(const char* contextName, const char* contextKind,
                    const char* name, const char* kind);

  /**
   * @brief Unregister a servant to the Namming Service within the default context and
   * with a specific name.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @return true if the operation succeed
   */
  static
  bool
  unregisterServant(const char* name, const char* kind);

  /**
   * @brief Find a servant by asking the Namming Service with a specific context and
   * name.
   * @param contextName Name of the context
   * @param contextKind Kind of the context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The new servant found (check the return value to be sure
   * that the reference is correct)
   * @return true if the operation succeed
   */
  static
  bool
  findServant(const char* contextName, const char* contextKind,
              const char* name, const char* kind,
              CORBA::Object*& objref);

  /**
   * @brief Find a servant by asking the Namming Service with the default context and
   * name.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The new servant found (check the return value to be sure
   * that the reference is correct)
   * @return true if the operation succeed
   */
  static
  bool
  findServant(const char* name, const char* kind,
              CORBA::Object*& objref);

  /**
   * @brief Make the thread to listen for a incomming connection to a servant.
   * This function will return if the user enters a specific key.
   * @param stopLowercase character to hit for stopping the listen function
   * @param stopUppercase character to hit for stopping the listen function
   * @return true if the operation succeed
   */
  static
  bool
  listen(char stopLowercase, char stopUppercase);

  /**
   * @brief Shutdown and destroy the ORB
   * @return true if the operation succeed
   */
  static
  bool
  kill();

/**
 * @brief The ORB
 */
  static CORBA::ORB_var orb;
/**
 * @brief The POA
 */
  static PortableServer::POA_var poa;
};
#endif

