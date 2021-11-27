ADD_LIBRARY( events )

SET( HEADERS
    "include/yavsg/events/event_listener.hpp"
)
SET( SOURCES
    "src/event_listener.cpp"
)
TARGET_SOURCES( events PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( events
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( events
    PUBLIC
        sdl
        tasking
)
