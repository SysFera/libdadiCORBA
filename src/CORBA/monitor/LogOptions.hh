/****************************************************************************/
/* A static class for having access to some options from everywhere         */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.1  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/LogOptions due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ***************************************************************************/

#ifndef _LOGOPTIONS_HH_
#define _LOGOPTIONS_HH_

class LogOptions {
public:
  static unsigned long CORETHREAD_SLEEP_TIME_SEC;
  static unsigned long CORETHREAD_SLEEP_TIME_NSEC;
  static unsigned long CORETHREAD_MINAGE_TIME_SEC;
  static unsigned long CORETHREAD_MINAGE_TIME_MSEC;
  static unsigned long ALIVECHECKTHREAD_SLEEP_TIME_SEC;
  static unsigned long ALIVECHECKTHREAD_SLEEP_TIME_NSEC;
  static unsigned long ALIVECHECKTHREAD_DEAD_TIME_SEC;
  static unsigned long ALIVECHECKTHREAD_DEAD_TIME_MSEC;
  static unsigned long SENDTHREAD_SLEEP_TIME_SEC;
  static unsigned long SENDTHREAD_SLEEP_TIME_NSEC;
};

#endif
