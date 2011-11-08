/****************************************************************************/
/* Log debug utils source code                                              */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Benjamin DEPARDON (benjamin.depardon@sysfera.com)                   */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/

#include "debug.hh"

/**
 * The trace level itself: it must be set by the Parsers.
 */
unsigned int TRACE_LEVEL = TRACE_DEFAULT;

// see debug.hh
omni_mutex debug_log_mutex ;
