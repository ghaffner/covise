MACRO(USE_GLUT)
  COVISE_FIND_PACKAGE(GLUT)
  IF ((NOT GLUT_FOUND) AND (${ARGC} LESS 1))
    USING_MESSAGE("Skipping because of missing GLUT")
    RETURN()
  ENDIF((NOT GLUT_FOUND) AND (${ARGC} LESS 1))
  IF (NOT GLUT_FOUND)
  ENDIF(NOT GLUT_FOUND)
  IF(NOT GLUT_USED AND GLUT_FOUND)
    SET(GLUT_USED TRUE)
    USE_OPENGL(optional)
    ADD_DEFINITIONS(-D GLUT_NO_LIB_PRAGMA)  
    
    SET(CUDA_NVCC_DEFINITIONS ${CUDA_NVCC_DEFINITIONS} GLUT_NO_LIB_PRAGMA)
    INCLUDE_DIRECTORIES(SYSTEM ${GLUT_INCLUDE_DIR})
    SET(EXTRA_LIBS ${EXTRA_LIBS} ${GLUT_LIBRARIES})
    IF(UNIX AND NOT APPLE)
      SET(EXTRA_LIBS ${EXTRA_LIBS} Xxf86vm)
    ENDIF()
  ENDIF()
ENDMACRO(USE_GLUT)

