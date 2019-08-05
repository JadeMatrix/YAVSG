FOREACH( LIBRARY IN ITEMS
    # "engine"
    # "events"
    "gl"
    "gl_wrap"
    "math"
    # "rendering"
    "sdl"
    # "tasking"
    "units"
    # "windowsys"
)
    ADD_SUBDIRECTORY( "${LIBRARY}/" )
ENDFOREACH()
