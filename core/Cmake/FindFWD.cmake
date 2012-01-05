# To find if the diet log service module is installed 
# And to set some environemment variables
#
# Variables set :
# LOG_LIB_DIR: Directory containing all the lib
# LOG_INCLUDE_DIR: Directory containing the files to include

set (libName "CorbaCommon")
set (libName2 "dadiCORBA")
set (libName3 "LibForwarder")

MARK_AS_ADVANCED (libName)

find_path(FWD_INCLUDE_DIR ORBMgr.hh
  PATHS ${FWD_DIR}/include/dadiCORBA
  DOC "Directory containing the forwarder include files")
find_library(FWD_LIBRARY ${libName}
  PATHS ${FWD_DIR}/lib 
  DOC "The forwarder library")
find_library(FWD_LIBRARY2 ${libName2}
  PATHS ${FWD_DIR}/lib 
  DOC "The forwarder library")
find_library(FWD_LIBRARY3 ${libName3}
  PATHS ${FWD_DIR}/lib 
  DOC "The forwarder library")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FWD DEFAULT_MSG FWD_INCLUDE_DIR FWD_LIBRARY)

set (FWD_LIBRARY
  ${FWD_LIBRARY}
  ${FWD_LIBRARY2}
  ${FWD_LIBRARY3}
)

if (NOT FWD_FOUND)
  MESSAGE("Forwarder found on this machine.")
  SET(FWD_DIR "" CACHE "Root of log service tree installation".)
endif (NOT FWD_FOUND)

