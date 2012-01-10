#****************************************************************************#
#* FWD cmake local file                                                    *#
#****************************************************************************#

# Generates the DIET_CORBA static lib

# Include directories for the library compilation:
#    - OmniORB4 include: try to compile without it! ;)
#    - Binary idl directory: for the generated headers files

include_directories(
  ${OMNIORB4_INCLUDE_DIR}
  ${CORBA_BINARY_DIR}/src/idl
)


# Generate the code out of the IDL source files

# Define the flags for the IDL compiler
#    -bcxx: C++ Backend
#    -Wba:  Generate stubs for TypeCode and Any classes
#    -Wbtp: Generate 'tie' implementation skeletons
 
set(CORBA_IDL_FLAGS "-bcxx -Wba -Wbtp")

#if (CORBA_DIET)
# Flags for DIET modules
if (DIET_USE_ALT_BATCH)
   set(CORBA_IDL_FLAGS "${DIET_IDL_FLAGS} -DHAVE_ALT_BATCH")
endif (DIET_USE_ALT_BATCH)

#set(CORBA_IDL_FLAGS "${DIET_IDL_FLAGS} -DHAVE_WORKFLOW")

if (DIET_WITH_MULTI_MA)
   set(CORBA_IDL_FLAGS "${DIET_IDL_FLAGS} -DHAVE_MULTI_MA")
endif (DIET_WITH_MULTI_MA)

if (DIET_USE_USERSCHED)
  set(CORBA_IDL_FLAGS "${DIET_IDL_FLAGS} -DUSERSCHED")
  # Install the files needed for scheduler development.
  # TODO: Create a DIET_scheduler.hpp file
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/response.hh
    ${CMAKE_CURRENT_BINARY_DIR}/common_types.hh
    ${CMAKE_CURRENT_BINARY_DIR}/SeD.hh
    DESTINATION include/scheduler
  )
endif (DIET_USE_USERSCHED)
#endif (CORBA_DIET)

# Transform DIET_IDL_FLAGS from a string to a string list containing
# the arguments for omniidl
separate_arguments(CORBA_IDL_FLAGS)

# Fix the IDL source files to compile
#    - Main parts of DIET
#    - Forwarder's IDLs

set(IDL_SOURCES
  common_types
  Callback
  response
  CallbackFwdr
  Forwarder
  Agent
  LocalAgent
  MasterAgent
  SeD
  AgentFwdr
  LocalAgentFwdr
  MasterAgentFwdr
  SeDFwdr
  Dagda
  DagdaFwdr
  LogComponent
  LogTool
  LogTypes
  LogComponentFwdr
  LogToolFwdr
  MaDag
  CltMan
  CltManFwdr
  MaDagFwdr
  WfLogService
  WfLogServiceFwdr
  )


# A clean way to call omniidl
#   Output:        Skeletons, dynamic skeletons and headers files
#   Depends:       Produced files depends on the idl files
#   Implicit deps: Scan the idl file for dependencies using CXX scanner

foreach(loop_var ${IDL_SOURCES})
  add_custom_command(
    OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
    COMMAND   ${OMNIORB4_IDL_COMPILER} ARGS ${CORBA_IDL_FLAGS}
              ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    DEPENDS   ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    IMPLICIT_DEPENDS CXX ${loop_var}.idl
    COMMENT   "idl generated code: ${loop_var}SK.cc, ${loop_var}DynSK.cc and ${loop_var}.hh"
  )
  set(CORBA_SOURCES ${CORBA_SOURCES}
    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
  )
  set(IDL_GENERATED_HEADERS
    ${IDL_GENERATED_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
    CACHE INTERNAL "CORBA generated headers"
    )
endforeach(loop_var)

# Create the CORBAFiles target.
add_custom_target(CORBAFiles
  DEPENDS ${CORBA_SOURCES} ${IDL_GENERATED_HEADERS}
)

# Compile the main static CORBA library
add_library(dadiCORBA SHARED ${CORBA_SOURCES})
add_dependencies(dadiCORBA ${IDL_GENERATED_HEADERS})
target_link_libraries(dadiCORBA
${OMNIORB4_LIBRARIES}
)

install(FILES ${IDL_GENERATED_HEADERS} DESTINATION ${INC_INSTALL_DIR})
install(TARGETS dadiCORBA DESTINATION ${LIB_INSTALL_DIR})

set(IDL_INSTALL_SOURCES
  common_types.idl
  Callback.idl
  response.idl
  CallbackFwdr.idl
  Forwarder.idl
  Agent.idl
  LocalAgent.idl
  MasterAgent.idl
  SeD.idl
  AgentFwdr.idl
  LocalAgentFwdr.idl
  MasterAgentFwdr.idl
  SeDFwdr.idl
  Dagda.idl
  DagdaFwdr.idl
  LogComponent.idl
  LogTool.idl
  LogTypes.idl
  LogComponentFwdr.idl
  LogToolFwdr.idl
  )

set(IDL_INSTALL_SOURCES
  MaDag.idl
  CltMan.idl
  CltManFwdr.idl
  MaDagFwdr.idl
  WfLogService.idl
  WfLogServiceFwdr.idl
  ${IDL_INSTALL_SOURCES}
  )

install(FILES ${IDL_INSTALL_SOURCES} DESTINATION ${INC_INSTALL_DIR}/idl)
