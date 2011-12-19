/**
 * @file LocalTime.hh
 *
 * @brief  For getting the current local time in ms since Jan. 1 1970
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

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

