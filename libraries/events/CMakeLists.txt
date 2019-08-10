ADD_LIBRARY( events )
TARGET_SOURCES( events PRIVATE
    "src/event_listener.cpp"
)
TARGET_INCLUDE_DIRECTORIES( events PUBLIC "include/" )

TARGET_LINK_LIBRARIES( events PUBLIC sdl tasking )
