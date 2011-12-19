/**
 * @file ComponentList.hh
 *
 * @briefdefines a list that holds all information on componentes connected to
 * the new Logservice
 *
 * @author - Gaël Le Mahec (gael.le.mahec@ens-lyon.fr)
 *         - Kevin Coulomb (kevin.coulomb@sysfera.com)
 *         - Georg Hoesch (hoesch@in.tum.de)
 *         - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)
 *
 * @section Licence
 *   |LICENSE|
 */

#ifndef _COMPONENTLIST_HH_
#define _COMPONENTLIST_HH_

#include "utils/FullLinkedList.hh"
#include "LogComponent.hh"

/**
 * @brief All information that is stored for each client.
 * (In the Moment just the Proxy to make calls to client)
 */
struct ComponentElement {
  ComponentConfigurator_var componentConfigurator;
  CORBA::String_var componentName;
};

/**
 * @brief Define a ComponentList for the Componentelements.
 * No Elements should be changed without
 * having a writeLock(Iterator) on the whole
 * List.
 */
typedef FullLinkedList<ComponentElement> ComponentList;

// Eventually define a function that takes an iterator
// and a componentName as input and sets the iterators
// currentElement to the component with the given name ?

#endif
