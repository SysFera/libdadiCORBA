/**
 * @file ToolList.hh
 *
 * @brief defines a list that holds all information an tools connected to the
 * new Logservice
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _TOOLLIST_HH_
#define _TOOLLIST_HH_

#include "utils/FullLinkedList.hh"
#include "LogTool.hh"

typedef FullLinkedList<log_msg_t> OutBuffer;
typedef FullLinkedList<filter_t> FilterList;

/**
 * @brief All information that is stored for each Tool
 * (filterlist, output buffer, proxy)
 * @struct ToolElement
 */
struct ToolElement {
/**
 * @brief The message receiver
 */
  ToolMsgReceiver_var msgReceiver;
/**
 * @brief The output buffer
 */
  OutBuffer outBuffer;
/**
 * @brief The list of filters
 */
  FilterList filterList;
/**
 * @brief The name of the tool
 */
  CORBA::String_var toolName;
};

/**
 * @brief Define a ToolList for the ToolElements
 * No Elements should be changed without
 * having a writeLock(Iterator) on the whole
 * List.
 * Attention: This also applies to the nested
 * list filterList. If the filterlist is to be
 * changed, please also acquire a writelock on
 * the surrounding toolList. Only the outBuf
 * may be changed with a readOnly iterator
 * on the toolList.
 */
typedef FullLinkedList<ToolElement> ToolList;

#endif
