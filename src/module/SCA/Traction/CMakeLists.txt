INCLUDE_DIRECTORIES(
  ../SCA_DataBase
)
SET(HEADERS
)
SET(SOURCES
  Traction.cpp
)
ADD_COVISE_MODULE(SCA Traction ${EXTRASOURCES})
TARGET_LINK_LIBRARIES(Traction coApi coAppl coCore SCADataBase)
COVISE_INSTALL_TARGET(Traction)
