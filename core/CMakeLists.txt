project( FWD )
cmake_minimum_required( VERSION 2.8 )
include(CMakeDependentOption)

set(FWD_MAJOR_VERSION 0)
set(FWD_MINOR_VERSION 1)
set(FWD_REVISION_VERSION 0)

set(FWD_VERSION ${FWD_MAJOR_VERSION}.${FWD_MINOR_VERSION}.${FWD_REVISION_VERSION})

set( CMAKE_MODULE_PATH ${FWD_SOURCE_DIR}/Cmake )

set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin"
  CACHE INTERNAL "Directory where binary will be installed" )

set( LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib"
  CACHE INTERNAL "Directory where libraries will be installed" )

set( INC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/dadiCORBA"
  CACHE INTERNAL "Directory where include will be installed" )


# documentation
option(ENABLE_DOC "Build documentation" OFF)
#tests
option(ENABLE_TESTING "Provide tests execution" OFF)

# when ENABLE_DOC is enabled, we build by default: doxygen html and man pages
cmake_dependent_option(ENABLE_DOXYGEN "Build doxygen documentation" ON
  "ENABLE_DOC" OFF)
set(DOXYGEN_FORMATS  "html;man"
  CACHE STRING "Doxygen output formats (allowed: html;man;pdf;rtf)")
mark_as_advanced(DOXYGEN_FORMATS)
cmake_dependent_option(ENABLE_EXTRAS_DOC "Build extrasdocumentation" OFF
  "ENABLE_DOC" OFF)

if (ENABLE_DOC)
  add_subdirectory(doc)
endif(ENABLE_DOC)

#-------------- PLATFORM SPECIFIC COMPILATION FLAGS -----------------------
# Requires CMake >= 2.4.7
# Used within omniORB, when set remove some warnings/errors
# Not tested under SunOS & FreeBSD

if (CYGWIN)
  add_definitions(-D__cygwin__)
elseif (AIX)
  add_definitions(-D__aix__)
  # AIX doesn't like mixing static and shared libraries	 
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libraries as shared libraries." FORCE)
elseif (APPLE)
  add_definitions( -D__darwin__)
  if (NOT BUILD_SHARED_LIBS)
    message("Cannot do a static build on Apple platforms")
  endif (NOT BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON CACHE BOOL "Build libraries as shared libraries." FORCE)
elseif (LINUX)
  add_definitions(-D__linux__)
elseif (SUNOS)
  add_definitions(-D__sunos__)
elseif(FREEBSD)
  add_definitions(-D__freebsd__)
endif (CYGWIN)


# OmniORB is mandatory:
find_package(OmniORB)
if (OMNIORB4_FOUND)
  add_definitions(-D__OMNIORB4__)
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

## setup Boost
find_package(Boost 1.46.1 REQUIRED
  date_time
  iostreams
  filesystem
  regex
  system
  thread
  program_options
  unit_test_framework)

find_package(Dadi)

include_directories(
  ${OMNIORB4_INCLUDE_DIR}
  ${FWD_BINARY_DIR}/idl
  ${FWD_SOURCE_DIR}/impl
  ${FWD_SOURCE_DIR}/
  ${DADI_INCLUDE_DIR}
)

add_subdirectory(idl)


# Compile CorbaCommon static library
add_library(CorbaCommon SHARED
  ORBMgr.cc
  CorbaForwarder.cc
  diet/SeDImpl.cc
  diet/CallbackImpl.cc
  diet/AgentImpl.cc
  diet/LocalAgentImpl.cc
  diet/MasterAgentImpl.cc
  dagda/DagdaImpl.cc
  diet/DIETForwarder.cc
  log/LogForwarder.cc
  monitor/LogCentralToolFwdr_impl.cc
  monitor/LogCentralComponentFwdr_impl.cc
  monitor/LogOptions.cc
  monitor/TimeBuffer.cc
  monitor/StateManager.cc
  monitor/ReadConfig.cc
  utils/LocalTime.cc
  dagda/Dagda.cc
  diet/MasterAgent.cc
  diet/Agent.cc
  diet/SeD.cc
  diet/Callback.cc
)

# CorbaCommon library depends on CORBA files
add_dependencies(CorbaCommon CORBAFiles)


# Link CorbaCommon to DIET_CORBA, DIET_Utils and OmniORB
target_link_libraries(CorbaCommon
dadiCORBA
${DADI_LIBRARIES}
${Boost_LIBRARIES}
${OMNIORB4_LIBRARIES_sh}
${OMNIORB4_LIBRARIES}
)


if (DIET_USE_ALT_BATCH)
  target_link_libraries(CorbaCommon UtilsSeDBatch)
endif (DIET_USE_ALT_BATCH)


set(Forwarder_SOURCES
  utils/SSHTunnel.cc
  diet/Agent.cc
  diet/Callback.cc
  diet/MasterAgent.cc
  diet/SeD.cc
  log/LogForwarder.cc
)


set(Forwarder_SOURCES
  ${Forwarder_SOURCES}
  dagda/Dagda.cc
  )
if (DIET_USE_WORKFLOW)
  set(Forwarder_SOURCES
    ${Forwarder_SOURCES}
    diet/CltMan.cc
    diet/MaDag.cc
    diet/WfLogService.cc
  )
  include_directories(
    ${XERCES_INCLUDE_DIR}
    ${XQILLA_INCLUDE_DIR}
  )
endif (DIET_USE_WORKFLOW)

add_library(LibForwarder SHARED
  ${Forwarder_SOURCES}
)
add_executable(corbaForwarder
  Fwdr.cc
  ${Forwarder_SOURCES}
)

# dietForwarder & LibForwarder depend on CORBA files.
add_dependencies(corbaForwarder CORBAFiles)
add_dependencies(LibForwarder CORBAFiles)

set(Forwarder_Libs
  ${Forwarder_Libs}
  dadiCORBA
  CorbaCommon
  ${OMNIORB4_LIBRARIES}
  )



target_link_libraries(corbaForwarder
  ${Forwarder_Libs}
  pthread
  dadiCORBA
  ${Boost_LIBRARIES}
  ${DADI_LIBRARIES}
  ${OMNIORB4_LIBRARIES}
)

## ------------- New : For scheduler load support. ---------------
# Install the files needed for a scheduler creation.

  
install(TARGETS corbaForwarder DESTINATION ${BIN_INSTALL_DIR})
install(TARGETS LibForwarder DESTINATION ${LIB_INSTALL_DIR})
install(TARGETS CorbaCommon DESTINATION ${LIB_INSTALL_DIR})


install(FILES ORBMgr.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogCentralToolFwdr_impl.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogCentralComponentFwdr_impl.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES CorbaForwarder.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ToolList.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ComponentList.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/FilterManagerInterface.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/LogOptions.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/StateManager.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/ReadConfig.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES monitor/TimeBuffer.hh DESTINATION ${INC_INSTALL_DIR})
install(FILES utils/FullLinkedList.hh DESTINATION ${INC_INSTALL_DIR}/utils)
install(FILES utils/FullLinkedList.cc DESTINATION ${INC_INSTALL_DIR}/utils)
install(FILES utils/LocalTime.hh DESTINATION ${INC_INSTALL_DIR}/utils)

## tests
if(ENABLE_TESTING)
  set(TEST_FILES_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test/test_files)
  include(dadicorbaTestingMacros)
  dadicorba_test_setup()
  add_subdirectory(test)
endif()

