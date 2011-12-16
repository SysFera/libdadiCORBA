/**
 * @file ReadConfig.cc
 *
 * @brief Implementation of the ReadConfig class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "ReadConfig.hh"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

ReadConfig::ReadConfig(const char* configFilename, bool* success) {
  *success = false;
  this->filename = NULL;
  if (configFilename == NULL) {
    return;
  }
  FILE* file = fopen(configFilename, "r");
  if (file == NULL) {
    return;
  }
  fclose(file);
  this->filename = strdup(configFilename);
  this->alreadyParsed = false;
  this->tracelevel = 0;
  this->port = 0;
  this->startSuffix = NULL;
  this->stopSuffix = NULL;
  this->dynamicTags = new tag_list_t();
  this->staticTags = new tag_list_t();
  this->uniqueTags = new tag_list_t();
  this->volatileTags = new tag_list_t();
  *success = true;
}

ReadConfig::~ReadConfig()
{
  if (this->filename != NULL) {
    free(this->filename);
  }
  delete this->dynamicTags;
  delete this->staticTags;
  delete this->uniqueTags;
  delete this->volatileTags;
}

char*
ReadConfig::readLine(FILE* file)
{
  char* buff = NULL;
  int nextChar;

  // check for file content before trying to read line
  nextChar = fgetc(file);
  if(nextChar == EOF){
    return NULL;
  }
  ungetc(nextChar, file);

  buff = new char[256];
  fgets(buff, 255, file);
  if(buff == NULL){
    return NULL;
  }

  int len = strlen(buff);

  // remove \n
  if (buff[len - 1] == '\n') {
    len--;
    buff[len] = '\0';
  }

  // remove comments
  for (int i=0; i<len; i++) {
    if (buff[i] == '#') {
      buff[i]='\0';
      len = i;
    }
  }

  // remove empty spaces at end of line
  for (int i=len-1; i>=0; i--) {
    if (buff[i] == ' ') {
      buff[i]='\0';
      len=i+1;
    } else {
      break;
    }
  }

  char* ret = new char[len + 1];
  sprintf(ret, "%s", buff);
  delete[] buff;
  return ret;
}

short
ReadConfig::parseTagSection(FILE* file, const char* sectionName,
                            tag_list_t* taglist)
{
  rewind(file);
  int i = 0;
  int n = 0;
  bool found = false;
  char* s;
  // Find the section
  while (i == 0) {
    s = this->readLine(file);
    if (s == NULL){
      i = 2;    // stop if end of file
    } else if (strcmp(s, sectionName) == 0) {
      i = 1;
    } else if (feof(file)) {
      i = 2;    // stop if end of file
    }
    delete[] s;
  }
  if (i == 2) {
    return LS_PARSE_SECTIONNOTFOUND;
  }
  // Parse the section
  i = 0;
  while (i == 0) {
    s = this->readLine(file);
    // stop if new section or end of file
    if ((s == NULL) || s[0] == '[') {
      i = 1;
    // avoid empty lines and commented lines
    } else if (strcmp(s, "") != 0) {
      n = taglist->length();
      found = false;
      for (CORBA::Long j = 0 ; j < n ; j++) {
        if (strcmp(s, (*taglist)[j]) == 0) {
          found = true;
          break;
        }
      }
      if (!found) {
        // increase the length of the list
        taglist->length(n + 1);
        // add the tag
        (*taglist)[n] = CORBA::string_dup(s);
      }
    }
    delete[] s;
  }
  return LS_OK;
}

short
ReadConfig::parse()
{
  if (this->alreadyParsed) {
    return LS_PARSE_ALREADYPARSED;
  }

  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    return LS_PARSE_FILEERROR;
  }

  short i = 0;
  int ltracelevel = 0;
  int lport = 0;
  int lstart = 0;
  int lstop = 0;
  char* s;
  char* s2;

  // First find the General section
  while (i == 0) {
    s = this->readLine(file);
    if(s == NULL) {
      i = 2; // stop if end of file
    } else if (strcmp(s, "[General]") == 0) {
      i = 1;
    } else if (feof(file)) {
      i = 2; // stop if end of file
    }
    delete[] s;
  }
  if (i == 2) {
    return LS_PARSE_SECTIONNOTFOUND;
  }
  // Parse the General section
  i = 0;
  ltracelevel = strlen("Tracelevel=");
  lport = strlen("Port=");
  lstart = strlen("DynamicStartSuffix=");
  lstop = strlen("DynamicStopSuffix=");
  this->tracelevel = 0;
  this->port = 0;
  this->startSuffix = NULL;
  this->stopSuffix = NULL;
  while (i == 0) {
    s = this->readLine(file);
    if((s == NULL) || (s[0] == '[')){
      i = 1;  // stop if new section or end of file
    } else if (strncmp(s, "Tracelevel=", ltracelevel) == 0) {
      sscanf(s, "Tracelevel=%u", &(this->tracelevel));
    } else if (strncmp(s, "Port=", lport) == 0) {
      sscanf(s, "Port=%u", &(this->port));
    } else if (strncmp(s, "DynamicStartSuffix=", lstart) == 0) {
      s2 = new char[strlen(s) - lstart + 1];
      sscanf(s, "DynamicStartSuffix=%s", s2);
      this->startSuffix = strdup(s2);
      delete[] s2;
    } else if (strncmp(s, "DynamicStopSuffix=", lstop) == 0) {
      s2 = new char[strlen(s) - lstop + 1];
      sscanf(s, "DynamicStopSuffix=%s", s2);
      this->stopSuffix = strdup(s2);
      delete[] s2;
    } else if (feof(file)){
      i = 1;  // stop if new section or end of file
    }
    delete[] s;
  }
  if (this->startSuffix == NULL) {
    this->startSuffix = new char[6];
    strncpy(this->startSuffix, "START", 6);
  }
  if (this->stopSuffix == NULL) {
    this->stopSuffix = new char[5];
    strncpy(this->stopSuffix, "STOP", 5);
  }


  // Find the DynamicTagList section
  i = this->parseTagSection(file, "[DynamicTagList]", this->dynamicTags);
  if (i != LS_OK) {
    return i;
  }

  // Find the StaticTagList section
  i = this->parseTagSection(file, "[StaticTagList]", this->staticTags);
  if (i != LS_OK) {
    return i;
  }

  // Find the UniqueTagList section
  i = this->parseTagSection(file, "[UniqueTagList]", this->uniqueTags);
  if (i != LS_OK) {
    return i;
  }

  // Find the VolatileTagList section
  i = this->parseTagSection(file, "[VolatileTagList]", this->volatileTags);
  if (i != LS_OK) {
    return i;
  }

  fclose(file);
  this->alreadyParsed = true;
  return LS_OK;
}

unsigned int
ReadConfig::getTracelevel()
{
  unsigned int ret = 0;
  if (this->alreadyParsed) {
    ret = this->tracelevel;
  }
  return ret;
}

unsigned int
ReadConfig::getPort()
{
  unsigned int ret = 0;
  if (this->alreadyParsed) {
    ret = this->port;
  }
  return ret;
}

char*
ReadConfig::getDynamicStartSuffix()
{
  char* ret = NULL;
  if (this->alreadyParsed) {
    ret = strdup(this->startSuffix);
  }
  return ret;
}

char*
ReadConfig::getDynamicStopSuffix()
{
  char* ret = NULL;
  if (this->alreadyParsed) {
    ret = strdup(this->stopSuffix);
  }
  return ret;
}

tag_list_t*
ReadConfig::getDynamicTags()
{
  tag_list_t* ret = NULL;
  if (this->alreadyParsed) {
    ret = new tag_list_t(*(this->dynamicTags));
  }
  return ret;
}

tag_list_t*
ReadConfig::getStaticTags()
{
  tag_list_t* ret = NULL;
  if (this->alreadyParsed) {
    ret = new tag_list_t(*(this->staticTags));
  }
  return ret;
}

tag_list_t*
ReadConfig::getUniqueTags()
{
  tag_list_t* ret = NULL;
  if (this->alreadyParsed) {
    ret = new tag_list_t(*(this->uniqueTags));
  }
  return ret;
}

tag_list_t*
ReadConfig::getVolatileTags()
{
  tag_list_t* ret = NULL;
  if (this->alreadyParsed) {
    ret = new tag_list_t(*(this->volatileTags));
  }
  return ret;
}

void
ReadConfig::appendToList(tag_list_t* list, tag_list_t* appendlist)
{
  bool b = false;
  CORBA::ULong n1 = list->length();
  CORBA::ULong n2 = appendlist->length();
  for (CORBA::ULong i = 0 ; i < n2 ; i++) {
    b = false;
    for (CORBA::ULong j = 0 ; j < n1 ; j++) {
      if (strcmp((*appendlist)[i], (*list)[j]) == 0) {
        b = true;
        break;
      }
    }
    if (!b) {
      n1++;
      list->length(n1);
      (*list)[n1 - 1] = CORBA::string_dup((*appendlist)[i]);
    }
  }
}

tag_list_t*
ReadConfig::getStateTags()
{
  tag_list_t* ret = NULL;
  if (this->alreadyParsed) {
    CORBA::ULong n = this->dynamicTags->length();
    ret = new tag_list_t();
    ret->length(2 * n);
    char* s1;
    char* s2;
    for (CORBA::ULong i = 0 ; i < n ; i++) {
      s1 = (*this->dynamicTags)[i];
      s2 = new char[strlen(s1) + strlen(this->startSuffix) + 2];
      sprintf(s2, "%s_%s", s1, this->startSuffix);
      (*ret)[2 * i] = CORBA::string_dup(s2);
      delete[] s2;

      s2 = new char[strlen(s1) + strlen(this->stopSuffix) + 2];
      sprintf(s2, "%s_%s", s1, this->stopSuffix);
      (*ret)[2 * i + 1] = CORBA::string_dup(s2);
      delete[] s2;
    }
    this->appendToList(ret, this->staticTags);
    this->appendToList(ret, this->uniqueTags);
  }
  return ret;
}

tag_list_t*
ReadConfig::getAllTags()
{
  tag_list_t* ret = this->getStateTags();
  if (ret != NULL) {
    this->appendToList(ret, this->volatileTags);
  }
  return ret;
}

