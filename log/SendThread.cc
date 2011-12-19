/**
 * @file SendThread.cc
 *
 * @brief A thread that continously empties the tools outbuffers by sending them
 * to the corresponding toolMsgReceiver. Based on omni_thread and the orb.
 * Attention: causes memory leaks if the orb does not exist !!
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "SendThread.hh"
#include "LogOptions.hh"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

SendThread::SendThread(ToolList* toolList)
{
  this->mtoolList = toolList;
  mrunSendThread=false;
}

void
SendThread::startThread()
{
  if (mrunSendThread == false) {
    mrunSendThread = true;
    start_undetached();
  }
  // else thread is already running
}

void
SendThread::stopThread()
{
  if (mrunSendThread == true) {
    mrunSendThread = false;
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

  while (mrunSendThread) {
    // main loop
    toolIt = mtoolList->getIterator();

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
  if (mrunSendThread == true) {
    mrunSendThread = false;
    join(NULL);
  }
}

