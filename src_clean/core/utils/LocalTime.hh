/****************************************************************************/
/* For getting the current local time in ms since Jan. 1 1970               */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id$
 * $Log$
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/
#ifndef _LOCALTIME_HH_
#define _LOCALTIME_HH_

#include <sys/time.h>
#include "LogTypes.hh"

/**
 * Gets back the local time in the log_time_t format
 */
log_time_t
getLocalTime();

#endif

