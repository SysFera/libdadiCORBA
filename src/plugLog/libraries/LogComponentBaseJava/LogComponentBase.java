/****************************************************************************/
/* LogComponentBase class                                                   */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: LogComponentBase.java,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: LogComponentBase.java,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 * Revision 1.6  2003/12/12 19:36:13  cpontvie
 * New version of ORBTools (using the POA bidir in C++) and fully handle the
 * servants activation.
 *
 * Revision 1.5  2003/11/21 11:02:27  cpontvie
 * Catch more exceptions, that allow the component/tool to not crash if the
 * LogCentral crash.
 *
 * Revision 1.4  2003/11/20 09:15:54  cpontvie
 * Remove a debug message
 *
 * Revision 1.3  2003/11/19 17:51:44  cpontvie
 * New version of the LogComponentBase in Java that correct some bugs in the
 * initalization and in the sendMsg method.
 *
 * Revision 1.2  2003/11/18 18:41:14  cpontvie
 * Catch more exceptions
 *
 * Revision 1.1  2003/11/18 10:51:32  cpontvie
 * Add the LogComponentBase libraries
 *
 ****************************************************************************/

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.util.*;
import java.net.InetAddress;

public class LogComponentBase
{
  public
  LogComponentBase(boolean_out success, String[] args,
                   int tracelevel, int port) {
    success.value = false;
    this.partialInit = false;
    this.name = "";
    try {
      InetAddress ia[] = InetAddress.getAllByName(
        InetAddress.getLocalHost().getHostName()
      );
      if (ia.length != 0) {
        this.hostname = ia[0].getHostName();
      } else {
        this.hostname = "unknownhost";
      }
    } catch (java.net.UnknownHostException ex) {
      this.hostname = "unknownhost";
    }
    if (!ORBTools.init(args, tracelevel, port)) {
      System.out.println("Cannot initialize the ORB !");
      return;
    }
    CORBA_Object_out objout = new CORBA_Object_out();
    if (!ORBTools.findServant("LogService", "", "LogComponent", "", objout)) {
      System.out.println("Cannot contact the LogComponent servant of "
        + "LogService (not found) !");
      return;
    }
    this.LCCref = LogCentralComponentHelper.narrow(objout.value);
    this.CCimpl = new ComponentConfiguratorImpl(this);
    if (!ORBTools.activateServant(this.CCimpl, this.id)) {
      System.out.println("Cannot activate the ComponentConfigurator servant !");
      return;
    }
    if (!ORBTools.activatePOA()) {
      System.out.println("Cannot activate the POA !");
      return;
    }
    this.CCref = this.CCimpl._this();
    this.pingThread = null;
    this.flushBufferThread = null;
    try { // To make sure that the object is callable
      this.LCCref.test();
    } catch (Exception ex2) {
      System.out.println("Cannot contact the LogComponent servant of "
        + "LogService, even if it was found !");
      return;
    }
    this.currentTagList = new String[0];
    this.buffer = new log_msg_t[0];
    success.value = true;
  }

  public
  LogComponentBase(boolean_out success, org.omg.CORBA.ORB orb) {
    success.value = false;
    this.partialInit = true;
    this.name = "";
    try {
      InetAddress ia[] = InetAddress.getAllByName(
        InetAddress.getLocalHost().getHostName()
      );
      if (ia.length != 0) {
        this.hostname = ia[0].getHostName();
      } else {
        this.hostname = "unknownhost";
      }
    } catch (java.net.UnknownHostException ex) {
      this.hostname = "unknownhost";
    }
    ORBTools.orb = orb;

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
    if (!ORBTools.findServant("LogService", "", "LogComponent", "", objout)) {
      System.out.println("Cannot contact the LogComponent servant of "
        + "LogService (not found) !");
      return;
    }
    this.LCCref = LogCentralComponentHelper.narrow(objout.value);
    this.CCimpl = new ComponentConfiguratorImpl(this);
    if (!ORBTools.activateServant(this.CCimpl, this.id)) {
      System.out.println("Cannot activate the ComponentConfigurator servant !");
      return;
    }
    this.CCref = this.CCimpl._this();
    this.pingThread = null;
    this.flushBufferThread = null;
    try { // To make sure that the object is callable
      this.LCCref.test();
    } catch (Exception ex2) {
      System.out.println("Cannot contact the LogComponent servant of "
        + "LogService, even if it was found !");
      return;
    }
    this.currentTagList = new String[0];
    this.buffer = new log_msg_t[0];
    success.value = true;
  }

  // kind of destructor
  public
  void
  finalize() {
    if (this.pingThread != null) {
      this.pingThread.stopThread();
      this.pingThread = null;
    }
    if (this.flushBufferThread != null) {
      this.flushBufferThread.stopThread();
      this.flushBufferThread = null;
    }
    this.name = null;
    this.hostname = null;
    this.CCimpl = null;
    ORBTools.deactivateServant(this.id);
    if (!this.partialInit) {
      ORBTools.kill();
    }
  }

  public
  void
  setName(String name) {
    if (this.pingThread != null) {
      this.disconnect("change of name");
    }
    this.name = name;
  }

  public
  String
  getName() {
    return this.name;
  }

  public
  String
  getHostname() {
    return this.hostname;
  }

  public
  short
  connect(String message) {
    if (this.pingThread != null) {
      this.disconnect("for immediate reconnection");
    }
    short ret;
    StringHolder s = new StringHolder(this.name);
    long epoch = System.currentTimeMillis();
    long sec = epoch / 1000;
    long msec = epoch - (sec * 1000);
    log_time_t lt = new log_time_t(sec, (int)msec);
    String[] initialConfigValue = new String[0];
    tag_list_tHolder initialConfig = new tag_list_tHolder(initialConfigValue);
    try {
      ret = this.LCCref.connectComponent(
        s,
        this.hostname,
        message,
        this.CCref,
        lt,
        initialConfig
      );
    } catch (Exception ex) {
      return 1;
    }
    this.name = s.value;
    this.currentTagList = initialConfig.value;
    this.pingThread = new PingThread(this);
    this.flushBufferThread = new FlushBufferThread(this);
    this.pingThread.startThread();
    this.flushBufferThread.startThread();
    return ret;
  }

  public
  short
  disconnect(String message) {
    if (this.pingThread == null) {
      return LS_OK.value;
    }
    this.pingThread.stopThread();
    this.flushBufferThread.stopThread();
    this.pingThread = null;
    this.flushBufferThread = null;
    short ret;
    try {
      ret = this.LCCref.disconnectComponent(this.name, message);
    } catch (Exception ex) {
      ret = 1;
    }
    return 1;
  }

  public
  void
  sendMsg(String tag, String msg) {
    long epoch = System.currentTimeMillis();
    long sec = epoch / 1000;
    long msec = epoch - (sec * 1000);
    log_time_t lt = new log_time_t(sec, (int)msec);
    log_msg_t logmsg = new log_msg_t(
      this.name,
      lt,
      false,
      tag,
      msg
    );
    log_msg_t[] list = new log_msg_t[this.buffer.length + 1];
    System.arraycopy(this.buffer, 0, list, 0, this.buffer.length);
    list[this.buffer.length] = logmsg;
    this.buffer = list;
  }

  public
  String
  getHostnameOf(String componentName) {
    String ret;
    try {
      ret = this.LCCref.getHostnameOf(componentName);
    } catch (Exception ex) {
      ret = null;
    }
    return ret;
  }

  public
  boolean
  isLog(String tagname) {
    ArrayList list = new ArrayList(Arrays.asList(this.currentTagList));
    return list.contains(tagname);
  }

  public
  LogCentralComponent LCCref;

  public
  String[] currentTagList;

  public
  log_msg_t[] buffer;



  private
  ComponentConfiguratorImpl CCimpl;

  private
  ComponentConfigurator CCref;

  private
  boolean partialInit;

  private
  String name;

  private
  String hostname;

  private
  PingThread pingThread;

  private
  FlushBufferThread flushBufferThread;

  private
  byte[] id;
}
