ADD_LIBRARY( windowsys )

SET( HEADERS
    "include/yavsg/windowsys/frame.hpp"
    "include/yavsg/windowsys/window.hpp"
)
SET( SOURCES
    "src/frame.cpp"
    "src/window.cpp"
)
TARGET_SOURCES( windowsys PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( windowsys
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( windowsys
    PUBLIC
        events
        gl
        rendering
        sdl
        units
        tasking
    PRIVATE
        logging
)
