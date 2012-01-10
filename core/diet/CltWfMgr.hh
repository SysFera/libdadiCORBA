
/* Forwarder part. */
class CltWfMgrFwdr : public POA_CltManFwdr,
public PortableServer::RefCountServantBase {
public:
  CltWfMgrFwdr(Forwarder_ptr fwdr, const char *objName);

  virtual CORBA::Long
  execNodeOnSed(const char *node_id, const char *dag_id,
                const char *sed, const CORBA::ULong reqID,
                corba_estimation_t &ev);

  virtual CORBA::Long
  execNode(const char *node_id, const char *dag_id);

  virtual char *
  release(const char *dag_id, bool successful);

  virtual CORBA::Long
  ping();

private:
  Forwarder_ptr forwarder;
  char *objName;
};

