# - Find Xerces-C
# Find the Xerces-C includes and library
#
#  XERCESC_INCLUDE_DIR - Where to find xercesc include sub-directory.
#  XERCESC_LIBRARIES   - List of libraries when using Xerces-C.
#  XERCESC_FOUND       - True if Xerces-C found.

IF (XERCESC_INCLUDE_DIR)
  SET(XERCESC_FIND_QUIETLY TRUE)
ENDIF (XERCESC_INCLUDE_DIR)

FIND_PATH(XERCESC_INCLUDE_DIR xercesc/dom/DOM.hpp
  PATHS
  $ENV{XERCESC_HOME}/include
  $ENV{EXTERNLIBS}/xerces/include
  $ENV{EXTERNLIBS}/xercesc/include
  DOC "xercesc - Headers"
  NO_DEFAULT_PATH
)
FIND_PATH(XERCESC_INCLUDE_DIR xercesc/dom/DOM.hpp DOC "xercesc - Headers")

SET(XERCESC_NAMES xerces-c_3_1 xerces-c-3.1 xerces-c_3 xerces-c-3 xerces-c_2 xerces-c)
SET(XERCESC_DBG_NAMES xerces-c_3d_1 xerces-c_3d xerces-c_2d xerces-cd)

FIND_LIBRARY(XERCESC_LIBRARY NAMES ${XERCESC_NAMES}
  PATHS
  $ENV{XERCESC_HOME}/lib
  $ENV{EXTERNLIBS}/xerces/lib
  $ENV{EXTERNLIBS}/xercesc/lib
  DOC "xercesc - Library"
  NO_DEFAULT_PATH
)
FIND_LIBRARY(XERCESC_LIBRARY NAMES ${XERCESC_NAMES} DOC "xercesc - Library")

INCLUDE(FindPackageHandleStandardArgs)

IF(MSVC)
  # VisualStudio needs a debug version
  FIND_LIBRARY(XERCESC_LIBRARY_DEBUG NAMES ${XERCESC_DBG_NAMES}
    PATHS
    $ENV{XERCESC_HOME}/lib
    $ENV{EXTERNLIBS}/xerces/lib
    $ENV{EXTERNLIBS}/xercesc/lib
    DOC "xercesc - Library (Debug)"
  )
  
  IF(XERCESC_LIBRARY_DEBUG AND XERCESC_LIBRARY)
    SET(XERCESC_LIBRARIES optimized ${XERCESC_LIBRARY} debug ${XERCESC_LIBRARY_DEBUG})
  ENDIF(XERCESC_LIBRARY_DEBUG AND XERCESC_LIBRARY)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(XERCESC DEFAULT_MSG XERCESC_LIBRARY XERCESC_LIBRARY_DEBUG XERCESC_INCLUDE_DIR)

  MARK_AS_ADVANCED(XERCESC_LIBRARY XERCESC_LIBRARY_DEBUG XERCESC_INCLUDE_DIR)
  
ELSE(MSVC)
  # rest of the world
  SET(XERCESC_LIBRARIES ${XERCESC_LIBRARY})

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(XERCESC DEFAULT_MSG XERCESC_LIBRARY XERCESC_INCLUDE_DIR)
  
  MARK_AS_ADVANCED(XERCESC_LIBRARY XERCESC_INCLUDE_DIR)
  
ENDIF(MSVC)

IF(XERCESC_FOUND)
  SET(XERCESC_INCLUDE_DIRS ${XERCESC_INCLUDE_DIR})
ENDIF(XERCESC_FOUND)
