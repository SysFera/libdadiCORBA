/****************************************************************************/
/* ComponentConfiguratorImpl.java class                                     */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ComponentConfiguratorImpl.java,v 1.2 2007/08/31 16:41:17 bdepardo Exp $
 * $Log: ComponentConfiguratorImpl.java,v $
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
 * Revision 1.1  2003/11/18 10:51:32  cpontvie
 * Add the LogComponentBase libraries
 *
  ****************************************************************************/

import java.util.*;

public class ComponentConfiguratorImpl extends ComponentConfiguratorPOA
{
  public
  ComponentConfiguratorImpl(LogComponentBase LCB) {
    this.LCB = LCB;
  }

  public
  void
  setTagFilter(String[] tagList) {
    this.LCB.currentTagList = tagList;
  }

  public
  void
  addTagFilter(String[] tagList) {
    HashSet list = new HashSet(Arrays.asList(this.LCB.currentTagList));
    for (int i = 0 ; i < tagList.length ; i++) {
      list.add(tagList[i]);
    }
    this.LCB.currentTagList = (String[])list.toArray();
  }

  public
  void
  removeTagFilter(String[] tagList) {
    HashSet list = new HashSet(Arrays.asList(this.LCB.currentTagList));
    for (int i = 0 ; i < tagList.length ; i++) {
      list.remove(tagList[i]);
    }
    this.LCB.currentTagList = (String[])list.toArray();
  }

  public
  void
  test() {
  }

  private
  LogComponentBase LCB;
}
