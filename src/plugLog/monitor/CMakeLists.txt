######### First the LogServiceMonitor library:
INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${DADICORBA_INCLUDE_DIR}/idl
  ${DADICORBALOG_BINARY_DIR}/idl
  ${DADICORBALOG_SOURCE_DIR}/monitor
  ${DADICORBALOG_SOURCE_DIR}
)

#message("link : ")

link_directories(
  ${DADICORBA_SOURCE_DIR}/build/lib
)


add_library(LogServiceMonitor SHARED
  ${DADICORBALOG_SOURCE_DIR}/monitor/CoreThread.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/LogCentralComponent_impl.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/LogCentralTool_impl.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/ReadConfig.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/SendThread.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/SimpleFilterManager.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/StateManager.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/TimeBuffer.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/LogOptions.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/LocalTime.cc
  ${DADICORBALOG_SOURCE_DIR}/monitor/ORBTools.cc
)

install (TARGETS LogServiceMonitor DESTINATION lib)

set (binary
  ${binary}
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/CoreThread.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/LogCentralComponent_impl.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/LogCentralTool_impl.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/ReadConfig.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/SendThread.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/SimpleFilterManager.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/StateManager.cc.o
  ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogServiceMonitor.dir/TimeBuffer.cc.o
  PARENT_SCOPE
  )

add_dependencies (LogServiceMonitor
  DADICORBALOG_TARGET_IDL_GENERATED_HEADER_FILES
)

TARGET_LINK_LIBRARIES( LogServiceMonitor
  ${DADICORBA_LIB}
  dadiCORBAlog
  ${OMNIORB4_LIBRARIES}
  pthread
  )


######### Then the LogCentral executable:
ADD_EXECUTABLE( LogCentral LogCentral.cc )
TARGET_LINK_LIBRARIES( LogCentral
  dadiCORBAlog
  ${DADICORBA_LIB}
  ${OMNIORB4_LIBRARIES}
  LogServiceMonitor
  )
INSTALL( TARGETS LogCentral DESTINATION bin )
