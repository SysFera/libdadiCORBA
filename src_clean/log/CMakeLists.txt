project( LOGS )
cmake_minimum_required( VERSION 2.8 )

set(LOGS_MAJOR_VERSION 1)
set(LOGS_MINOR_VERSION 0)
set(LOGS_REVISION_VERSION 0)

set(LOGS_VERSION ${LOGS_MAJOR_VERSION}.${LOGS_MINOR_VERSION}.${LOGS_REVISION_VERSION})

set( CMAKE_MODULE_PATH ${LOGS_SOURCE_DIR}/Cmake )

set( BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin"
  CACHE INTERNAL "Directory where binary will be installed" )

find_package(FWD)

if (FWD_FOUND)
  message("founded")
endif(FWD_FOUND)

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

find_package(Dadi)


include_directories(
  ${FWD_INCLUDE_DIR}
  ${DADI_INCLUDE_DIR}
)

add_library(LogCommon
  ORBTools.cc
  SendThread.cc
  SimpleFilterManager.cc
  CoreThread.cc
)
add_subdirectory(
  examples
)

add_executable(LogCentral LogCentral.cc)
target_link_libraries(LogCentral
  ${FWD_LIBRARY}
  ${OMNIORB4_LIBRARIES}
  pthread
  LogCommon
  ${DADI_LIBRARIES}
  ${Boost_LIBRARIES}
  )
install(TARGETS LogCentral DESTINATION ${BIN_INSTALL_DIR})
