/****************************************************************************/
/* defines a list that holds all information on componentes connected to    */
/* the new Logservice                                                       */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ComponentList.hh,v 1.3 2007/09/03 06:33:24 bdepardo Exp $
 * $Log: ComponentList.hh,v $
 * Revision 1.3  2007/09/03 06:33:24  bdepardo
 * Removed IOR, it was useless.
 *
 * Revision 1.2  2007/08/31 16:41:17  bdepardo
 * When trying to add a new component, we check if the name of the component exists and if the component is reachable
 * - it the name already exists:
 *    - if the component is reachable, then we do not connect the new component
 *    - else we consider that the component is lost, and we delete the old component ant add the new one
 * - else add the component
 *
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 ****************************************************************************/

#ifndef _COMPONENTLIST_HH_
#define _COMPONENTLIST_HH_

#include "utils/FullLinkedList.hh"
#include "LogComponent.hh"

/**
 * All information that is stored for each client.
 * (In the Moment just the Proxy to make calls to client)
 */
struct ComponentElement {
  ComponentConfigurator_var componentConfigurator;
  CORBA::String_var componentName;
};

/**
 * Define a ComponentList for the Componentelements.
 * No Elements should be changed without
 * having a writeLock(Iterator) on the whole
 * List.
 */
typedef FullLinkedList<ComponentElement> ComponentList;

// Eventually define a function that takes an iterator
// and a componentName as input and sets the iterators
// currentElement to the component with the given name ?

#endif
