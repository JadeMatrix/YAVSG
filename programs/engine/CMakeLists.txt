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
TARGET_COMPILE_OPTIONS( engine PRIVATE ${YAVSG_COMPILE_OPTIONS} )
