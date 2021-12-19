ADD_LIBRARY( logging )

SET( HEADERS
    "include/yavsg/asserts.hpp"
    "include/yavsg/logging.hpp"
)
SET( SOURCES
    "src/asserts.cpp"
    "src/logging.cpp"
)
TARGET_SOURCES( logging PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( logging
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( logging
    PUBLIC
        JadeMatrix::ext
        doctest::doctest
        fmt::fmt
)
