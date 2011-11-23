INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${LOGS_SOURCE_DIR}
  ${FWD_INCLDE_DIR}
)

#########
INCLUDE_DIRECTORIES( ${LOGS_SOURCE_DIR}/libraries/LogToolBaseC++ )

ADD_EXECUTABLE( DIETtestTool DIETtestTool.cc )
TARGET_LINK_LIBRARIES( DIETtestTool
  ${OMNIORB4_LIBRARIES}
  pthread
  ${FWD_LIBRARY}
  LogCommon
  LogServiceToolBase
  ${DADI_LIBRARIES}
)
INSTALL( TARGETS DIETtestTool DESTINATION bin )

#########
INCLUDE_DIRECTORIES(
  ${LOGS_SOURCE_DIR}/libraries/LogComponentBaseC++
)
ADD_EXECUTABLE( testComponent testComponent.cc )
TARGET_LINK_LIBRARIES( testComponent
  ${OMNIORB4_LIBRARIES}
  pthread
  ${FWD_LIBRARY}
  LogCommon
  LogServiceComponentBase
  ${DADI_LIBRARIES}
)
INSTALL( TARGETS testComponent DESTINATION bin )
#
###########
