
#ifndef _FWDRFACTORY_HH_
#define _FWDRFACTORY_HH_

enum fwd_t {
  DIET = 0,
  LOG,
  DAGDA
}fwd_t;


class FwdrFactory {
public :
  /**
   * \brief Destructor
   */
  ~FwdrFactory();
  /**
   * \brief Get the current isntance
   */
  FwdrFactory*
  getInstance();
  /**
   * \brief To get the corresponding forwarder
   * \param id: The id of the forwarder to get
   */
  Forwarder*
  getFwdr(fwd_t id);
protected:
private:
  /**
   * \brief the only instance
   */
  static FwdrFactory* mfac;
  /**
   * \brief Constructor
   */
  FwdrFactory();


};


#endif
