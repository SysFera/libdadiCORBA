/****************************************************************************/
/* Implements the object LogCentralTool defined in the LogTool.idl          */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.4  2010/12/03 12:40:27  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.3  2010/11/10 04:23:44  bdepardo
 * Correctly generate random number
 *
 * Revision 1.2  2010/11/10 02:27:44  kcoulomb
 * Update the log to use the forwarder.
 * Programm run without launching forwarders but fails with forwarder.
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "LogCentralTool_impl.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include "ORBMgr.hh"



LogCentralTool_impl::LogCentralTool_impl(ToolList* toolList,
                                         ComponentList* compList,
                                         FilterManagerInterface* filterMan,
                                         StateManager* stateMan,
                                         tag_list_t* allTags) {
  this->toolList = toolList;
  componentList = compList;
  filterManager = filterMan;
  stateManager = stateMan;
  this->allTags = (*allTags);
  srand(time(NULL));
}

LogCentralTool_impl::~LogCentralTool_impl() {
}

void
LogCentralTool_impl::test() {
}

CORBA::Short
LogCentralTool_impl::connectTool(char*& toolName, const char* msgRec) {
  ToolList::Iterator* it;
  char tmpName[6];
  it = toolList->getIterator();
  // MODIF ***
  ToolMsgReceiver_ptr msgReceiver = ORBMgr::getMgr()->resolve
    <ToolMsgReceiver, ToolMsgReceiver_ptr>(LOGTOOLMSGCTXT, msgRec);

  if (CORBA::is_nil(msgReceiver)) {
    fprintf(stderr, "Failed to resolve LCT. Nil receiver \n");
  }
  // Check if toolName must be created.
  while (strcmp(toolName, "") == 0) {
    snprintf(tmpName, 6, "%ld", (rand() % 99999));
    toolName = CORBA::string_dup(tmpName);

    // check if new name is valid
    if (getToolByName(toolName, it) == true) {
      // repeat this loop
      toolName = CORBA::string_dup("");
    }
  }
  // Check if tool exists
  if (getToolByName(toolName, it)) {
    // tool already exists and cannot be connected a second time
    delete(it);
    printf("Connection of tool: '%s' failed, tool already exists\n",
           toolName);
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
  filterManager->toolConnect(toolName, it);

  // "produce" Initial Config for tool
  stateManager->askForSystemState(&(tElem->outBuffer));

  delete(it);
  printf("Connection of tool '%s'\n", toolName);
  return LS_OK;
}

CORBA::Short
LogCentralTool_impl::disconnectTool(const char* toolName) {
  ToolList::Iterator* it;

  it = toolList->getIterator();

  // Check if tool exists
  if (getToolByName(toolName, it) == false) {
    // tool does not exist and cannot be disconnected
    delete(it);
    printf("Disconnection of tool: '%s' failed, tool does not exist\n",
           toolName);
    return LS_TOOL_DISCONNECT_NOTEXISTS;
  }

  // notify FilterManager
  filterManager->toolDisconnect(toolName, it);

  // remove Tool
  getToolByName(toolName, it);
  // element and all its lists, buffers, etc are deleted automatically
  it->removeCurrent();
  delete(it);
  printf("Disconnection of tool '%s'\n", toolName);
  return LS_OK;
}


tag_list_t*
LogCentralTool_impl::getDefinedTags() {
  tag_list_t* tagList;
  tagList = new tag_list_t();

  (*tagList) = allTags;

  return tagList;
}

component_list_t*
LogCentralTool_impl::getDefinedComponents() {
  ComponentList::ReadIterator* it;
  ComponentElement* compElem;
  component_list_t* compList;

  compList = new component_list_t();
  it = componentList->getReadIterator();

  compList->length(it->length());

  for (unsigned int i = 0; i < (it->length()); i++) {
    compElem = it->nextRef();
    (*compList)[i] = CORBA::string_dup(compElem->componentName);
  }

  delete(it);
  return compList;
}

CORBA::Short
LogCentralTool_impl::addFilter(const char* toolName, const filter_t& filter) {
  ToolList::Iterator* toolIt;
  ToolList::ReadIterator* toolRIt;
  FilterList::Iterator* filterIt;
  ToolElement* actTool;

  // Make sure tool exists
  // get a writeIterator as we want to change the filters
  toolIt = toolList->getIterator();
  if (getToolByName(toolName, toolIt) == false) {
    // tool does not exist, so filter cannot be added
    delete (toolIt);
    return LS_TOOL_ADDFILTER_TOOLNOTEXISTS;
  }
  actTool = toolIt->getCurrentRef();

  // Make sure this filter for this tool does not exist
  filterIt = actTool->filterList.getIterator();
  if (getFilterByName(filter.filterName, filterIt) == true) {
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
LogCentralTool_impl::removeFilter(const char* toolName,
                                  const char* filterName) {
  ToolList::Iterator* toolIt;
  FilterList::Iterator* filterIt;
  ToolElement* actTool;

  // Make sure tool exists
  // get a writeIterator as we want to change the filters
  toolIt = toolList->getIterator();
  if (getToolByName(toolName, toolIt) == false) {
    // tool does not exist, so filter cannot be removed
    delete (toolIt);
    return LS_TOOL_REMOVEFILTER_TOOLNOTEXISTS;
  }
  actTool = toolIt->getCurrentRef();

  // Make sure this filter for this tool exists
  filterIt = actTool->filterList.getIterator();
  if (getFilterByName(filterName, filterIt) == false) {
    // filter does not exist, so we cannot delete it
    delete (filterIt);
    delete (toolIt);
    return LS_TOOL_REMOVEFILTER_NOTEXISTS;
  }
  // delete the filterIterator, as the Filtermanager will
  // open its own. List is still locked by toolIt
  delete(filterIt);

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
LogCentralTool_impl::flushAllFilters(const char* toolName) {
  ToolList::Iterator* toolIt;
  ToolElement* actTool;

  // Make sure tool exists
  // get a writeIterator as we want to change the filters
  toolIt = toolList->getIterator();
  if (getToolByName(toolName, toolIt) == false) {
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
                                   ToolList::ReadIterator* it) {
  bool exists = false;

  it->reset();
  while (it->hasCurrent()) {
    if (strcmp((char*)(it->getCurrentRef()->toolName), toolName) == 0) {
      exists = true;
      break;
    }
    it->nextRef();
  }

  return exists;
}

bool
LogCentralTool_impl::getFilterByName(const char* filterName,
                                     FilterList::ReadIterator* it) {
  bool exists = false;

  it->reset();
  while (it->hasCurrent()) {
    if (strcmp((char*)(it->getCurrentRef()->filterName), filterName) == 0) {
      exists = true;
      break;
    }
    it->nextRef();
  }

  return exists;
}






//////////////////////////////////// FWDR

LogCentralToolFwdr_impl::LogCentralToolFwdr_impl(Forwarder_ptr fwdr,
                                                 const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}

LogCentralToolFwdr_impl::~LogCentralToolFwdr_impl() {
}

void
LogCentralToolFwdr_impl::test() {
//  return forwarder->test(objName);
}

/**
 * Connects a new Tool to the monitor.
 * Registers the tool internally with its unique toolName
 * and creates all necessary outbuffers, ...
 * Provides a unique name for the tool if the given toolName
 * is the empty string.
 * (Attention: the omniidl is buggy for string INOUT. The
 * String_INOUT_arg of the LogTool.hh must be replaced by
 * char*&)
 *
 * @param toolName the unique name of the tool
 * @param msgReceiver messageConsumer of the tool, which processes
 * incoming messages for the tool
 * @returns if the connection was successful
 */
CORBA::Short
LogCentralToolFwdr_impl::connectTool(char*& toolName,
                                     const char* msgReceiver) {
  return forwarder->connectTool (toolName, msgReceiver, objName);
}

/**
 * Disconnect a tool from the monitor.
 * Remove the tools filters and deregister tool.
 *
 * @param toolName the unique name of the tool
 * @returns if the deconnection was successful
 */
CORBA::Short
LogCentralToolFwdr_impl::disconnectTool(const char* toolName) {
  return forwarder->disconnectTool(toolName, objName);
}

/**
 * Returns a list of configured tags. This is just a
 * convenience function. It relies on the configuration of
 * the monitor and must not reflect the real system.
 */
tag_list_t*
LogCentralToolFwdr_impl::getDefinedTags() {
  return forwarder->getDefinedTags(objName);
}

/**
 * Returns a list of currently attached components. This is
 * just a convenience function, as all the whole systemstate
 * including all components are sent to the tool upon connection
 * in form of messages.
 */
component_list_t*
LogCentralToolFwdr_impl::getDefinedComponents() {
  return forwarder->getDefinedComponents(objName);
}

/**
 * Add a (posivive) filter for this tool.
 * Messages matching this filter will be forwarded to the tool.
 *
 * @param toolName the name of the tool, which adds the filter
 * @param filter the filterconfiguration containing the filtername,
 * a list of tags and a list of components
 * @returns if the filter could be added properly
 */
CORBA::Short
LogCentralToolFwdr_impl::addFilter(const char* toolName,
                                   const filter_t& filter) {
  return forwarder->addFilter(toolName, filter, objName);
}

/**
 * Remove a existing filter from the list.
 *
 * @param toolName the name of the tool who added the filter
 * @param filterName the name of the filter
 * @returns if the filter could be removed properly
 */
CORBA::Short
LogCentralToolFwdr_impl::removeFilter(const char* toolName,
                                      const char* filterName) {
  return forwarder->removeFilter(toolName, filterName, objName);
}

/**
 * Remove all existing filters of the tool.
 *
 * @param toolName the name of the tool whose filterlist will be cleared
 */
CORBA::Short
LogCentralToolFwdr_impl::flushAllFilters(const char* toolName) {
  return forwarder->flushAllFilters(toolName, objName);
}


ToolMsgReceiverFwdr_impl::ToolMsgReceiverFwdr_impl(Forwarder_ptr fwdr,
                                                   const char* objName) {
  this->forwarder = Forwarder::_duplicate(fwdr);
  this->objName = CORBA::string_dup(objName);
}


ToolMsgReceiverFwdr_impl::~ToolMsgReceiverFwdr_impl() {
}


void
ToolMsgReceiverFwdr_impl::sendMsg(const log_msg_buf_t& msgBuf) {
  return forwarder->sendMsg(msgBuf, objName);
}
