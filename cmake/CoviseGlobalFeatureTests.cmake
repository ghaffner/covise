# @file CoviseGlobalFeatureTests.cmake
#
# Place all the feature tests here you need in a global scope
#
# @author Blasius Czink


INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(COVISE_SYS_BIGENDIAN)
IF(COVISE_SYS_BIGENDIAN)
  # MESSAGE("BYTESWAP is OFF")
ELSE(COVISE_SYS_BIGENDIAN)
  # MESSAGE("BYTESWAP is ON")
  ADD_DEFINITIONS(-DBYTESWAP)
ENDIF(COVISE_SYS_BIGENDIAN)
IF(COVISE_USE_CPP11)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endif()
ENDIF(COVISE_USE_CPP11)
