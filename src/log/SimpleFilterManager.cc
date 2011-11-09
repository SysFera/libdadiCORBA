/****************************************************************************/
/* Simple implementation of the FilterManagerInterface. The implementation  */
/* is complete, but not very efficient.                                     */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: SimpleFilterManager.cc,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: SimpleFilterManager.cc,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "SimpleFilterManager.hh"
#include <string.h>
#include <stdio.h>

using namespace std;

SimpleFilterManager::SimpleFilterManager(ToolList* toolList,
                                         ComponentList* compList,
                                         tag_list_t* stateTags)
{
  this->toolList = toolList;
  this->componentList = compList;
  
  if (stateTags != NULL) {
    this->systemStateTags = *stateTags;
  }
}

SimpleFilterManager::~SimpleFilterManager()
{
}

void
SimpleFilterManager::toolConnect(const char* toolName,
                                 ToolList::ReadIterator* iter)
{
  // nothing to do here
}

void
SimpleFilterManager::toolDisconnect(const char* toolName,
                                    ToolList::ReadIterator* iter)
{
  // nothing to do here
}

void
SimpleFilterManager::addFilter(const char* toolName,
                               const char* filterName,
                               ToolList::ReadIterator* iter)
{
  ToolElement* toolEl;
  FilterList::ReadIterator* filterIt;

  // find tool
  iter->reset();
  while(iter->hasCurrent()) {
    if (strcmp(toolName, (char*)(iter->getCurrentRef()->toolName)) == 0) {
      break;
    }
    iter->nextRef();
  }
  toolEl = iter->getCurrentRef();

  // find filter
  filterIt = toolEl->filterList.getReadIterator();
  while(filterIt->hasCurrent()) {
    if (strcmp(filterName, (char*)(filterIt->getCurrentRef()->filterName)) == 0) {
      break;
    }
    filterIt->nextRef();
  }

  // add the new filter to the existing configs
  addFilter(filterIt->getCurrentRef());
  delete(filterIt);

  // Activate the new configuration
  updateComponentConfigs();
}

void
SimpleFilterManager::removeFilter(const char* toolName,
                                  const char* filterName,
                                  ToolList::ReadIterator* iter)
{
  ToolElement* toolEl;
  FilterList::ReadIterator* filterIt;
  ConfigList::Iterator* configIt;
  bool toolMatches;

  // rebuild all configs
  // clear configs first...
  configIt = configList.getIterator();
  while (configIt->hasCurrent()) {
    // reset config to core tags
    configIt->getCurrentRef()->config = systemStateTags;
    configIt->nextRef();
  }
  delete(configIt);

  // for each tool and each filter
  iter->reset();
  while(iter->hasCurrent()) {
    if (strcmp(toolName, (char*)(iter->getCurrentRef()->toolName)) == 0) {
      toolMatches = true;
    } else {
      toolMatches = false;
    }
    toolEl = iter->getCurrentRef();

    filterIt = toolEl->filterList.getReadIterator();
    while (filterIt->hasCurrent()) {
      // add this filter if it is not the one that will be removed
      if (!toolMatches ||
         (!strcmp(filterName, (char*)(filterIt->getCurrentRef()->filterName)) == 0)) {
        addFilter(filterIt->getCurrentRef());
      }
      filterIt->nextRef();
    }
    delete(filterIt);
    iter->nextRef();
  }

  // Activate the new configuration
  updateComponentConfigs();
}

void
SimpleFilterManager::flushAllFilters(const char* toolName,
                                     ToolList::ReadIterator* iter)
{
  ToolElement* toolEl;
  FilterList::ReadIterator* filterIt;
  ConfigList::Iterator* configIt;

  // rebuild all configs
  // clear configs first...
  configIt = configList.getIterator();
  while (configIt->hasCurrent()) {
    // reset config to core tags
    configIt->getCurrentRef()->config = systemStateTags;
    configIt->nextRef();
  }
  delete(configIt);

  // for each tool and each filter
  iter->reset();
  while(iter->hasCurrent()) {
    toolEl = iter->getCurrentRef();
    if (strcmp(toolName, (char*)(iter->getCurrentRef()->toolName)) != 0) {
      // this is not the tool whose filters are flushed, so add its filters

      filterIt = toolEl->filterList.getReadIterator();
      while (filterIt->hasCurrent()) {
        addFilter(filterIt->getCurrentRef());
        filterIt->nextRef();
      }
      delete(filterIt);
    }
    iter->nextRef();
  }

  // Activate the new configuration
  updateComponentConfigs();
}

tag_list_t*
SimpleFilterManager::componentConnect(const char* componentName,
                                      ComponentList::ReadIterator* iter)
{
  ConfigList::Iterator* cfgIt;
  ToolList::ReadIterator* toolIt;
  FilterList::ReadIterator* filterIt;

  ConfigElement* cfgEl;
  ToolElement* toolEl;

  // add a configuration for the new component
  cfgEl = new ConfigElement();
  cfgEl->componentName = CORBA::string_dup(componentName);
  // "empty" configs always include the systemState tags
  cfgEl->config = systemStateTags;
  cfgEl->oldConfig = systemStateTags;

  cfgIt = configList.getIterator();
  cfgIt->insertBeforeRef(cfgEl);

  // now iterate through each Filter
  // if it matches add it to the new config
  toolIt = toolList->getReadIterator();
  while (toolIt->hasCurrent()) {
    toolEl = toolIt->getCurrentRef();
    filterIt = toolEl->filterList.getReadIterator();
    while (filterIt->hasCurrent()) {
      if (containsComponent(&(filterIt->getCurrentRef()->componentList),
                            componentName)) {
        addTagList(&(cfgEl->config), &(filterIt->getCurrentRef()->tagList));
      }
      filterIt->nextRef();
    }
    delete(filterIt);
    toolIt->nextRef();
  }
  delete(toolIt);

  // send config back 
  tag_list_t* tl = new tag_list_t();
  (*tl) = cfgEl->config;

  delete(cfgIt);
  return tl;
}

void
SimpleFilterManager::componentDisconnect(const char* componentName,
                                         ComponentList::ReadIterator* iter)
{
  // remove the components configuration
  ConfigList::Iterator* it;

  it = configList.getIterator();
  while (it->hasCurrent()) {
    if (strcmp((char*)(it->getCurrentRef()->componentName),componentName)==0) {
      break;
    }
    it->nextRef();
  }
  it->removeCurrent();  // Element will be deleted automatically
  delete it;
}

void
SimpleFilterManager::sendMessageWithFilters(log_msg_t* message)
{
  ToolList::ReadIterator* toolIt;
  FilterList::ReadIterator* filterIt;
  filter_t* filter;

  toolIt = toolList->getReadIterator();
  while (toolIt->hasCurrent()) {

    filterIt = toolIt->getCurrentRef()->filterList.getReadIterator();
    while (filterIt->hasCurrent()) {
      filter = filterIt->getCurrentRef();

      if (containsComponent(&(filter->componentList),message->componentName)) {
        if (containsTag(&(filter->tagList),message->tag)) {
          // okay, this filter fits
          // ush the element on the list, it will be copied automatically
          toolIt->getCurrentRef()->outBuffer.push(message);
          // DEBUG: printf("shifted message in outBuffer\n");
          break;  // no need to check the remaining filters of this tool ...
        }
      }

      filterIt->nextRef();
    }
    delete(filterIt);
    toolIt->nextRef();
  }
  delete(toolIt);
}


// Private Helpers
//   component_list_t helpers

bool
SimpleFilterManager::containsComponent(component_list_t* list,
                                        const char* name)
{
  if (containsComponentStar(list)) {
    return true;
  } else if (containsComponentName(list, name)) {
    return true;
  }
  return false;
}

bool
SimpleFilterManager::containsComponentName(component_list_t* list,
                                           const char* name)
{
  for (unsigned int i=0; i < list->length(); i++) {
    if (strcmp((*list)[i], name) == 0) {
      // if found: break by returning
      return true;
    }
  }
  return false;
}

bool
SimpleFilterManager::containsComponentStar(component_list_t* list)
{
  for(unsigned int i=0; i< list->length(); i++) {
    if(strcmp((*list)[0], "*") == 0) {
      // if found: break by returning
      return true;
    }
  }
  return false;
}

// tag_list_t helpers (copy and paste of component_list_t)

bool
SimpleFilterManager::containsTag(tag_list_t* list,
                                        const char* name)
{
  if (containsTagStar(list)) {
    return true;
  } else if (containsTagName(list, name)) {
    return true;
  }
  return false;
}

bool
SimpleFilterManager::containsTagName(tag_list_t* list,
                                           const char* name)
{
  for (unsigned int i=0; i < list->length(); i++) {
    if (strcmp((*list)[i], name) == 0) {
      // if found: break by returning
      return true;
    }
  }
  return false;
}

bool
SimpleFilterManager::containsTagStar(tag_list_t* list)
{
  for(unsigned int i=0; i< list->length(); i++) {
    if(strcmp((*list)[0], "*") == 0) {
      // if found: break by returning
      return true;
    }
  }
  return false;
}

void
SimpleFilterManager::addTagList(tag_list_t* destList,
                                tag_list_t* srcList)
{
  unsigned int i_src;
  int len;

  if (containsTagStar(destList)) {
    // no need to add up, break and return
    return;
  }
  if (containsTagStar(srcList)) {
    // no need to add up, set destList and return
    destList->length(1);
    (*destList)[0]=CORBA::string_dup("*");
    return;
  }

  // no stars, we need a complete merge
  for (i_src=0; i_src < srcList->length(); i_src++) {
    if (!containsTagName(destList, (*srcList)[i_src])) {
      len = destList->length();
      destList->length(len+1);
      (*destList)[len] = CORBA::string_dup((*srcList)[i_src]);
    }
  }
}

void
SimpleFilterManager::addFilter(filter_t* filter)
{
  ConfigList::Iterator* configIt;

  configIt = configList.getIterator();

  while(configIt->hasCurrent()) {
    if (containsComponent(&(filter->componentList), 
                          (const char*)(configIt->getCurrentRef()->componentName))) {
      // this component is part of the filter and must be altered
      addTagList(&(configIt->getCurrentRef()->config), &(filter->tagList));
    }

    configIt->nextRef();
  }

  delete(configIt);
}

void
SimpleFilterManager::updateComponentConfigs() {
  ConfigList::Iterator* configIt;
  ConfigElement* cfgEl;
  ComponentList::ReadIterator* compIt;
  ComponentElement* compEl;

  unsigned int i;
  bool listsEqual;

  configIt = configList.getIterator();
  compIt = componentList->getIterator();

  while(configIt->hasCurrent()) {
    cfgEl = configIt->getCurrentRef();

    // Check if configuration has changed and really must be sent
    // FIXME: Is this really performant ?
    listsEqual = true; // assume they are equal
    if (cfgEl->config.length() == cfgEl->oldConfig.length()) {
      for (i=0; i < cfgEl->config.length(); i++) {
        if (!containsTagName(&(cfgEl->oldConfig), cfgEl->config[i])) {
          listsEqual=false;
          break;
        }
      }
    }
    else {    // the length does not match, so the lists never match
      listsEqual=false;
    }

    // no need to inform component if config stays the same
    if (!listsEqual) {
      compIt->reset();
      while (compIt->hasCurrent()) {
        if (strcmp(compIt->getCurrentRef()->componentName, cfgEl->componentName) == 0) {
          compEl = compIt->getCurrentRef();
          break; // we have found our tool ...
        }
        compIt->nextRef();
      }
      try {
        compEl->componentConfigurator->setTagFilter(cfgEl->config);
      } catch (CORBA::SystemException& e) {
        printf("NETWORK WARNING: Could not configure the component '%s'.\n",
                (const char*)(compEl->componentName));
      }
    }

    cfgEl->oldConfig = cfgEl->config;
    configIt->nextRef();
  }

  delete(compIt);
  delete(configIt);
}
