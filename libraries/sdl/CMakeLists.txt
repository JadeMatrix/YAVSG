ADD_LIBRARY( sdl )

SET( HEADERS
    "include/yavsg/sdl/sdl.hpp"
    "include/yavsg/sdl/utils.hpp"
)
SET( SOURCES
    "src/utils.cpp"
)
TARGET_SOURCES( sdl PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( sdl
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( sdl
    PUBLIC
        SDL2::sdl
        gl_wrap
    PRIVATE
        fmt::fmt
)
