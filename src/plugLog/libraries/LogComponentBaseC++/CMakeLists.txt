INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}         # For e.g. ORBTools.hh
  ${PROJECT_SOURCE_DIR}/monitor
  ${PROJECT_BINARY_DIR}/idl
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
#SET_TARGET_PROPERTIES(LogServiceComponentBase PROPERTIES VERSION ${LOGSERVICE_VERSION})

TARGET_LINK_LIBRARIES( LogServiceComponentBase
  dadiCORBAlog
  ${OMNIORB4_LIBRARIES}
)
INSTALL( TARGETS LogServiceComponentBase DESTINATION lib )
include(CPack)