# LogService cmake local file

# Generate the code out of the IDL source files and build the ad-hoc libraries.
# (idl is part of omniORB which is mandatory)


INCLUDE_DIRECTORIES(
  ${OMNIORB4_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${BOB_SOURCE_DIR}/idl
  ${BOB_SOURCE_DIR}/impl
  ${BOB_SOURCE_DIR}
)


# Define the flags for the IDL compiler
SET(
  LOGSERVICE_IDL_FLAGS "-bcxx -Wba -Wbtp"
  CACHE STRING
  "Idl compiler flags"
  FORCE
)

generate_idl

MARK_AS_ADVANCED( LOGSERVICE_IDL_FLAGS )

# Transform LOGSERVICE_IDL_FLAGS from a string to a string list containing
# the arguments for omniidl
SEPARATE_ARGUMENTS( LOGSERVICE_IDL_FLAGS )

SET( IDL_SOURCES
  LogComponent
  LogTool
  LogTypes
  CorbaLogForwarder
  LogComponentFwdr
  LogToolFwdr
)

# The C/C++ callers of the idl generated interface will include the 
# idl generated header files. The following technical variable is a
# commidity for expressing this dependency (refer below to the definition
# of LOGSERVICE_TARGET_IDL_GENERATED_HEADERS):
#SET( LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES )

SET( LOGSERVICE_IDL_GENERATED_CXX_FILES "" )

FOREACH( loop_var ${IDL_SOURCES} )

  ADD_CUSTOM_COMMAND(
    OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
    COMMAND   ${OMNIORB4_IDL_COMPILER}
    ARGS      ${LOGSERVICE_IDL_FLAGS}
              ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    DEPENDS   ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    IMPLICIT_DEPENDS CXX ${loop_var}.idl
    COMMENT   "idl generated code: ${loop_var}SK.cc, ${loop_var}DynSK.cc and ${loop_var}.hh")

    set (LOGSERVICE_IDL_GENERATED_CXX_FILES
      ${LOGSERVICE_IDL_GENERATED_CXX_FILES}
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
      )

    set (LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES
      ${LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES}
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
      )

    # To save all the generated files object
    set (OBJ_TMP
      ${OBJ_TMP}
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogCorba.dir/${loop_var}SK.cc.o
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/LogCorba.dir/${loop_var}DynSK.cc.o
      )

ENDFOREACH(loop_var)

# Setting generated objects for the ar command
set (binary
  ${binary}
  ${OBJ_TMP}
  PARENT_SCOPE
)


install( FILES ${LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES} DESTINATION include/ )

# idl generated header files:
ADD_CUSTOM_TARGET( LOGSERVICE_TARGET_IDL_GENERATED_HEADERS
  DEPENDS  ${LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES}
)

add_custom_target(CORBAFiles
  DEPENDS  
  ${LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES}
  ${LOGSERVICE_IDL_GENERATED_CXX_FILES}
  )

add_library(LogCorba STATIC
  ${LOGSERVICE_IDL_GENERATED_CXX_FILES}
  )

add_dependencies(LogCorba ${LOGSERVICE_TARGET_IDL_GENERATED_HEADER_FILES})