/****************************************************************************/
/* LogToolBase class                                                        */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogToolBase.java,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: LogToolBase.java,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 * Revision 1.7  2003/12/12 19:36:13  cpontvie
 * New version of ORBTools (using the POA bidir in C++) and fully handle the
 * servants activation.
 *
 * Revision 1.6  2003/11/21 11:02:27  cpontvie
 * Catch more exceptions, that allow the component/tool to not crash if the
 * LogCentral crash.
 *
 * Revision 1.5  2003/11/19 18:54:47  cpontvie
 * Fix some bugs (out args)
 *
 * Revision 1.4  2003/11/18 10:53:01  cpontvie
 * Change the constructor
 *
 * Revision 1.3  2003/11/17 15:26:15  cpontvie
 * Modify constructors to fit the new ORBTools class
 *
 * Revision 1.2  2003/11/17 14:21:52  cpontvie
 * Implementation done.
 *
 * Revision 1.1  2003/11/12 21:23:48  cpontvie
 * Add a structure for the LCT and LTB. Add a new ORBTools class for Java. Make
 * servant for Java from the idls. The 'lib' directory contains static C++
 * libraries and Java jar files.
 *
  ****************************************************************************/

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.util.Properties;

public class LogToolBase
{
  public
  LogToolBase(boolean_out success, String[] args,
              int tracelevel, int port) {
    success.value = false;
    this.partialInit = false;
    this.name = "";
    if (!ORBTools.init(args, tracelevel, port)) {
      return;
    }
    CORBA_Object_out objout = new CORBA_Object_out();
    if (!ORBTools.findServant("LogService", "", "LogTool", "", objout)) {
      System.out.println("Cannot contact the LogTool servant of "
        + "LogService !");
      return;
    }
    this.LCTref = LogCentralToolHelper.narrow(objout.value);
    this.TMRimpl = new ToolMsgReceiverImpl(this);
    if (!ORBTools.activateServant(this.TMRimpl, this.id)) {
      System.out.println("Cannot activate the ToolMsgReceiver servant !");
      return;
    }
    if (!ORBTools.activatePOA()) {
      return;
    }
    this.TMRref = this.TMRimpl._this();
    success.value = true;
  }

  public
  LogToolBase(boolean_out success, org.omg.CORBA.ORB orb) {
    success.value = false;
    this.partialInit = true;
    this.name = "";

    //find the POA
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("RootPOA");
      try {
        ORBTools.poa = org.omg.PortableServer.POAHelper.narrow(objRef);
        ORBTools.pman = ORBTools.poa.the_POAManager();
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to find the POA.");
        return;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return;
    }
    // Activate the POA if it is not already activate
    ORBTools.activatePOA();

    CORBA_Object_out objout = new CORBA_Object_out();
    if (!ORBTools.findServant("LogService", "", "LogTool", "", objout)) {
      System.out.println("Cannot contact the LogTool servant of "
        + "LogService !");
      return;
    }
    this.LCTref = LogCentralToolHelper.narrow(objout.value);
    this.TMRimpl = new ToolMsgReceiverImpl(this);
    if (!ORBTools.activateServant(this.TMRimpl, this.id)) {
      System.out.println("Cannot activate the ToolMsgReceiver servant !");
      return;
    }
    this.TMRref = this.TMRimpl._this();
    success.value = true;
  }

  // kind of destructor
  public
  void
  finalize() {
    this.TMRimpl = null;
    this.name = null;
    ORBTools.deactivateServant(this.id);
    if (!this.partialInit) {
      ORBTools.kill();
    }
  }

  public
  void
  setName(String name) {
    this.name = name;
  }

  public
  String
  getName() {
    return this.name;
  }

  public
  short
  connect() {
    short ret;
    StringHolder s = new StringHolder(this.name);
    try {
      ret = this.LCTref.connectTool(s, this.TMRref);
    } catch (Exception ex) {
      ret = 1;
    }
    this.name = s.value;
    return ret;
  }

  public
  short
  disconnect() {
    short ret;
    try {
      ret = this.LCTref.disconnectTool(this.name);
    } catch (Exception ex) {
      ret = 1;
    }
    return ret;
  }

  public
  short
  addFilter(filter_t filter) {
    short ret;
    try {
      ret = this.LCTref.addFilter(this.name, filter);
    } catch (Exception ex) {
      ret = 1;
    }
    return ret;
  }

  public
  short
  removeFilter(String filterName) {
    short ret;
    try {
      ret = this.LCTref.removeFilter(this.name, filterName);
    } catch (Exception ex) {
      ret = 1;
    }
    return ret;
  }

  public
  short
  flushAllFilters() {
    short ret;
    try {
      ret = this.LCTref.flushAllFilters(this.name);
    } catch (Exception ex) {
      ret = 1;
    }
    return ret;
  }

  public
  String[]
  getDefinedTags() {
    String[] ret;
    try {
      ret = this.LCTref.getDefinedTags();
    } catch (Exception ex) {
      ret = null;
    }
    return ret;
  }

  public
  String[]
  getDefinedComponents() {
    String[] ret;
    try {
      ret = this.LCTref.getDefinedComponents();
    } catch (Exception ex) {
      ret = null;
    }
    return ret;
  }

  public
  void
  sendMsg(log_msg_t msg) {
  }

  private
  LogCentralTool LCTref;

  private
  ToolMsgReceiverImpl TMRimpl;

  private
  ToolMsgReceiver TMRref;

  private
  boolean partialInit;

  private
  String name;

  private
  byte[] id;
}
