ADD_EXECUTABLE( engine )
TARGET_SOURCES( engine PRIVATE "src/main.cpp" )
TARGET_LINK_LIBRARIES( engine
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
