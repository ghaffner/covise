MACRO(USE_GEOTIFF)
   covise_find_package(GeoTIFF)
   IF ((NOT GEOTIFF_FOUND)  AND (${ARGC} LESS 1))
      USING_MESSAGE("Skipping because of missing GeoTIFF")
    RETURN()
 ENDIF((NOT GEOTIFF_FOUND) AND (${ARGC} LESS 1))
  IF(NOT GEOTIFF_USED AND GEOTIFF_FOUND)
     SET(GEOTIFF_USED TRUE)
     INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
     SET(EXTRA_LIBS ${EXTRA_LIBS} ${GEOTIFF_LIBRARIES})
  ENDIF()
ENDMACRO(USE_GEOTIFF)

