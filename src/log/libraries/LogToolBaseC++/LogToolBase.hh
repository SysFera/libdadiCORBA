/****************************************************************************/
/* LogToolBase header class                                                 */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogToolBase.hh,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: LogToolBase.hh,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#ifndef _LOGTOOLBASE_HH_
#define _LOGTOOLBASE_HH_

#include "LogTypes.hh"
#include "LogTool.hh"

class LogToolBase;
class ToolMsgReceiver_impl: public POA_ToolMsgReceiver,
                            public PortableServer::RefCountServantBase
{
public:
  ToolMsgReceiver_impl(LogToolBase* LTB);
  ~ToolMsgReceiver_impl();

  void
  sendMsg(const log_msg_buf_t& msgBuf);

private:
  LogToolBase* LTB;
};


class LogToolBase
{
public:
  LogToolBase(bool* succes, int argc, char** argv,
              unsigned int tracelevel, unsigned int port = 0);

  virtual
  ~LogToolBase();

  void
  setName(const char* name);

  char*
  getName();

  short
  connect();

  short
  disconnect();

  short
  addFilter(const filter_t& filter);

  short
  removeFilter(const char* filterName);

  short
  flushAllFilters();

  tag_list_t*
  getDefinedTags();

  component_list_t*
  getDefinedComponents();

  virtual
  void
  sendMsg(const log_msg_t& msg) = 0;

private:
  LogCentralTool_var LCTref;
  ToolMsgReceiver_impl* TMRimpl;
  ToolMsgReceiver_var TMRref;
  char* name;
};

#endif

