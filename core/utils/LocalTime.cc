/**
 * @file LocalTime.cc
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

#include "LocalTime.hh"

log_time_t
getLocalTime() {
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  log_time_t ret;
  ret.sec = tv.tv_sec;
  ret.msec = tv.tv_usec / 1000;
  return ret;
}
