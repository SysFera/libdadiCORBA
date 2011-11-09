project( FWD )
cmake_minimum_required( VERSION 2.8 )

set(FWD_MAJOR_VERSION 1)
set(FWD_MINOR_VERSION 0)
set(FWD_REVISION_VERSION 0)

set(FWD_VERSION ${FWD_MAJOR_VERSION}.${FWD_MINOR_VERSION}.${FWD_REVISION_VERSION})

set( CMAKE_MODULE_PATH ${FWD_SOURCE_DIR}/Cmake )

set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin"
  CACHE INTERNAL "Directory where binary will be installed" )

set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  CACHE INTERNAL "Directory where libraries will be installed" )

set( INC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include"
  CACHE INTERNAL "Directory where include will be installed" )

# OmniORB is mandatory:
find_package(OmniORB)
if (OMNIORB4_FOUND)
  add_definitions(-D__OMNIORB4__)
	if (BUILD_SHARED_LIBS)
	  # Link DIET to OmniORB shared libs
		set(OMNIORB4_LIBRARIES ${OMNIORB4_LIBRARIES_sh})
	else (BUILD_SHARED_LIBS)
	  # Link DIET to OmniORB static libs
		set(OMNIORB4_LIBRARIES ${OMNIORB4_LIBRARIES_st})
	endif (BUILD_SHARED_LIBS)
else (OMNIORB4_FOUND)
  message("omniORB installation was not found. Please provide OMNIORB4_DIR:")
  message("  - through the GUI when working with ccmake, ")
  message("  - as a command line argument when working with cmake e.g. ")
  message("    cmake .. -DOMNIORB4_DIR:PATH=/usr/local/omniORB-4.0.7 ")
  message("Note: the following message is triggered by cmake on the first ")
  message("    undefined necessary PATH variable (e.g.  OMNIORB4_INCLUDE_DIR).")
  message("    Providing OMNIORB4_DIR (as above described) is probably the")
  message("    simplest solution unless you have a really customized/odd")
  message("    omniORB installation...")
  set(OMNIORB4_DIR "" CACHE PATH "Root of omniORB install tree.")
endif (OMNIORB4_FOUND)


include_directories(
  ${OMNIORB4_INCLUDE_DIR}
  ${FWD_BINARY_DIR}/idl    
  ${FWD_SOURCE_DIR}/impl
  ${FWD_SOURCE_DIR}/
)

add_subdirectory(idl)


# Compile CorbaCommon static library
add_library(CorbaCommon SHARED
  ORBMgr.cc
  CorbaForwarder.cc
  impl/SeDImpl.cc
  impl/CallbackImpl.cc
  impl/AgentImpl.cc
  impl/LocalAgentImpl.cc
  impl/MasterAgentImpl.cc
  impl/DagdaImpl.cc
  debug.cc
  diet/DIETForwarder.cc
  log/LogForwarder.cc
  monitor/LogCentralTool_impl.cc
  monitor/LogCentralComponent_impl.cc
  monitor/LogOptions.cc
  monitor/TimeBuffer.cc
  monitor/StateManager.cc
  monitor/ReadConfig.cc
  utils/LocalTime.cc
  forwarders/Dagda.cc
  forwarders/MasterAgent.cc
  forwarders/Agent.cc
  forwarders/SeD.cc
  forwarders/Callback.cc
)

# CorbaCommon library depends on CORBA files
add_dependencies(CorbaCommon CORBAFiles)

# Dependency when compiling with Batch support
if (DIET_USE_ALT_BATCH)
  include_directories( ${DIET_SOURCE_DIR}/src/utils/batch )
  add_dependencies( CorbaCommon BatchSystem.hh )
endif (DIET_USE_ALT_BATCH)

# Link CorbaCommon to DIET_CORBA, DIET_Utils and OmniORB
target_link_libraries(CorbaCommon
#  DIET_CORBA
dadiCORBA
)


if (DIET_USE_ALT_BATCH)
  target_link_libraries(CorbaCommon UtilsSeDBatch)
endif (DIET_USE_ALT_BATCH)

# Forwarder include dirs:
# Need all the DIET objects headers.
include_directories(
  ${DIET_SOURCE_DIR}/src/agent
  ${DIET_SOURCE_DIR}/src/client
  ${DIET_SOURCE_DIR}/src/utils/nodes
  ${DIET_SOURCE_DIR}/src/client/workflow
  ${DIET_SOURCE_DIR}/src/agent/workflow
  ${DIET_SOURCE_DIR}/src/utils/workflow
  ${DIET_SOURCE_DIR}/src/SeD
)

set(Forwarder_SOURCES
  SSHTunnel.cc
  Options.cc
#  NetConfig.cc
  forwarders/Agent.cc
  forwarders/Callback.cc
  forwarders/MasterAgent.cc
  forwarders/SeD.cc
  forwarders/LogForwarder.cc
#  forwarders/LogComponent.cc
)


set(Forwarder_SOURCES
  ${Forwarder_SOURCES}
  forwarders/Dagda.cc
  )
if (DIET_USE_WORKFLOW)
  set(Forwarder_SOURCES
    ${Forwarder_SOURCES}
    forwarders/CltMan.cc
    forwarders/MaDag.cc
    forwarders/WfLogService.cc
  )
  include_directories(
    ${XERCES_INCLUDE_DIR}
    ${XQILLA_INCLUDE_DIR}
  )
endif (DIET_USE_WORKFLOW)

add_library(LibForwarder STATIC
  ${Forwarder_SOURCES}
)
add_executable(corbaForwarder
  ORBMgr.cc
  Fwdr.cc
  CorbaForwarder.cc
  ${Forwarder_SOURCES}
)

# dietForwarder & LibForwarder depend on CORBA files.
add_dependencies(corbaForwarder CORBAFiles)
add_dependencies(LibForwarder CORBAFiles)

set(Forwarder_Libs
  ${Forwarder_Libs}
  dadiCORBA
#  DIET_Dagda
  )

set(Forwarder_Libs
#  DIET_Utils
  ${Forwarder_Libs}
#  DIET_CORBA
  CorbaCommon
#  UtilsNodes
)

if (DIET_USE_WORKFLOW)
  set(Forwarder_Libs
    ${Forwarder_Libs}
    CltWf
    MaDag
    UtilsWf
    UtilsEvents
    ${XQILLA_LIBRARY}
    ${XERCES_LIBRARY}
  )
endif (DIET_USE_WORKFLOW)
set(Forwarder_Libs
  ${Forwarder_Libs}
  CorbaCommon
#  AgentCommon
#  DIET_SeD
#  DIET_client
  ${OMNIORB4_LIBRARIES}
)

message(STATUS "DIET forwarder libs: ${Forwarder_Libs}")

target_link_libraries(corbaForwarder
  ${Forwarder_Libs}
  pthread
  dadiCORBA
)
if (DIET_USE_USERSCHED)
  target_link_libraries(corbaForwarder ${CMAKE_DL_LIBS})
endif (DIET_USE_USERSCHED)

## ------------- New : For scheduler load support. ---------------
# Install the files needed for a scheduler creation.

  
install(TARGETS corbaForwarder DESTINATION ${BIN_INSTALL_DIR})
install(TARGETS CorbaCommon DESTINATION ${LIB_INSTALL_DIR})


install(FILES ORBMgr.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogCentralTool_impl.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogCentralComponent_impl.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES CorbaForwarder.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ToolList.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ComponentList.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/FilterManagerInterface.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogOptions.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/StateManager.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ReadConfig.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/TimeBuffer.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES FullLinkedList.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES FullLinkedList.cc DESTINATION ${INC_INSTALL_DIR})
install(FILES LocalTime.hh DESTINATION ${INC_INSTALL_DIR})



if (DIET_USE_LOG)
  include_directories(
    ${LOG_INCLUDE_DIR}
    ${LOG_INCLUDE_DIR}/monitor
    ${LOG_INCLUDE_DIR}/idl
    ${DIET_SOURCE_DIR}/src/utils/log)

  set(Forwarder_Libs
    ${Forwarder_Libs}
    DietLogLibrary)

endif (DIET_USE_LOG)

if (DIET_BUILD_TOOLS)
  add_subdirectory(tools)
endif (DIET_BUILD_TOOLS)
