# To find if the diet log service module is installed 
# And to set some environemment variables
#
# Variables set :
# LOG_LIB_DIR: Directory containing all the lib
# LOG_INCLUDE_DIR: Directory containing the files to include

set (libGen "dadiCORBA")
set (libImpl "LibForwarder")

MARK_AS_ADVANCED (libName)

find_path(FWD_INCLUDE_DIR ORBMgr.hh
  PATHS ${FWD_DIR}/include/dadiCORBA
  DOC "Directory containing the forwarder include files")
find_library(FWD_LIBRARY ${libGen}
  PATHS ${FWD_DIR}/lib 
  DOC "The forwarder library idl generated")
find_library(FWD_LIBRARY2 ${libImpl}
  PATHS ${FWD_DIR}/lib 
  DOC "The forwarder library human implementation")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FWD DEFAULT_MSG FWD_INCLUDE_DIR FWD_LIBRARY)

set (FWD_LIBRARY
  ${FWD_LIBRARY}
  ${FWD_LIBRARY2}
)

if (NOT FWD_FOUND)
  MESSAGE("Forwarder found on this machine.")
  SET(FWD_DIR "" CACHE "Root of lib dadiCORBA".)
endif (NOT FWD_FOUND)

