/****************************************************************************/
/* FlushBuffer thread                                                       */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: FlushBufferThread.java,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: FlushBufferThread.java,v $
 * Revision 1.1  2004/01/09 11:07:12  ghoesch
 * Restructured the whole LogService source tree.
 * Added autotools make process. Cleaned up code.
 * Removed some testers. Ready to release.
 *
 * Revision 1.4  2003/11/21 11:02:27  cpontvie
 * Catch more exceptions, that allow the component/tool to not crash if the
 * LogCentral crash.
 *
 * Revision 1.3  2003/11/20 09:18:16  cpontvie
 * Remove some debug messages
 *
 * Revision 1.2  2003/11/19 17:50:32  cpontvie
 * Now the threads are running properly (non blocant)
 *
 * Revision 1.1  2003/11/18 10:51:32  cpontvie
 * Add the LogComponentBase libraries
 *
  ****************************************************************************/

import java.lang.Thread;
import java.util.*;

public class FlushBufferThread extends Thread
{
  final
  long FLUSHBUFFERTHREAD_SLEEP_TIME_MSEC = 50;

  final
  int FLUSHBUFFERTHREAD_SLEEP_TIME_NSEC = 0;



  public
  FlushBufferThread(LogComponentBase LCB) {
    this.LCB = LCB;
    this.name = this.LCB.getName();
    this.threadRunning = false;
  }

  public
  void
  startThread() {
    this.threadRunning = true;
    try {
      this.start();
    } catch (Exception ex) {
      System.out.println("Exception raised while runnning the FlushBuffer "
        + "thread !");
    }
  }

  public
  void
  stopThread() {
    this.threadRunning = false;
    try {
      join();
    } catch (Exception ex) {
      System.out.println("Exception raised while stopping the FlushBuffer "
        + "thread !");
    }
  }

  public
  void
  run() {
    while (this.threadRunning) {
      try {
        this.LCB.LCCref.sendBuffer(this.LCB.buffer);
        this.LCB.buffer = new log_msg_t[0];
      } catch (Exception ex1) {
      }
      try {
        sleep(FLUSHBUFFERTHREAD_SLEEP_TIME_MSEC,
              FLUSHBUFFERTHREAD_SLEEP_TIME_NSEC);
      } catch (Exception ex2) {
        System.out.println("Sleep problem in FlushBuffer thread");
      }
    }
  }

  private
  LogComponentBase LCB;

  private
  String name;

  private
  boolean threadRunning;

}
