/****************************************************************************/
/* Ping thread                                                              */
/*                                                                          */
/*  Author(s):                                                              */
/*    - Georg Hoesch (hoesch@in.tum.de)                                     */
/*    - Cyrille Pontvieux (cyrille.pontvieux@edu.univ-fcomte.fr)            */
/*                                                                          */
/* $LICENSE$                                                                */
/****************************************************************************/
/* $Id: PingThread.java,v 1.1 2004/01/09 11:07:12 ghoesch Exp $
 * $Log: PingThread.java,v $
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

public class PingThread extends Thread
{
  final
  long PINGTHREAD_SLEEP_TIME_MSEC = 1000;

  final
  int PINGTHREAD_SLEEP_TIME_NSEC = 0;

  final
  int PINGTHREAD_SYNCHRO_FREQUENCY = 60;



  public
  PingThread(LogComponentBase LCB) {
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
      System.out.println("Exception raised while runnning the Ping thread !");
    }
  }

  public
  void
  stopThread() {
    this.threadRunning = false;
    try {
      join();
    } catch (Exception ex) {
      System.out.println("Exception raised while stopping the Ping thread !");
    }
  }

  public
  void
  run() {
    int counter = 0;
    log_time_t lt;
    while (this.threadRunning) {
      try {
        this.LCB.LCCref.ping(this.name);
      } catch (Exception ex1) {
      }
      if (counter == PINGTHREAD_SYNCHRO_FREQUENCY) {
        lt = this.getLocalTime();
        try {
          this.LCB.LCCref.synchronize(this.name, lt);
        } catch (Exception ex2) {
        }
        counter = 0;
      }
      counter++;
      try {
        sleep(PINGTHREAD_SLEEP_TIME_MSEC,
              PINGTHREAD_SLEEP_TIME_NSEC);
      } catch (Exception ex3) {
        System.out.println("Sleep problem in Ping thread");
      }
    }
  }

  private
  log_time_t
  getLocalTime() {
    long epoch = System.currentTimeMillis();
    long sec = epoch / 1000;
    long msec = epoch - (sec * 1000);
    log_time_t ret = new log_time_t(sec, (int)msec);
    return ret;
  }

  private
  LogComponentBase LCB;

  private
  String name;

  private
  boolean threadRunning;

}
