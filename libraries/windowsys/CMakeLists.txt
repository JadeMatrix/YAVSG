ADD_LIBRARY( windowsys )
TARGET_SOURCES( windowsys PRIVATE
    "src/frame.cpp"
    "src/window.cpp"
)
TARGET_INCLUDE_DIRECTORIES( windowsys PUBLIC "include/" )
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
