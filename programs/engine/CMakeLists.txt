ADD_EXECUTABLE( engine )
TARGET_SOURCES( engine PRIVATE "src/main.cpp" )
TARGET_LINK_LIBRARIES( engine
    events
    gl_wrap
    gl
    rendering
    sdl
    tasking
    units
    windowsys
)
SET_ALL_COMPILE_WARNINGS( engine )
