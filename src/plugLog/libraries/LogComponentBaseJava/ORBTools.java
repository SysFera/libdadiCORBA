/****************************************************************************/
/* ORBTools class                                                           */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $@Id 
 * $@Log 
 *
  ****************************************************************************/

import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.util.Properties;

final public class ORBTools
{
  /**
   * Initialize the CORBA ORB.
   * See CORBA documentation for all possibles parameters.
   * @param args Array of parameters to pass to the ORB
   * @return true if the operation succeed
   */
  public static
  boolean
  init(String args[]) {
    try {
      orb = org.omg.CORBA.ORB.init(args, null);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    org.omg.CORBA.Object obj;
    try {
      obj = orb.resolve_initial_references("RootPOA");
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex) {
      orb = null;
      return false;
    }
    try {
      poa = POAHelper.narrow(obj);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    return true;
  }

  /**
   * Initialize the CORBA ORB.
   * @param args Array of parameters to pass to the ORB
   * @param tracelevel The tracelevel of the ORB (from 0 to 50)
   * @param port the port to use for listenning servants (0 = default random)
   * @return true if the operation succeed
   */
  public static
  boolean
  init(String args[], int tracelevel, int port) {
    String[] orbargs;
    if (port == 0) {
      orbargs = new String[args.length + 2];
      System.arraycopy(args, 0, orbargs, 0, args.length);
      orbargs[orbargs.length - 2] = "-ORBtraceLevel";
      orbargs[orbargs.length - 1] = new Integer(tracelevel).toString();
    } else {
      orbargs = new String[args.length + 4];
      System.arraycopy(args, 0, orbargs, 0, args.length);
      orbargs[orbargs.length - 4] = "-ORBtraceLevel";
      orbargs[orbargs.length - 3] = new Integer(tracelevel).toString();
      orbargs[orbargs.length - 2] = "-ORBendPoint";
      orbargs[orbargs.length - 1] = new Integer(port).toString();
    }
    try {
      orb = org.omg.CORBA.ORB.init(orbargs, null);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    org.omg.CORBA.Object obj;
    try {
      obj = orb.resolve_initial_references("RootPOA");
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex) {
      orb = null;
      return false;
    }
    try {
      poa = POAHelper.narrow(obj);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    return true;
  }

  /**
   * Initialize the CORBA ORB.
   * None argument is set, usefull for tests
   * @return true if the operation succeed
   */
  public static
  boolean
  init() {
    try {
      orb = org.omg.CORBA.ORB.init((String[])null, null);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    org.omg.CORBA.Object obj;
    try {
      obj = orb.resolve_initial_references("RootPOA");
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex) {
      orb = null;
      return false;
    }
    try {
      poa = POAHelper.narrow(obj);
    } catch(org.omg.CORBA.SystemException ex) {
      orb = null;
      return false;
    }
    return true;
  }

  /**
   * Register a servant to the Namming Service within a specific context and
   * name. Do not activate the servant.
   * @param contextName Name of the new context
   * @param contextKind Kind of the new context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The implementation of a servant (result of the
   * rootpoa.servant_to_reference function)
   * @return true if the operation succeed
   */
  public static
  boolean
  registerServant(final String contextName, final String contextKind,
                  final String name, final String kind,
                  org.omg.CORBA.Object objref) {
    if (orb == null) {
      return false;
    }
    if (objref == null) {
      return false;
    }
    NamingContext rootContext;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return false;
    }
    try {
      NameComponent[] context = new NameComponent[1];
      context[0] = new NameComponent(contextName, contextKind);
      NamingContext newcontext;
      try {
        newcontext = rootContext.bind_new_context(context);
      } catch(org.omg.CORBA.UserException ex2) {
        try {
          newcontext = NamingContextHelper.narrow(rootContext.resolve(context));
        } catch(org.omg.CORBA.UserException ex3) {
          System.out.println("Failed to narrow naming context.");
          return false;
        }
      }
      NameComponent[] servant = new NameComponent[1];
      servant[0] = new NameComponent(name, kind);
      try {
        newcontext.bind(servant, objref);
      } catch(org.omg.CORBA.UserException ex4) {
        try {
          newcontext.rebind(servant, objref);
        } catch(org.omg.CORBA.UserException ex5) {
          System.out.println("Failed to bind the name.");
          return false;
        }
      }
    } catch(org.omg.CORBA.COMM_FAILURE ex6) {
      System.out.println("Caught system exception COMM_FAILURE -- unable to "
        + "contact the naming service.");
      return false;
    } catch(org.omg.CORBA.SystemException ex7) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Register a servant to the Namming Service within the default context and
   * with a specific name. Do not activate the servant.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objref The implementation of a servant (result of the
   * rootpoa.servant_to_reference function)
   * @return true if the operation succeed
   */
  public static
  boolean
  registerServant(final String name, final String kind,
                  org.omg.CORBA.Object objref) {
    if (orb == null) {
      return false;
    }
    if (objref == null) {
      return false;
    }
    NamingContext rootContext;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return false;
    }
    try {
      NameComponent[] servant = new NameComponent[1];
      servant[0] = new NameComponent(name, kind);
      try {
        rootContext.bind(servant, objref);
      } catch(org.omg.CORBA.UserException ex4) {
        try {
          rootContext.rebind(servant, objref);
        } catch(org.omg.CORBA.UserException ex5) {
          System.out.println("Failed to bind the name.");
          return false;
        }
      }
    } catch(org.omg.CORBA.COMM_FAILURE ex6) {
      System.out.println("Caught system exception COMM_FAILURE -- unable to "
        + "contact the naming service.");
      return false;
    } catch(org.omg.CORBA.SystemException ex7) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Activate a servant.
   * After the IOR of the servant is known by the POA.
   * @param object The implementation of a servant
   * @param id the id correspounding to the activating object
   * @return true if the operation succeed
   */
  public static
  boolean
  activateServant(org.omg.PortableServer.Servant object,
                  byte[] id) {
    if (orb == null) {
      return false;
    }
    if (object == null) {
      return false;
    }
    try {
      if (poa != null) {
        id = poa.activate_object(object);
      } else {
        return false;
      }
    } catch(org.omg.CORBA.UserException ex) {
      return false;
    }
    return true;
  }

  /**
   * Deactivate a servant.
   * After this, the correspounding object cannot be called.
   * @param id the id correspounding to the activating object
   * @return true if the operation succeed
   */
  public static
  boolean
  deactivateServant(byte[] id)
  {
    if (orb == null) {
      return false;
    }
    try {
      if (poa != null) {
        poa.deactivate_object(id);
      } else {
        return false;
      }
    } catch(org.omg.CORBA.UserException ex) {
      return false;
    }
    return true;
  }

  /**
   * Activate the POA (Portable Object Adaptor).
   * After  all servants registered are accessible
   * @return true if the operation succeed
   */
  public static
  boolean
  activatePOA() {
    if (orb == null) {
      return false;
    }
    if (poa != null) {
      return true;
    }
    try {
      pman = poa.the_POAManager();
      pman.activate();
    } catch(org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
      return false;
    }
    return true;
  }

  /**
   * Deactivate the POA (Portable Object Adaptor).
   * After this, all servants registered are inaccessible
   * @return true if the operation succeed
   */
  static
  boolean
  deactivatePOA() {
    if (orb == null) {
      return false;
    }
    if (poa == null) {
      return false;
    }
    try {
      pman.deactivate(true, true);
    } catch(org.omg.PortableServer.POAManagerPackage.AdapterInactive ex) {
      return false;
    }
    poa.destroy(true, true);
    pman = null;
    poa = null;
    return true;
  }

  /**
   * Unregister a servant to the Namming Service within a specific context and
   * name.
   * @param contextName Name of the context
   * @param contextKind Kind of the context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @return true if the operation succeed
   */
  public static
  boolean
  unregisterServant(final String contextName, final String contextKind,
                    final String name, final String kind) {
    if (orb == null) {
      return false;
    }
    NamingContext rootContext, context;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return false;
    }
    NameComponent[] context2find = new NameComponent[1];
    context2find[0] = new NameComponent(contextName, contextKind);
    try {
      try {
        context = NamingContextHelper.narrow(
          rootContext.resolve(context2find));
      } catch(org.omg.CORBA.UserException ex2) {
        System.out.println("Cannot find servant.");
        return false;
      }
      NameComponent[] servant2find = new NameComponent[1];
      servant2find[0] = new NameComponent(name, kind);
      try {
        context.unbind(servant2find);
      } catch(org.omg.CORBA.UserException ex3) {
        System.out.println("Cannot unbind servant.");
        return false;
      }
    } catch(org.omg.CORBA.COMM_FAILURE ex4) {
      System.out.println("Caught system exception COMM_FAILURE -- unable to "
        + "contact the naming service.");
      return false;
    } catch(org.omg.CORBA.SystemException ex5) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Unregister a servant to the Namming Service within the default context and
   * with a specific name.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @return true if the operation succeed
   */
  public static
  boolean
  unregisterServant(final String name, final String kind) {
    if (orb == null) {
      return false;
    }
    NamingContext rootContext;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return false;
    }
    try {
      NameComponent[] servant2find = new NameComponent[1];
      servant2find[0] = new NameComponent(name, kind);
      try {
        rootContext.unbind(servant2find);
      } catch(org.omg.CORBA.UserException ex3) {
        System.out.println("Cannot unbind servant.");
        return false;
      }
    } catch(org.omg.CORBA.COMM_FAILURE ex4) {
      System.out.println("Caught system exception COMM_FAILURE -- unable to "
        + "contact the naming service.");
      return false;
    } catch(org.omg.CORBA.SystemException ex5) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Find a servant by asking the Namming Service with a specific context and
   * name.
   * @param contextName Name of the context
   * @param contextKind Kind of the context (extra information)
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objout The new servant found (the obj returned is in the class)
   * @return true if the operation succeed
   */
  public static
  boolean
  findServant(final String contextName, final String contextKind,
              final String name, final String kind,
              CORBA_Object_out objout) {
    objout.value = null;
    if (orb == null) {
      return false;
    }
    NamingContext rootContext;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      System.out.println("Failed to narrow the root naming context.");
      return false;
    }
    NameComponent nc1 = new NameComponent(contextName, contextKind);
    NameComponent nc2 = new NameComponent(name, kind);
    NameComponent path[] = {nc1, nc2};
    try {
      org.omg.CORBA.Object objref = rootContext.resolve(path);
      objout.value = objref;
    } catch(org.omg.CORBA.UserException ex2) {
      System.out.println("Cannot find the servant.");
      return false;
    } catch(org.omg.CORBA.SystemException ex3) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Find a servant by asking the Namming Service with the default context and
   * name.
   * @param name Name of the servant
   * @param kind Kind of the servant (extra information)
   * @param objout The new servant found (the obj returned is in the class)
   * @return true if the operation succeed
   */
  public static
  boolean
  findServant(final String name, final String kind,
              CORBA_Object_out objout) {
    objout.value = null;
    if (orb == null) {
      return false;
    }
    NamingContext rootContext;
    try {
      org.omg.CORBA.Object objRef =
        orb.resolve_initial_references("NameService");
      try {
        rootContext = NamingContextHelper.narrow(objRef);
      } catch(org.omg.CORBA.SystemException ex) {
        System.out.println("Failed to narrow the root naming context.");
        return false;
      }
    } catch(org.omg.CORBA.ORBPackage.InvalidName ex1) {
      return false;
    }
    NameComponent nc = new NameComponent(name, kind);
    NameComponent path[] = {nc};
    try {
      org.omg.CORBA.Object objref = rootContext.resolve(path);
      objout.value = objref;
    } catch(org.omg.CORBA.UserException ex2) {
      System.out.println("Cannot find the servant.");
      return false;
    } catch(org.omg.CORBA.SystemException ex3) {
      System.out.println("Caught a CORBA::SystemException while using the "
        + "naming service.");
      return false;
    }
    return true;
  }

  /**
   * Make the thread to listen for a incomming connection to a servant
   * @param stopString String that make the listen function to stop
   * @return true if the operation succeed
   */
  public static
  boolean
  listen(String stopString) {
    if (orb == null) {
      return false;
    }
    String c = "";
    java.io.BufferedReader inr =
      new java.io.BufferedReader(new java.io.InputStreamReader(System.in));
    while (c.equals("")) {
      if (orb.work_pending()) {
        orb.perform_work();
      }
      c ="";
      try {
        c = inr.readLine();
      } catch(Exception ex) {
        c = "";
      }
    }
    inr = null;
    return true;
  }

  /**
   * Destroy the ORB
   * @return true if the operation succeed
   */
  public static
  boolean
  kill() {
    if (orb == null) {
      return false;
    }
    if (poa != null) {
      deactivatePOA();
    }
    try {
      orb.shutdown(true);
    } catch(org.omg.CORBA.SystemException ex) {
      return false;
    }
    orb = null;
    poa = null;
    return true;
  }

  public static org.omg.CORBA.ORB orb = null;
  public static org.omg.PortableServer.POA poa = null;
  public static org.omg.PortableServer.POAManager pman = null;
}

