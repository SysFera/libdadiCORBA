cmake_minimum_required(VERSION 2.6)
project(DADICORBALOG)

message("path = ${PROJECT_SOURCE_DIR}/Cmake")


set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Cmake)
set(MANDIR /usr/share/man CACHE string "the man path")
set(binary)


include (UseOmniORB4)

find_package(dadiCORBA)
if(DADICORBA_FOUND)
  message("FOUND IT")
endif(DADICORBA_FOUND)


# Options set to ON by default
option(BUILD_SHARED_LIBS "disable shared libraries usage" ON)


find_package(OmniORB)
if (OMNIORB4_FOUND)
else (OMNIORB4_FOUND)
  message("OMNIORB4 installation was not found. Define the OMNIORB4_DIR variable to continue.")
  message(" - You can define a OMNIORB4_DIR environment variable")
  message(" - You can pass it as argument to cmake:")
  message("   $ cmake <source root directory> -DOMNIORB4_DIR:PATH=/path/to/OMNIORB4")
  message(" - You can use the ccmake GUI")
  set(OMNIORB4_DIR "" cache path "OMNIORB4 installation path")
endif(OMNIORB4_FOUND)

set(DADICORBALOG_LIBRARIES ${DADICORBALOG_LIBRARIES} ${OMNIORB4_LIBRARIES})


find_package(
	Boost
	1.45
	REQUIRED program_options date_time thread filesystem system unit_test_framework serialization random
)

find_package( Threads )
if( THREADS_FOUND )
  set( THREADS_LIB ${CMAKE_THREAD_LIBS_INIT} )
endif()



set( Boost_LIBRARIES ${Boost_LIBRARIES} ${THREADS_LIB} )

add_definitions( "-DBOOST_FILESYSTEM_VERSION=3" )

# gather executables and libraries under one roof
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set( BIN_DIR ${EXECUTABLE_OUTPUT_PATH} )

# ---------------- SET THE LINKER SEARCH PATH (RPATH) ----------------------
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON
    CACHE BOOL "Whether to set an rpath for dynamic libraries.")
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
    CACHE STRING "Rpath set at install stage." FORCE)


link_directories(
  ${Boost_LIBRARY_DIRS}
  ${OMNIORB4_LIBRARY_DIR}
  ${DADICORBA_LIB}
)

message("val:   ${DADICORBALOG_SOURCE_DIR}/idl")

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${OMNIORB4_INCLUDE_DIR}
  ${DADICORBA_INCLUDE_DIR}/idl
  ${DADICORBAlog_INCLUDE_DIR}/monitor
  ${DADICORBA_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}/idl
)

set(idl_path
  ${DADICORBA_INCLUDE_DIR}/idl/common_types.idl
  ${DADICORBA_INCLUDE_DIR}/idl/CorbaForwarder.idl
  ${PROJECT_SOURCE_DIR}/idl/CorbaLogForwarder.idl
  ${PROJECT_SOURCE_DIR}/idl/LogComponentFwdr.idl
  ${PROJECT_SOURCE_DIR}/idl/LogComponent.idl
  ${PROJECT_SOURCE_DIR}/idl/LogToolFwdr.idl
  ${PROJECT_SOURCE_DIR}/idl/LogTool.idl
  ${PROJECT_SOURCE_DIR}/idl/LogTypes.idl
)

generate_idl(IDLS idl_path INCLUDE_DIR ${DADICORBA_INCLUDE_DIR}/idl  OUTPUT generated)

add_subdirectory(monitor)
add_library(dadiCORBAlog
  ${generated}
  LogConnector.cc
  LogForwarder.cc
)

install (TARGETS dadiCORBAlog DESTINATION lib)

add_subdirectory(libraries/LogToolBaseC++)
add_subdirectory(libraries/LogComponentBaseC++)


add_subdirectory(examples)

