/****************************************************************************/
/* Implementation for the TimeBuffer class                                  */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.2  2011/02/04 15:13:02  bdepardo
 * isOlder is now const
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "TimeBuffer.hh"
#include <iostream>


TimeBuffer::TimeBuffer() {
  this->msgList = new MsgLinkedList();
  this->lastTime.sec = 0;
  this->lastTime.msec = 0;
}

TimeBuffer::~TimeBuffer() {
  delete this->msgList;
}

void
TimeBuffer::put(log_msg_t* msg) {
  msg->warning = false;
  // To get an read/write iterator for the messages list
  MsgLinkedList::Iterator* it = this->msgList->getIterator();
  // find the good place to insert the message (according to its timestamp)
  // Messages are order from older messages to newer messages (e.g. newer
  // messages are on the top of the list)
  it->resetToLast();
  while (it->hasCurrent()) {
    if (!isOlder(it->getCurrentRef()->time, msg->time)) {
      break;
    }
    it->previousRef();
  }
  if (it->hasCurrent()) {
    it->insertAfter(msg);
  } else {
    // must insert in the first position
    if (this->isOlder(msg->time, this->lastTime)) {
      msg->warning = true;
    }
    it->reset();
    it->insertBefore(msg);
  }
  delete it;
}

log_msg_t*
TimeBuffer::get(log_time_t minAge) {
  log_msg_t* msg = NULL;
  MsgLinkedList::Iterator* it = this->msgList->getIterator();
  if (it->hasCurrent()) {
    // check the age limit
    if (this->isOlder(minAge, it->getCurrentRef()->time)) {
      msg = it->removeAndGetCurrent();
      this->lastTime = msg->time;
    }
  }
  delete it;
  return msg;
}

// true if t2 older than t1
bool
TimeBuffer::isOlder(log_time_t t1, log_time_t t2) const {
  // important not to use <=
  return ((t2.sec < t1.sec)
          || ((t2.sec == t1.sec) && (t2.msec < t1.msec)));
}

