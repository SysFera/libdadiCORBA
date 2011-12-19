/**
 * @file ReadConfig.hh
 *
 * @brief Header of the ReadConfig class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _READCONFIG_HH_
#define _READCONFIG_HH_

#include "LogTypes.hh"
#include <stdio.h>

const short LS_PARSE_ALREADYPARSED = 1;
const short LS_PARSE_FILEERROR = 2;
const short LS_PARSE_SECTIONNOTFOUND = 3;

/**
 * @brief Class to read the specific log central config file
 * @class ReadConfig
 */
class ReadConfig
{
public:
  ReadConfig(const char* configFilename, bool* success);
  ~ReadConfig();

  /**
   * @brief Parse the config file.
   * @return an error level
   */
  short
  parse();

  /**
   * @brief Get the tracelevel to use for CORBA communication.
   * @return the tracelevel or 0 if no tracelevel specified
   */
  unsigned int
  getTracelevel();

  /**
   * @brief Get the Port to use for CORBA communication.
   * @return the port or 0 if no port specified
   */
  unsigned int
  getPort();

  /**
   * @brief Get the dynamic start suffix, by default it's "START".
   * @return the dynmaic start suffix
   */
  char*
  getDynamicStartSuffix();

  /**
   * @brief Get the dynamic stop suffix, by default it's "STOP".
   * @return the dynmaic stop suffix
   */
  char*
  getDynamicStopSuffix();

  /**
   * @brief Get all dynamic system state tags.
   * If a tag name is "TAG1" then a pair of tags is expected : TAG1_START and
   * TAG1_STOP. START and STOP names are get from getDynamicStart-StopSuffix.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getDynamicTags();

  /**
   * @brief Get all static system state tags.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getStaticTags();

  /**
   * @brief Get all unique system state tags.
   * Only one information is keeped at a time. A new tag overwrite the
   * previous.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getUniqueTags();

  /**
   * @brief Get all other tags (volatile tags).
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getVolatileTags();

  /**
   * @brief Get all the System state tags.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getStateTags();

  /**
   * @brief Get all the possible tags.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getAllTags();

private:
  char*
  readLine(FILE* file);

  short
  parseTagSection(FILE* file, const char* sectionName, tag_list_t* taglist);

  void
  appendToList(tag_list_t* list, tag_list_t* appendlist);

private:
  char* mfilename;
  bool malreadyParsed;

  unsigned int mtracelevel;
  unsigned int mport;
  char* mstartSuffix;
  char* mstopSuffix;
  tag_list_t* mdynamicTags;
  tag_list_t* mstaticTags;
  tag_list_t* muniqueTags;
  tag_list_t* mvolatileTags;
};

#endif

