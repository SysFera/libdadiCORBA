INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${LOGSERVICE_SOURCE_DIR}/src/utils         # For e.g. ORBTools.hh
  ${LOGSERVICE_BINARY_DIR}/src/idl
)

ADD_LIBRARY( LogServiceComponentBase
  LogComponentBase.cc
  ComponentConfigurator_impl.cc
  PingThread.cc
  FlushBufferThread.cc
  )
ADD_DEPENDENCIES( LogServiceComponentBase
  LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES
)
SET_TARGET_PROPERTIES(LogServiceComponentBase PROPERTIES VERSION ${LOGSERVICE_VERSION})

TARGET_LINK_LIBRARIES( LogServiceComponentBase
  LogForwarderUtils
  ${OMNIORB4_LIBRARIES}
)
INSTALL( TARGETS LogServiceComponentBase DESTINATION lib )
include(CPack)