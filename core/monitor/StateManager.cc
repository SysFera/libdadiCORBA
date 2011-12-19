/**
 * @file StateManager.hh
 *
 * @brief Implementation of the StateManager class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "StateManager.hh"
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

StateManager::StateManager(ReadConfig* readConfig, bool* success)
{
  *success = false;
  this->mdynamicTagsList = readConfig->getDynamicTags();
  if (this->mdynamicTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  } else {
    char* s;

    this->mstart = readConfig->getDynamicStartSuffix();
    unsigned int l1 = strlen(mstart);
    this->mstop = readConfig->getDynamicStopSuffix();
    unsigned int l2 = strlen(mstop);

    this->mdynamicStarts = new tag_list_t();
    this->mdynamicStarts->length(this->mdynamicTagsList->length());
    this->mdynamicStops = new tag_list_t();
    this->mdynamicStops->length(this->mdynamicTagsList->length());

    for (CORBA::ULong i = 0 ; i < this->mdynamicTagsList->length() ; i++) {
      s = new char[strlen((*(this->mdynamicTagsList))[i]) + l1 + 2];
      sprintf(s, "%s_%s", (const char*)(*(this->mdynamicTagsList))[i],
          this->mstart);
      (*(this->mdynamicStarts))[i] = CORBA::string_dup(s);
      delete[] s;

      s = new char[strlen((*(this->mdynamicTagsList))[i]) + l2 + 2];
      sprintf(s, "%s_%s", (const char*)(*(this->mdynamicTagsList))[i],
          this->mstop);
      (*(this->mdynamicStops))[i] = CORBA::string_dup(s);
      delete[] s;
    }
  }
  this->mstaticTagsList = readConfig->getStaticTags();
  if (this->mstaticTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  }
  this->muniqueTagsList = readConfig->getUniqueTags();
  if (this->muniqueTagsList == NULL) {
    cout << "Internal Error (StateManager)\n";
  }
  this->mstateList = new StateList();
  *success = true;
}

StateManager::~StateManager()
{
  if (this->mdynamicTagsList != NULL) {
    delete this->mdynamicTagsList;
    delete this->mdynamicStarts;
    delete this->mdynamicStops;
    delete[] this->mstart;
    delete[] this->mstop;
  }
  if (this->mstaticTagsList != NULL) {
    delete this->mstaticTagsList;
  }
  if (this->muniqueTagsList != NULL) {
    delete this->muniqueTagsList;
  }
  delete this->mstateList;
}

bool
StateManager::check(log_msg_t* msg)
{
  if (this->mdynamicTagsList == NULL) {
    return false;
  }
  char* tag = msg->tag;
  char* name = msg->componentName;
  StateList::Iterator* it = NULL;
  bool found = false;
  log_msg_t* state = NULL;
  // Check if this tag belongs to the unique tag list
  for (CORBA::ULong i = 0 ; i < this->muniqueTagsList->length() ; i++) {
    if (strcmp(tag, (*(this->muniqueTagsList))[i]) == 0) {
      found = true;
      it = this->mstateList->getIterator();
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
      this->mstateList->push(msg);
    }
  }
  if (!found) {
    found = false;
    // Check if this tag belongs to the dynamic starts tag list
    for (CORBA::ULong i = 0 ; i < this->mdynamicStarts->length() ; i++) {
      if (strcmp(tag, (*(this->mdynamicStarts))[i]) == 0) {
        found = true;
        this->mstateList->push(msg);
        break;
      }
    }
  }
  if (!found) {
    found = false;
    // Check if this tag belongs to the dynamic stops tag list
    for (CORBA::ULong i = 0 ; i < this->mdynamicStops->length() ; i++) {
      if (strcmp(tag, (*(this->mdynamicStops))[i]) == 0) {
        found = true;
        unsigned int l1 = strlen(tag);
        unsigned int l2 = strlen(this->mstart);
        unsigned int l3 = strlen(this->mstop);
        *(tag + l1 - l3) = '\0'; // trunc after the '_'
        char* s = new char[l1 + l2 - l3 + 1];
        sprintf(s, "%s%s", tag, this->mstart);
        *(tag + l1 - l3) = *(this->mstop); // restore the string
        it = this->mstateList->getIterator();
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
    for (CORBA::ULong i = 0 ; i < this->mstaticTagsList->length() ; i++) {
      if (strcmp(tag, (*(this->mstaticTagsList))[i]) == 0) {
        found = true;
        this->mstateList->push(msg);
        break;
      }
    }
  }
  if (!found) {
    found = false;
    // Check if this tag is an IN or OUT tag
    if (strcmp(tag, "IN") == 0 ) {
      found = true;
      this->mstateList->push(msg);
    } else if (strcmp(tag, "OUT") == 0) {
      found = true;
      it = this->mstateList->getIterator();
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
    outBuffer->appendList(this->mstateList);
  }
}

