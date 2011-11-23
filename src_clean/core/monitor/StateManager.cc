/****************************************************************************/
/* Implementation for the StateManager class                                */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: StateManager.cc,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: StateManager.cc,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "StateManager.hh"
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

StateManager::StateManager(ReadConfig* readConfig, bool* success)
{
  *success = false;
  this->dynamicTagsList = readConfig->getDynamicTags();
  if (this->dynamicTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  } else {
    char* s;

    this->start = readConfig->getDynamicStartSuffix();
    unsigned int l1 = strlen(start);
    this->stop = readConfig->getDynamicStopSuffix();
    unsigned int l2 = strlen(stop);

    this->dynamicStarts = new tag_list_t();
    this->dynamicStarts->length(this->dynamicTagsList->length());
    this->dynamicStops = new tag_list_t();
    this->dynamicStops->length(this->dynamicTagsList->length());

    for (CORBA::ULong i = 0 ; i < this->dynamicTagsList->length() ; i++) {
      s = new char[strlen((*(this->dynamicTagsList))[i]) + l1 + 2];
      sprintf(s, "%s_%s", (const char*)(*(this->dynamicTagsList))[i],
          this->start);
      (*(this->dynamicStarts))[i] = CORBA::string_dup(s);
      delete[] s;

      s = new char[strlen((*(this->dynamicTagsList))[i]) + l2 + 2];
      sprintf(s, "%s_%s", (const char*)(*(this->dynamicTagsList))[i],
          this->stop);
      (*(this->dynamicStops))[i] = CORBA::string_dup(s);
      delete[] s;
    }
  }
  this->staticTagsList = readConfig->getStaticTags();
  if (this->staticTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  }
  this->uniqueTagsList = readConfig->getUniqueTags();
  if (this->uniqueTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  }
  this->stateList = new StateList();
  *success = true;
}

StateManager::~StateManager()
{
  if (this->dynamicTagsList != NULL) {
    delete this->dynamicTagsList;
    delete this->dynamicStarts;
    delete this->dynamicStops;
    delete[] this->start;
    delete[] this->stop;
  }
  if (this->staticTagsList != NULL) {
    delete this->staticTagsList;
  }
  if (this->uniqueTagsList != NULL) {
    delete this->uniqueTagsList;
  }
  delete this->stateList;
}

bool
StateManager::check(log_msg_t* msg)
{
  if (this->dynamicTagsList == NULL) {
    return false;
  }
  char* tag = msg->tag;
  char* name = msg->componentName;
  StateList::Iterator* it = NULL;
  bool found = false;
  log_msg_t* state = NULL;
  // Check if this tag belongs to the unique tag list
  for (CORBA::ULong i = 0 ; i < this->uniqueTagsList->length() ; i++) {
    if (strcmp(tag, (*(this->uniqueTagsList))[i]) == 0) {
      found = true;
      it = this->stateList->getIterator();
      it->resetToLast();
      while (it->hasCurrent()) {
        state = it->getCurrentRef();
        if (strcmp(name, state->componentName) == 0
          && strcmp(tag, state->tag) == 0) {
          it->removeCurrent();
          break;
        } else {
          it->previousRef();
        }
      }
      delete it;
      this->stateList->push(msg);
    }
  }
  if (!found) {
    found = false;
    // Check if this tag belongs to the dynamic starts tag list
    for (CORBA::ULong i = 0 ; i < this->dynamicStarts->length() ; i++) {
      if (strcmp(tag, (*(this->dynamicStarts))[i]) == 0) {
        found = true;
        this->stateList->push(msg);
        break;
      }
    }
  }
  if (!found) {
    found = false;
    // Check if this tag belongs to the dynamic stops tag list
    for (CORBA::ULong i = 0 ; i < this->dynamicStops->length() ; i++) {
      if (strcmp(tag, (*(this->dynamicStops))[i]) == 0) {
        found = true;
        unsigned int l1 = strlen(tag);
        unsigned int l2 = strlen(this->start);
        unsigned int l3 = strlen(this->stop);
        *(tag + l1 - l3) = '\0'; // trunc after the '_'
        char* s = new char[l1 + l2 - l3 + 1];
        sprintf(s, "%s%s", tag, this->start);
        *(tag + l1 - l3) = *(this->stop); // restore the string
        it = this->stateList->getIterator();
        it->resetToLast();
        while (it->hasCurrent()) {
          state = it->getCurrentRef();
          if (strcmp(name, state->componentName) == 0
            && strcmp(s, state->tag) == 0) {
            it->removeCurrent();
            break;
          } else {
            it->previousRef();
          }
        }
        delete it;
        delete[] s;
        break;
      }
    }
  }
  if (!found) {
    found = false;
    // Check if this tag belongs to the static tag list
    for (CORBA::ULong i = 0 ; i < this->staticTagsList->length() ; i++) {
      if (strcmp(tag, (*(this->staticTagsList))[i]) == 0) {
        found = true;
        this->stateList->push(msg);
        break;
      }
    }
  }
  if (!found) {
    found = false;
    // Check if this tag is an IN or OUT tag
    if (strcmp(tag, "IN") == 0 ) {
      found = true;
      this->stateList->push(msg);
    } else if (strcmp(tag, "OUT") == 0) {
      found = true;
      it = this->stateList->getIterator();
      while (it->hasCurrent()) {
        state = it->getCurrentRef();
        if (strcmp(name, state->componentName) == 0) {
          it->removeCurrent();
        } else {
          it->nextRef();
        }
      }
      delete it;
    }
  }
  return found;
}

void
StateManager::askForSystemState(OutBuffer* outBuffer)
{
  if (outBuffer != NULL) {
    outBuffer->appendList(this->stateList);
  }
}

