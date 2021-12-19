ADD_EXECUTABLE( engine )

SET( HEADERS
)
SET( SOURCES
    "src/main.cpp"
)
TARGET_SOURCES( engine PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_LINK_LIBRARIES( engine
    PRIVATE
        events
        gl_wrap
        gl
        logging
        rendering
        sdl
        tasking
        units
        windowsys
)
