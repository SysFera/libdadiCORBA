#    Find dadi library

# This module defines:
# DADI_FOUND		- whether library was found
# DADI_INCLUDE_DIR	- include path
# DADI_LIBRARIES	- libraries to link against

# get some hints from pkg-config
if(NOT WIN32)
  include(FindPkgConfig)
  # run quietly since we only retrieve some hints from pkg-config
  pkg_check_modules(DADI QUIET dadi)
  # returns DADI_INCLUDE_DIRS and DADI_LIBRARY_DIRS
endif()

find_path(DADI_INCLUDE_DIR
  NAMES dadi/dadi.hh
  PATHS ${DADI_INSTALL_DIR}/include
  ${DADI_INCLUDE_DIR}
)

find_library(DADI_LIBRARIES
  NAMES $dadi
  PATHS_SUFFIXES dadi
  PATHS ${DADI_INSTALL_DIR}/lib
  ${DADI_LIBRARY_DIRS}
)

# defines our variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DADI DEFAULT_MSG
  DADI_LIBRARIES DADI_INCLUDE_DIR)
