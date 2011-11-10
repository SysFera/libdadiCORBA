/****************************************************************************/
/* ToolMsgReceiverImpl.java class                                           */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: ToolMsgReceiverImpl.java,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: ToolMsgReceiverImpl.java,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 * Revision 1.1  2003/11/12 21:23:48  cpontvie
 * Add a structure for the LCT and LTB. Add a new ORBTools class for Java. Make
 * servant for Java from the idls. The 'lib' directory contains static C++
 * libraries and Java jar files.
 *
  ****************************************************************************/

public class ToolMsgReceiverImpl extends ToolMsgReceiverPOA
{
  public
  ToolMsgReceiverImpl(LogToolBase LTB) {
    this.LTB = LTB;
  }

  public
  void
  sendMsg(log_msg_t[] msgBuf) {
    for (int i = 0 ; i < msgBuf.length ; i++) {
      this.LTB.sendMsg(msgBuf[i]);
    }
  }

  private
  LogToolBase LTB;
}
