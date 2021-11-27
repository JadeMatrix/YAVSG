ADD_LIBRARY( units INTERFACE )

SET( HEADERS
    "include/yavsg/units/angular.hpp"
    "include/yavsg/units/constants.hpp"
    "include/yavsg/units/dpi.hpp"
    "include/yavsg/units/prefixes.hpp"
    "include/yavsg/units/unit.hpp"
)
SET( SOURCES
)
TARGET_SOURCES( units PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( units
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)
