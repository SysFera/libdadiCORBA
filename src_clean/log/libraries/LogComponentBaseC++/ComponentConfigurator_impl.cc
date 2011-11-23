/****************************************************************************/
/* ComponentConfigurator_impl implementation class                          */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ComponentConfigurator_impl.cc,v 1.1 2010/11/10 03:02:28 kcoulomb Exp $
 * $Log: ComponentConfigurator_impl.cc,v $
 * Revision 1.1  2010/11/10 03:02:28  kcoulomb
 * Add missing files
 *
 * Revision 1.2  2007/08/31 16:41:16  bdepardo
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

#include "LogComponentBase.hh"
#include "ORBTools.hh"
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
using namespace std;


ComponentConfigurator_impl::ComponentConfigurator_impl(LogComponentBase* LCB)
{
  this->LCB = LCB;
}

ComponentConfigurator_impl::~ComponentConfigurator_impl()
{
}

void
ComponentConfigurator_impl::setTagFilter(const tag_list_t& tagList)
{
  this->LCB->currentTagList = tagList;
}

void
ComponentConfigurator_impl::addTagFilter(const tag_list_t& tagList)
{
  CORBA::ULong l1 = this->LCB->currentTagList.length();
  CORBA::ULong l2 = tagList.length();
  bool found = false;
  for (CORBA::ULong i = 0 ; i < l2 ; i++) {
    found = false;
    for (CORBA::ULong j = 0 ; j < l1 + i ; j++) {
      if (strcmp(tagList[i], this->LCB->currentTagList[j]) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      this->LCB->currentTagList.length(l1 + i + 1);
      this->LCB->currentTagList[l1 + i] = tagList[i];
    }
  }
}

void
ComponentConfigurator_impl::removeTagFilter(const tag_list_t& tagList)
{
  tag_list_t oldtaglist = this->LCB->currentTagList;
  this->LCB->currentTagList.length(0);
  CORBA::ULong l1 = oldtaglist.length();
  CORBA::ULong l2 = tagList.length();
  CORBA::ULong l3 = 0;
  bool found = false;
  for (CORBA::ULong i = 0 ; i < l1 ; i++) {
    found = false;
    for (CORBA::ULong j = 0 ; j < l2 ; j++) {
      if (strcmp(oldtaglist[i], tagList[j]) == 0) {
        found = true;
        break;
      }
    }
    if (!found) {
      this->LCB->currentTagList.length(l3 + 1);
      this->LCB->currentTagList[l3++] = oldtaglist[i];
    }
  }
}

void
ComponentConfigurator_impl::test()
{
  return;
}
