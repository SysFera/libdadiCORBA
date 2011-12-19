/**
 * @file TimeBuffer.cc
 *
 * @brief Implementation of the TimeBuffer class
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#include "TimeBuffer.hh"
#include <iostream>
using namespace std;

TimeBuffer::TimeBuffer()
{
  this->msgList = new MsgLinkedList();
  this->lastTime.sec = 0;
  this->lastTime.msec = 0;
}

TimeBuffer::~TimeBuffer()
{
  delete this->msgList;
}

void
TimeBuffer::put(log_msg_t* msg)
{
  msg->warning = false;
  // To get an read/write iterator for the messages list
  MsgLinkedList::Iterator* it = this->msgList->getIterator();
  // find the good place to insert the message (according to its timestamp)
  // Messages are order from older messages to newer messages (e.g. newer
  // messages are on the top of the list)
  it->resetToLast(); // go at the end of the list
  while (it->hasCurrent()) {
    if (!isOlder(it->getCurrentRef()->time, msg->time)) {
      break;
    }
    it->previousRef();
  }
  if (it->hasCurrent()) {
    it->insertAfter(msg);
  } else { // must insert in the first position
    if (this->isOlder(msg->time, this->lastTime)) {
      msg->warning = true;
    }
    it->reset();
    it->insertBefore(msg);
  }
  delete it;
}

log_msg_t*
TimeBuffer::get(log_time_t minAge)
{
  log_msg_t* msg = NULL;
  MsgLinkedList::Iterator* it = this->msgList->getIterator();
  if (it->hasCurrent()) {
    // check the age limit
    if (this->isOlder(minAge, it->getCurrentRef()->time)) {
      msg = it->removeAndGetCurrent(); // get and delete the current element
      this->lastTime = msg->time;
    }
  }
  delete it;
  return msg;
}

// true if t2 older than t1
bool
TimeBuffer::isOlder(log_time_t t1, log_time_t t2) const
{
  // important not to use <=
  return ((t2.sec < t1.sec)
    || ((t2.sec == t1.sec) && (t2.msec < t1.msec)));
}

