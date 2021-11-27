ADD_LIBRARY( gl )

SET( HEADERS
    "include/yavsg/gl/attribute_buffer.hpp"
    "include/yavsg/gl/error.hpp"
    "include/yavsg/gl/framebuffer.hpp"
    "include/yavsg/gl/shader.hpp"
    "include/yavsg/gl/shader_program.hpp"
    "include/yavsg/gl/texture.hpp"
    "include/yavsg/gl/texture_utilities.hpp"
)
SET( SOURCES
    "src/error.cpp"
    "src/framebuffer.cpp"
    "src/shader.cpp"
    "src/texture_utilities.cpp"
)
TARGET_SOURCES( gl PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( gl
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( gl
    PUBLIC
        gl_wrap
        math
        sdl
    PRIVATE
        fmt::fmt
)
