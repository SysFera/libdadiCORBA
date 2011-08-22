######### First the LogServiceMonitor library:
INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${BOBLOG_BINARY_DIR}/idl
  ${BOBLOG_SOURCE_DIR}
)

#message("link : ")

link_directories(
  ${BOB_SOURCE_DIR}/build/lib
)


add_library(LogServiceMonitor SHARED
  ${BOBLOG_SOURCE_DIR}/monitor/CoreThread.cc
  ${BOBLOG_SOURCE_DIR}/monitor/LogCentralComponent_impl.cc
  ${BOBLOG_SOURCE_DIR}/monitor/LogCentralTool_impl.cc
  ${BOBLOG_SOURCE_DIR}/monitor/ReadConfig.cc
  ${BOBLOG_SOURCE_DIR}/monitor/SendThread.cc
  ${BOBLOG_SOURCE_DIR}/monitor/SimpleFilterManager.cc
  ${BOBLOG_SOURCE_DIR}/monitor/StateManager.cc
  ${BOBLOG_SOURCE_DIR}/monitor/TimeBuffer.cc
)

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
  BOBLOG_TARGET_IDL_GENERATED_HEADER_FILES
)

TARGET_LINK_LIBRARIES( LogServiceMonitor
#  LogServiceUtils
  Corba
  ${OMNIORB4_LIBRARIES}
)


######### Then the LogCentral executable:
ADD_EXECUTABLE( LogCentral LogCentral.cc )
TARGET_LINK_LIBRARIES( LogCentral
  LogForwarderUtils
  bob
  Corba
  LogServiceMonitor
)
INSTALL( TARGETS LogCentral DESTINATION bin )
