ADD_LIBRARY( tasking )
TARGET_SOURCES( tasking PRIVATE
    "src/task.cpp"
    "src/tasking.cpp"
)
TARGET_INCLUDE_DIRECTORIES( tasking PUBLIC "include/" )

TARGET_LINK_LIBRARIES( tasking PUBLIC gl )
