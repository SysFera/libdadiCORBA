/**
 * @file LogOptions.hh
 *
 * @brief A static class for having access to some options from everywhere
 *
 * @author - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _LOGOPTIONS_HH_
#define _LOGOPTIONS_HH_

/**
 * @brief Class that defines some useful constant for the log service
 * @class LogOptions
 */
class LogOptions
{
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
