FOREACH( LIBRARY IN ITEMS
    "events"
    "gl"
    "gl_wrap"
    "logging"
    "math"
    "rendering"
    "sdl"
    "tasking"
    "units"
    "windowsys"
)
    ADD_SUBDIRECTORY( "${LIBRARY}/" )
ENDFOREACH()
