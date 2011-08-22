/****************************************************************************/
/* A thread that continously empties the tools outbuffers by sending them   */
/* to the corresponding toolMsgReceiver.                                    */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.4  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/LogOptions due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.3  2010/12/03 12:40:27  kcoulomb
 * MAJ log to use forwarders
 *
 * Revision 1.2  2004/01/13 15:10:49  ghoesch
 * explicity initialize runThread bool
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "SendThread.hh"
#include "LogOptions.hh"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

SendThread::SendThread(ToolList* toolList)
{
  this->toolList = toolList;
  runSendThread=false;
}

void
SendThread::startThread()
{
  if (runSendThread == false) {
    runSendThread = true;
    start_undetached();
  }
  // else thread is already running
}

void
SendThread::stopThread()
{
  if (runSendThread == true) {
    runSendThread = false;
    join(NULL);
  }
  // else thread is not running
}

void*
SendThread::run_undetached(void* arg) {
  ToolList::Iterator* toolIt;
  ToolElement* toolEl;
  OutBuffer::Iterator* bufIt;
  log_msg_t* message;
  log_msg_buf_t msgBuf;
  int bufIndex;

  bool removeTool;

  int tCnt;
  int mCnt;

  while (runSendThread) {
    // main loop
    toolIt = toolList->getIterator();

    tCnt=0;
    mCnt=0;

    // check every tool

    while (toolIt->hasCurrent()) {

      toolEl = toolIt->getCurrentRef();

      // check if the current tool has messages that have to be sent
      bufIt = toolEl->outBuffer.getIterator();
      removeTool = false;
      if (bufIt->length() > 0) {
        // msgs exist, copy them into buffer
        msgBuf.length(bufIt->length());
        bufIndex = 0;
        while (bufIt->hasCurrent()) {
          // Attention: we need no next() here, as the remove() will proceed
          // to the next element in the list
          message = bufIt->removeAndGetCurrent();
          msgBuf[bufIndex] = (*message);  // copy message
          delete(message);
          mCnt++;
          bufIndex++;
        }
        // send the buffer
        try {
          toolEl->msgReceiver->sendMsg(msgBuf);
        } catch(CORBA::SystemException& e) {
          printf("NETWORK WARNING: Could not forward messages to tool '%s'. Disconnecting it.\n",
   	         (const char*)(toolEl->toolName));
          removeTool = true;
        }
      }
      delete(bufIt);

      if (removeTool) {
        toolIt->removeCurrent();
        // FIXME: we should update the Filtermanager here ...
      } else {
        tCnt++;
        toolIt->nextRef();
      }
    }
    delete(toolIt);

    sleep(LogOptions::SENDTHREAD_SLEEP_TIME_SEC,
          LogOptions::SENDTHREAD_SLEEP_TIME_NSEC);
  }
  // leaving main loop
  return NULL;
}

SendThread::~SendThread() {
  if (runSendThread == true) {
    runSendThread = false;
    join(NULL);
  }
}

