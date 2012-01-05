/**
 * @file LogCentralTool_impl.cc
 *
 * @brief Defines an object implementing the interface LogCentralTool defined in
 * the LogCentralTool.idl
 *
 * @author
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "LogCentralTool_impl.hh"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <ctime>

#include "ORBMgr.hh"

using namespace std;


LogCentralTool_impl::LogCentralTool_impl(ToolList* toolList,
                                         ComponentList* compList,
                                         FilterManagerInterface* filterMan,
                                         StateManager* stateMan,
                                         tag_list_t* allTags)
{
  this->toolList = toolList;
  componentList = compList;
  filterManager = filterMan;
  stateManager = stateMan;
  this->allTags = (*allTags);
  srand(time(NULL));
}

LogCentralTool_impl::~LogCentralTool_impl()
{
  // nothing to do
}

void
LogCentralTool_impl::test()
{
}

CORBA::Short
LogCentralTool_impl::connectTool(char*& toolName,
                                 const char* msgRec)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);
  ToolList::Iterator* it;
  char tmpName[6];
  it = toolList->getIterator();

  ToolMsgReceiver_ptr msgReceiver = ORBMgr::getMgr()->resolve
    <ToolMsgReceiver, ToolMsgReceiver_ptr>(LOGTOOLMSGCTXT, msgRec);

  if (CORBA::is_nil(msgReceiver)){
    logger->log(dadi::Message("LCT",
                              "Failed to resolve LCT. Nil receiver",
                              dadi::Message::PRIO_DEBUG));
  }
  // Check if toolName must be created.
  while (strcmp(toolName,"")==0) {
    sprintf(tmpName,"%ld",(rand() % 99999));
    toolName = CORBA::string_dup(tmpName);

    // check if new name is valid
    if (getToolByName(toolName,it)==true) {
      // repeat this loop
      toolName = CORBA::string_dup("");
    }
  }
  // Check if tool exists
  if (getToolByName(toolName,it)) {
    // tool already exists and cannot be connected a second time
    delete(it);
    logger->log(dadi::Message("LCT",
                              "Connection of tool: '"+string(toolName)+"' failed, tool already exists",
                              dadi::Message::PRIO_DEBUG));
    return LS_TOOL_CONNECT_ALREADYEXISTS;
  }
  // create ToolElement and insert it
  ToolElement* tElem;
  tElem = new ToolElement();
  tElem->toolName = CORBA::string_dup(toolName);
  tElem->msgReceiver = ToolMsgReceiver::_narrow(msgReceiver);
  // filterList and outBuffer are static

  it->reset();
  it->insertBeforeRef(tElem);

  // FIXME: can we reduce the iterator here or do we have to keep it
  // to allow a synchronised askForSystemState ?
  // notify FilterManager
  filterManager->toolConnect(toolName,it);

  // "produce" Initial Config for tool
  stateManager->askForSystemState(&(tElem->outBuffer));

  delete(it);
    logger->log(dadi::Message("LCT",
                              "Connection of tool: '"+string(toolName)+"' done",
                              dadi::Message::PRIO_DEBUG));
  return LS_OK;
}

CORBA::Short
LogCentralTool_impl::disconnectTool(const char* toolName)
{
  dadi::LoggerPtr logger = dadi::LoggerPtr(dadi::Logger::getLogger("org.dadicorba"));
  logger->setLevel(dadi::Message::PRIO_TRACE);
  dadi::ChannelPtr chan = dadi::ChannelPtr(new dadi::ConsoleChannel);
  logger->setChannel(chan);
  ToolList::Iterator* it;

  it = toolList->getIterator();

  // Check if tool exists
  if (getToolByName(toolName,it)==false) {
    // tool does not exist and cannot be disconnected
    delete(it);
        logger->log(dadi::Message("LCT",
                                  "Disconnection of tool "+string(toolName)+" failed because it does not exist",
                                  dadi::Message::PRIO_DEBUG));
           toolName);
    return LS_TOOL_DISCONNECT_NOTEXISTS;
  }

  // notify FilterManager
  filterManager->toolDisconnect(toolName, it);

  // remove Tool
  getToolByName(toolName, it);
  it->removeCurrent(); // element and all its lists, buffers, etc are
                       // deleted automatically

  delete(it);
        logger->log(dadi::Message("LCT",
                                  "Disconnection of tool "+string(toolName)+" done",
                                  dadi::Message::PRIO_DEBUG));
  return LS_OK;
}


tag_list_t*
LogCentralTool_impl::getDefinedTags()
{
  tag_list_t* tagList;
  tagList = new tag_list_t();

  (*tagList) = allTags;

  return tagList;
}

component_list_t*
LogCentralTool_impl::getDefinedComponents()
{
  ComponentList::ReadIterator* it;
  ComponentElement* compElem;
  component_list_t* compList;

  compList = new component_list_t();
  it = componentList->getReadIterator();

  compList->length(it->length());

  for (unsigned int i=0; i < (it->length()); i++) {
    compElem = it->nextRef();
    (*compList)[i] = CORBA::string_dup(compElem->componentName);
  }

  delete(it);
  return compList;
}

CORBA::Short
LogCentralTool_impl::addFilter(const char* toolName, const filter_t& filter)
{
  ToolList::Iterator* toolIt;
  ToolList::ReadIterator* toolRIt;
  FilterList::Iterator* filterIt;
  ToolElement* actTool;

  // Make sure tool exists
  toolIt = toolList->getIterator();  // get a writeIterator as we want to change
                                     // the filters
  if (getToolByName(toolName, toolIt)==false) {
    // tool does not exist, so filter cannot be added
    delete (toolIt);
    return LS_TOOL_ADDFILTER_TOOLNOTEXISTS;
  }
  actTool = toolIt->getCurrentRef();

  // Make sure this filter for this tool does not exist
  filterIt = actTool->filterList.getIterator();
  if (getFilterByName(filter.filterName, filterIt)==true) {
    // filter exists, so we cannot add it
    delete (filterIt);
    delete (toolIt);
    return LS_TOOL_ADDFILTER_ALREADYEXISTS;
  }

  // okay, we can copy this filter and insert it
  filter_t* filterTmp = new filter_t;
  (*filterTmp) = filter;
  filterIt->reset();
  filterIt->insertBeforeRef(filterTmp);
  delete(filterIt);

  // notify FilterManager
  toolRIt = toolList->reduceWriteIterator(toolIt);
  filterManager->addFilter(toolName, filter.filterName, toolRIt);

  // dont forget: release the iterator for the ToolList at the End !
  delete(toolRIt);

  return LS_OK;
}

CORBA::Short
LogCentralTool_impl::removeFilter(const char* toolName, const char* filterName)
{
  ToolList::Iterator* toolIt;
  FilterList::Iterator* filterIt;
  ToolElement* actTool;

  // Make sure tool exists
  toolIt = toolList->getIterator();  // get a writeIterator as we want to change
                                     // the filters
  if (getToolByName(toolName, toolIt)==false) {
    // tool does not exist, so filter cannot be removed
    delete (toolIt);
    return LS_TOOL_REMOVEFILTER_TOOLNOTEXISTS;
  }
  actTool = toolIt->getCurrentRef();

  // Make sure this filter for this tool exists
  filterIt = actTool->filterList.getIterator();
  if (getFilterByName(filterName, filterIt)==false) {
    // filter does not exist, so we cannot delete it
    delete (filterIt);
    delete (toolIt);
    return LS_TOOL_REMOVEFILTER_NOTEXISTS;
  }
  delete(filterIt);  // delete the filterIterator, as the Filtermanager will
                     // open its own. List is still locked by toolIt

  // notify FilterManager
  filterManager->removeFilter(toolName, filterName, toolIt);

  // retrieve filter from list and delete it
  filterIt = actTool->filterList.getIterator();
  getFilterByName(filterName, filterIt);
  filterIt->removeCurrent();  // filterElement is deleted automatically
  delete(filterIt);

  // dont forget: release the iterator for the ToolList at the End !
  delete(toolIt);

  return LS_OK;
}

CORBA::Short
LogCentralTool_impl::flushAllFilters(const char* toolName)
{
  ToolList::Iterator* toolIt;
  ToolElement* actTool;

  // Make sure tool exists
  toolIt = toolList->getIterator();  // get a writeIterator as we want to change
                                     // the filters
  if (getToolByName(toolName, toolIt)==false) {
    // tool does not exist, so filters cannot be removed
    delete (toolIt);
    return LS_TOOL_REMOVEFILTER_TOOLNOTEXISTS;
  }
  actTool = toolIt->getCurrentRef();

  // notify FilterManager
  filterManager->flushAllFilters(toolName, toolIt);

  // now delete all Filters
  actTool->filterList.emptyIt();

  delete(toolIt);
  return LS_OK;
}

bool
LogCentralTool_impl::getToolByName(const char* toolName,
                                   ToolList::ReadIterator* it)
{
  bool exists = false;

  it->reset();
  while (it->hasCurrent()) {
    if (strcmp((char*)(it->getCurrentRef()->toolName),toolName)==0) {
      exists=true;
      break;
    }
    it->nextRef();
  }

  return exists;
}

bool
LogCentralTool_impl::getFilterByName(const char* filterName,
                                     FilterList::ReadIterator* it)
{
  bool exists = false;

  it->reset();
  while(it->hasCurrent()) {
    if (strcmp((char*)(it->getCurrentRef()->filterName),filterName)==0) {
      exists=true;
      break;
    }
    it->nextRef();
  }

  return exists;
}


