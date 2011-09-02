
SET(
  IDL_FLAGS "-bcxx -Wba -Wbtp"
  CACHE STRING
  "Idl compiler flags"
  FORCE
)

MARK_AS_ADVANCED( IDL_FLAGS )

# Transform LOGSERVICE_IDL_FLAGS from a string to a string list containing
# the arguments for omniidl
SEPARATE_ARGUMENTS( IDL_FLAGS )

set (IDL_SOURCES
common_types
CorbaForwarder
)

set (IDL_INST_SOURCES
  common_types.idl
  CorbaForwarder.idl
)

SET( GENERATED_CXX_FILES "" )
SET( TARGET_IDL_GENERATED_HEADER_FILES "" )


# Generating the stubs from idl
FOREACH( loop_var ${IDL_SOURCES} )

  ADD_CUSTOM_COMMAND(
    OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
              ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
    COMMAND   ${OMNIORB4_IDL_COMPILER}
    ARGS      ${IDL_FLAGS}
              ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    DEPENDS   ${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}.idl
    IMPLICIT_DEPENDS CXX ${loop_var}.idl
    COMMENT   "idl generated code: ${loop_var}SK.cc, ${loop_var}DynSK.cc and ${loop_var}.hh")

    set (GENERATED_CXX_FILES
      ${GENERATED_CXX_FILES}
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}SK.cc
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}DynSK.cc
      )

    set (TARGET_IDL_GENERATED_HEADER_FILES
      ${TARGET_IDL_GENERATED_HEADER_FILES}
      ${CMAKE_CURRENT_BINARY_DIR}/${loop_var}.hh
      )

    # To save all the generated files object
    set (OBJ_TMP
      ${OBJ_TMP}
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/bob.dir/${loop_var}SK.cc.o
      ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/bob.dir/${loop_var}DynSK.cc.o
      )

ENDFOREACH(loop_var)

# Setting generated objects for the ar command
set (binary
  ${binary}
  ${OBJ_TMP}
  PARENT_SCOPE
)

# idl generated header files:
ADD_CUSTOM_TARGET( TARGET_IDL_GENERATED_HEADERS
  DEPENDS  ${TARGET_IDL_GENERATED_HEADER_FILES}
)

install (FILES ${TARGET_IDL_GENERATED_HEADER_FILES} DESTINATION include/)
install (FILES ${IDL_INST_SOURCES} DESTINATION include/idl/ )

add_custom_target(CORBAFiles
  DEPENDS  
  ${TARGET_IDL_GENERATED_HEADER_FILES}
  ${GENERATED_CXX_FILES}
  )

add_library(corba SHARED
  ${GENERATED_CXX_FILES}
  )

add_dependencies(corba ${TARGET_IDL_GENERATED_HEADER_FILES})

install (TARGETS corba DESTINATION lib/)
