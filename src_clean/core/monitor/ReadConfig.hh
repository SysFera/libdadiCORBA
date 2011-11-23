/****************************************************************************/
/* Header of the ReadConfig class                                           */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ReadConfig.hh,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: ReadConfig.hh,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#ifndef _READCONFIG_HH_
#define _READCONFIG_HH_

#include "LogTypes.hh"
#include <stdio.h>

const short LS_PARSE_ALREADYPARSED = 1;
const short LS_PARSE_FILEERROR = 2;
const short LS_PARSE_SECTIONNOTFOUND = 3;

class ReadConfig
{
public:
  ReadConfig(const char* configFilename, bool* success);
  ~ReadConfig();

  /**
   * Parse the config file.
   * @return an error level
   */
  short
  parse();

  /**
   * Get the tracelevel to use for CORBA communication.
   * @return the tracelevel or 0 if no tracelevel specified
   */
  unsigned int
  getTracelevel();

  /**
   * Get the Port to use for CORBA communication.
   * @return the port or 0 if no port specified
   */
  unsigned int
  getPort();

  /**
   * Get the dynamic start suffix, by default it's "START".
   * @return the dynmaic start suffix
   */
  char*
  getDynamicStartSuffix();

  /**
   * Get the dynamic stop suffix, by default it's "STOP".
   * @return the dynmaic stop suffix
   */
  char*
  getDynamicStopSuffix();

  /**
   * Get all dynamic system state tags.
   * If a tag name is "TAG1" then a pair of tags is expected : TAG1_START and
   * TAG1_STOP. START and STOP names are get from getDynamicStart-StopSuffix.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getDynamicTags();

  /**
   * Get all static system state tags.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getStaticTags();

  /**
   * Get all unique system state tags.
   * Only one information is keeped at a time. A new tag overwrite the
   * previous.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getUniqueTags();

  /**
   * Get all other tags (volatile tags).
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getVolatileTags();

  /**
   * Get all the System state tags.
   * @return NULL if the file was not already parsed
   */
  tag_list_t*
  getStateTags();

  /**
   * Get all the possible tags.
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
  char* filename;
  bool alreadyParsed;

  unsigned int tracelevel;
  unsigned int port;
  char* startSuffix;
  char* stopSuffix;
  tag_list_t* dynamicTags;
  tag_list_t* staticTags;
  tag_list_t* uniqueTags;
  tag_list_t* volatileTags;
};

#endif

