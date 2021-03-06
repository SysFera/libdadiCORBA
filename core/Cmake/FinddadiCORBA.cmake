# Try to find the dadiCORBA lib

set (DADICORBA_FOUND_STRING "Whether the dadiCORBA installation was found")

find_path(DADICORBA_INCLUDE_DIR
  Connector.hh
  paths
  ${DADICORBA_DIR}/include
  $ENV{DADICORBA_DIR}/include
  /usr/include
  /usr/local/include
)

find_library(DADICORBA_LIB
  NAMES dadiCORBA
  PATHS
  ${DADICORBA_DIR}/lib
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
)

if (DADICORBA_INCLUDE_DIR AND DADICORBA_LIB)
  set (DADICORBA_FOUND TRUE CACHE BOOL ${DADICORBA_FOUND_STRING} FORCE)
  mark_as_advanced(DADICORBA_LIB)
  mark_as_advanced(DADICORBA_LIB)
else (DADICORBA_INCLUDE_DIR AND DADICORBA_LIB)
  set (DADICORBA_FOUND FALSE CACHE BOOL ${DADICORBA_FOUND_STRING} FORCE)
endif (DADICORBA_INCLUDE_DIR AND DADICORBA_LIB)
