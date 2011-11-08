/****************************************************************************/
/* Header for the TimeBuffer class                                          */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
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
 ***************************************************************************/

#ifndef _TIMEBUFFER_HH_
#define _TIMEBUFFER_HH_

#include "LogTypes.hh"
#include "FullLinkedList.hh"

class TimeBuffer {
public:
  TimeBuffer();

  ~TimeBuffer();

  /**
   * Put a new message in the buffer
   * @param msg the new message to add
   */
  void
  put(log_msg_t* msg);

  /**
   * Get the older msg if its age is older than the minAge,
   * else get NULL
   * @param minAge the mininum age that messages must have to return one
   * @return a message or NULL
   */
  log_msg_t*
  get(log_time_t minAge);

private:
  typedef FullLinkedList<log_msg_t> MsgLinkedList;
  MsgLinkedList* msgList;
  log_time_t lastTime;

  /**
   * Compares the two log_time_t t1 and t2
   * @param t1 the first time to compare
   * @param t2 the time for comparing t1 to
   * @returns true if t2 is older than t1, else false
   */
  bool
  isOlder(log_time_t t1, log_time_t t2) const;
};
#endif

