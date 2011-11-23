/****************************************************************************/
/* A static class for having access to some options from everywhere         */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogOptions.cc,v 1.1 2010/12/08 11:37:54 kcoulomb Exp $
 * $Log: LogOptions.cc,v $
 * Revision 1.1  2010/12/08 11:37:54  kcoulomb
 * Refix the static library problem.
 * Renamed the monitor/Options to monitor/LogOptions due to conflict creating the static lib (2 files called Options)
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#include "LogOptions.hh"

/**
 * Very weired main.
 * Just initialize all static variables.
 *
 * We need the .cc to get an object file, and we need the .o to prevent
 * multiple definitions of the static variables in other object files.
 */
long unsigned int LogOptions::CORETHREAD_SLEEP_TIME_SEC        = 0;
long unsigned int LogOptions::CORETHREAD_SLEEP_TIME_NSEC       = 1000000;
long unsigned int LogOptions::CORETHREAD_MINAGE_TIME_SEC       = 0;
long unsigned int LogOptions::CORETHREAD_MINAGE_TIME_MSEC      = 200;
long unsigned int LogOptions::ALIVECHECKTHREAD_SLEEP_TIME_SEC  = 3;
long unsigned int LogOptions::ALIVECHECKTHREAD_SLEEP_TIME_NSEC = 0;
long unsigned int LogOptions::ALIVECHECKTHREAD_DEAD_TIME_SEC   = 60;
long unsigned int LogOptions::ALIVECHECKTHREAD_DEAD_TIME_MSEC  = 0;
long unsigned int LogOptions::SENDTHREAD_SLEEP_TIME_SEC        = 0;
long unsigned int LogOptions::SENDTHREAD_SLEEP_TIME_NSEC       = 1000000;


