/**
 * @file LogOptions.cc
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


