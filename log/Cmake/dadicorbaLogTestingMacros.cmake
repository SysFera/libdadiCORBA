###############################################################################
# dadicorbalog_test_setup: macro that setup our test environment
# it add a test-xml target
###############################################################################

macro(dadicorbalog_test_setup)
  include(CTest)
  enable_testing()

  # add a test-xml targets that allows us to generate Boost.Test xml reports
  add_custom_target( dadicorbalog_test-xml )

  file( MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/reports )
  set( REPORT_OUTPUT_PATH ${PROJECT_BINARY_DIR}/reports )

endmacro()



###############################################################################
# dadicorbalog_fixture_test: macro that setup a test
# @param[in] NAME  test filename stripped from extension
# @param[in] DISABLED (optional) when defined, remove test from master suite
#                     and display a message to the user.
###############################################################################

macro(dadicorbalog_fixture_test NAME)
  if(${ARGC} MATCHES 2)
    if(${ARGV1} MATCHES "DISABLED")
      message(STATUS "${ARGV0}: explicitly disabled by developer")
      set(${ARGV0}-DISABLED ON)
    endif()
  endif()


  if(NOT DEFINED ${ARGV0}-DISABLED)
    # create unit tests executable
    add_executable(${NAME}
      "${NAME}.cc"
      ${PROJECT_SOURCE_DIR}/test/utils.cpp
      #entry point
      ${PROJECT_SOURCE_DIR}/test/TestRunner.cc)

message("path: ${PROJECT_SOURCE_DIR}/utils")
    include_directories(${PROJECT_SOURCE_DIR}/test
      ${PROJECT_SOURCE_DIR}/test/third-party
      ${Boost_INCLUDE_DIR}
      ${PROJECT_SOURCE_DIR}
      )
    # link libraries
    target_link_libraries(${NAME}
      ${FWD_LIBRARY}
      ${Boost_LIBRARIES}
      ${OMNIORB4_LIBRARIES}
      pthread
      ${DADI_LIBRARIES}
      LogCommon
      )


    file(READ "${NAME}.cc" content)
    # TODO: fix this crazyness as soon as CMake to provides a proper way to
    # extract data with regex
    ## first get test suite name (should be the same as ${TEST_NAME} !!)
    string(REGEX MATCHALL "BOOST_FIXTURE_TEST_SUITE\\([a-zA-Z0-9_]*, [a-zA-Z0-9_]*\\)"
      res_int ${content})
message ("int: ${res_int}")
    string(REGEX REPLACE  
      "BOOST_FIXTURE_TEST_SUITE\\(([a-zA-Z0-9_]*), [a-zA-Z0-9_]*\\)" 
      "\\1/"
      testsuite ${res_int})


    ## removed commented case tests
    # TODO: fix this if test case is not commented out as coding standards
    # request, this will fail
    # CMake regex engine seems to have some failures ...
    string(REGEX REPLACE
      "(//|/*) BOOST_AUTO_TEST_CASE\\([a-zA-Z0-9_]*\\)" "\\1"
      content ${content})
    ## then test case names
    string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\([a-zA-Z0-9_]*\\)"
      res_int ${content})
    # tricks: i add a ";" at the end, so that i can get a proper list
    string(REGEX REPLACE "BOOST_AUTO_TEST_CASE\\(([a-zA-Z0-9_]*)\\)" "\\1;"
      tests ${res_int})

    foreach(loop_var ${tests})
      add_test("${testsuite}${loop_var}" "${BIN_DIR}/${NAME}"
        "--run_test=${testsuite}${loop_var}")
      # prevent Boost.Test to catch unrelated exceptions
      set_property(TEST "${testsuite}${loop_var}"
        PROPERTY ENVIRONMENT "BOOST_TEST_CATCH_SYSTEM_ERRORS=no;")
      # just make sure that our test are run in a serial fashion
      set_property(TEST "${testsuite}${loop_var}" PROPERTY RUN_SERIAL ON)
    endforeach()

    # generate XML reports
    if(ENABLE_REPORTS)
      add_custom_target(${NAME}-xml
      COMMAND ${CMAKE_COMMAND}
      -DTEST_PROG=${NAME}
      -DBIN_PATH=${EXECUTABLE_OUTPUT_PATH}
      -DREPORT_PATH=${REPORT_OUTPUT_PATH}
      -P ${PROJECT_SOURCE_DIR}/cmake/runtest.cmake)
      add_dependencies(test-xml ${NAME}-xml)
    endif()
  endif()
endmacro()

###############################################################################
# dadicorbalog_test: macro that setup a test
# @param[in] NAME  test filename stripped from extension
# @param[in] DISABLED (optional) when defined, remove test from master suite
#                     and display a message to the user.
###############################################################################

macro(dadicorbalog_test NAME)
  if(${ARGC} MATCHES 2)
    if(${ARGV1} MATCHES "DISABLED")
      message(STATUS "${ARGV0}: explicitly disabled by developer")
      set(${ARGV0}-DISABLED ON)
    endif()
  endif()

  if(NOT DEFINED ${ARGV0}-DISABLED)
    # create unit tests executable
    add_executable(${NAME}
      "${NAME}.cc"
      #entry point
      ${PROJECT_SOURCE_DIR}/test/TestRunner.cc)

    include_directories(${PROJECT_SOURCE_DIR}/test
      ${PROJECT_SOURCE_DIR}/utils
      )
    # link libraries
    target_link_libraries(${NAME}
      ${FWD_LIBRARY}
      ${Boost_LIBRARIES}
      ${OMNIORB4_LIBRARIES}
      LogCommon
      pthread
      ${DADI_LIBRARIES}
      )


    file(READ "${NAME}.cc" content)
    # TODO: fix this crazyness as soon as CMake to provides a proper way to
    # extract data with regex
    ## first get test suite name (should be the same as ${TEST_NAME} !!)
    string(REGEX MATCHALL "BOOST_AUTO_TEST_SUITE\\( [a-zA-Z0-9_]* \\)"
      res_int ${content})
    message("res: ${res_int}")
    message("res: ${NAME}")

    string(REGEX REPLACE  "BOOST_AUTO_TEST_SUITE\\(( [a-zA-Z0-9_]* )\\)" "\\1/"
      testsuite ${res_int})


    ## removed commented case tests
    # TODO: fix this if test case is not commented out as coding standards
    # request, this will fail
    # CMake regex engine seems to have some failures ...
    string(REGEX REPLACE
      "(//|/*) BOOST_AUTO_TEST_CASE\\( [a-zA-Z0-9_]* \\)" "\\1"
      content ${content})
    ## then test case names
    string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( [a-zA-Z0-9_]* \\)"
      res_int ${content})
    # tricks: i add a ";" at the end, so that i can get a proper list
    string(REGEX REPLACE "BOOST_AUTO_TEST_CASE\\(( [a-zA-Z0-9_]* )\\)" "\\1;"
      tests ${res_int})

    foreach(loop_var ${tests})
      add_test("${testsuite}${loop_var}" "${BIN_DIR}/${NAME}"
        "--run_test=${testsuite}${loop_var}")
      # prevent Boost.Test to catch unrelated exceptions
      set_property(TEST "${testsuite}${loop_var}"
        PROPERTY ENVIRONMENT "BOOST_TEST_CATCH_SYSTEM_ERRORS=no;")
      # just make sure that our test are run in a serial fashion
      set_property(TEST "${testsuite}${loop_var}" PROPERTY RUN_SERIAL ON)
    endforeach()

    # generate XML reports
    if(ENABLE_REPORTS)
      add_custom_target(${NAME}-xml
      COMMAND ${CMAKE_COMMAND}
      -DTEST_PROG=${NAME}
      -DBIN_PATH=${EXECUTABLE_OUTPUT_PATH}
      -DREPORT_PATH=${REPORT_OUTPUT_PATH}
      -P ${PROJECT_SOURCE_DIR}/cmake/runtest.cmake)
      add_dependencies(test-xml ${NAME}-xml)
    endif()
  endif()
endmacro()

