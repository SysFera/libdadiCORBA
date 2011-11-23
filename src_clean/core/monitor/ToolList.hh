/****************************************************************************/
/* defines a list that holds all information an tools connected to the      */
/* new Logservice                                                           */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ToolList.hh,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: ToolList.hh,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#ifndef _TOOLLIST_HH_
#define _TOOLLIST_HH_

#include "utils/FullLinkedList.hh"
#include "LogTool.hh"

typedef FullLinkedList<log_msg_t> OutBuffer;
typedef FullLinkedList<filter_t> FilterList;

/**
 * All information that is stored for each Tool
 * (filterlist, output buffer, proxy)
 */
struct ToolElement {
  ToolMsgReceiver_var msgReceiver;
  OutBuffer outBuffer;
  FilterList filterList;
  CORBA::String_var toolName;
};

/**
 * Define a ToolList for the ToolElements
 * No Elements should be changed without
 * having a writeLock(Iterator) on the whole
 * List.
 *
 * Attention: This also applies to the nested
 * list filterList. If the filterlist is to be
 * changed, please also acquire a writelock on
 * the surrounding toolList. Only the outBuf
 * may be changed with a readOnly iterator
 * on the toolList.
 */
typedef FullLinkedList<ToolElement> ToolList;

#endif
