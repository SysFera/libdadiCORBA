/****************************************************************************/
/* Log debug utils header                                                  */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Philippe COMBES (Philippe.Combes@ens-lyon.fr)                       */
/*    - Frederic LOMBARD (Frederic.Lombard@lifc.univ-fcomte.fr)             */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/

#ifndef _DEBUG_HH_
#define _DEBUG_HH_

#include <iostream>
using namespace std;

#include <omniconfig.h>
#include <omnithread.h>
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include <cmath> /* include used for the definition of HUGE_VAL*/


/** The trace level. */
extern unsigned int TRACE_LEVEL;
/** mutex used by the debug library to share the stderr and stdout */
extern omni_mutex debug_log_mutex ;

/**
 * Various values for the trace level
 */
#define NO_TRACE            0
#define TRACE_ERR_AND_WARN  1
#define TRACE_MAIN_STEPS    2
#define TRACE_ALL_STEPS     5
#define TRACE_STRUCTURES   10
#define TRACE_MAX_VALUE    TRACE_STRUCTURES
#define TRACE_DEFAULT      TRACE_MAIN_STEPS

/**
 * Definition of the EXIT_FUNCTION when not yet defined.
 * You must place the EXIT_FUNCTION's definition before any
 * preprocessing inclusion for this to work.
 */
#ifndef EXIT_FUNCTION
#define EXIT_FUNCTION cout << "DEBUG WARNING: EXIT_FUNCTION undeclared !" << endl
#endif


/**
 * Always useful
 */
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif /* MIN */
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif  /* MAX */

/**
 * Print a the name of the file and the line number where this macro
 * is put to stdout.
 */
#define BIP printf("bip - " __FILE__ " %i\n", __LINE__)

/**
 * Error message - return with return_value.
 */
#define ERROR(formatted_msg,return_value) {                     \
    if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {          \
      debug_log_mutex.lock() ;                                  \
      cerr << "LOG ERROR: " << formatted_msg << "." << endl;    \
      debug_log_mutex.unlock() ;                                \
    }                                                           \
    return return_value ; }

#define ERROR_EXIT(formatted_msg) {                             \
    if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {          \
      debug_log_mutex.lock() ;                                  \
      cerr << "LOG ERROR: " << formatted_msg << "." << endl;    \
      debug_log_mutex.unlock() ;                                \
    }                                                           \
    exit(1) ; }

#define INTERNAL_ERROR_EXIT(formatted_msg) {                    \
    if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {          \
      debug_log_mutex.lock() ;                                  \
      cerr << "LOG ERROR: " << formatted_msg << "." << endl;    \
      debug_log_mutex.unlock() ;                                \
    }                                                           \
    exit(1) ; }

/**
 * Warning message.
 */
#define WARNING(formatted_msg)                                  \
  if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {            \
    debug_log_mutex.lock() ;                                    \
    cerr << "LOG WARNING: " << formatted_msg << "." << endl;    \
    debug_log_mutex.unlock() ; }


/**
 * Internal Error message - exit with exit_value.
 */
#define INTERNAL_ERROR(formatted_msg,exit_value)                        \
  if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {                    \
    debug_log_mutex.lock() ;                                            \
    cerr << "LOG INTERNAL ERROR: " << formatted_msg << "." << endl <<   \
      "Please send bug report to diet-usr@ens-lyon.fr" << endl ;        \
    debug_log_mutex.unlock() ; }                                        \
  EXIT_FUNCTION;                                                        \
  exit(exit_value)

/**
 * Internal Warning message.
 */
#define INTERNAL_WARNING(formatted_msg)                                 \
  if ((int)TRACE_LEVEL >= (int)TRACE_ERR_AND_WARN) {                    \
    debug_log_mutex.lock() ;                                            \
    cerr << "LOG INTERNAL WARNING: " << formatted_msg << "." << endl << \
      "This is not a fatal bug, but please send a report "              \
      "to diet-dev@ens-lyon.fr" << endl                           ;     \
    debug_log_mutex.unlock() ; }


// DEBUG pause: insert a pause of duration <s>+<us>E-6 seconds
#define PAUSE(s,us)                             \
  {                                             \
    struct timeval tv;                          \
    tv.tv_sec  = s;                             \
    tv.tv_usec = us;                            \
    select(0, NULL, NULL, NULL, &tv);           \
  }


// DEBUG trace: print "function(formatted_text)\n", following the iostream
// format. First argument is the minimum TRACE_LEVEL for the line to be printed.
#define TRACE_FUNCTION(level,formatted_text)                            \
  if ((int)TRACE_LEVEL >= (int)(level)) {                               \
    debug_log_mutex.lock() ;                                            \
    cout << __FUNCTION__ << '(' << formatted_text << ")" << endl;       \
    debug_log_mutex.unlock() ; }

// DEBUG trace: print formatted_text following the iostream format (no '\n'
// added). First argument is the minimum TRACE_LEVEL for the line to be printed.
#define TRACE_TEXT(level,formatted_text)        \
  if ((int)TRACE_LEVEL >= (int)(level)) {       \
    debug_log_mutex.lock() ;                    \
    cout << formatted_text ;                    \
    debug_log_mutex.unlock() ; }

// DEBUG trace: print "file:line: formatted_text", following the iostream format
// (no '\n' added). First argument is the minimum TRACE_LEVEL for the line to be
// printed.
#define TRACE_TEXT_POS(level,formatted_text)                            \
  if ((int)TRACE_LEVEL >= (int)(level)) {                               \
    debug_log_mutex.lock() ;                                            \
    cout << __FILE__ << ':' << __LINE__ << ": " << formatted_text;      \
    debug_log_mutex.unlock() ; }

// DEBUG trace: print "time: formatted_text", following the iostream format (no
// '\n' added). First argument is the minimum TRACE_LEVEL for the line to be
// printed.
#define TRACE_TIME(level,formatted_text)                                \
  if ((int)TRACE_LEVEL >= (int)(level)) {                               \
    struct timeval tv;                                                  \
    debug_log_mutex.lock() ;                                            \
    gettimeofday(&tv, NULL);                                            \
    printf("%10ld.%06ld: ", (unsigned long)tv.tv_sec, (unsigned long)tv.tv_usec); \
    cout << formatted_text;                                             \
    debug_log_mutex.unlock() ;                                          \
  }

// DEBUG trace: print variable name and value
#define TRACE_VAR(var) {                                        \
    debug_log_mutex.lock() ;                                    \
    TRACE_TEXT_POS(NO_TRACE, #var << " = " << (var) << endl) ;  \
    debug_log_mutex.unlock() ; }



#endif // _DEBUG_HH_
