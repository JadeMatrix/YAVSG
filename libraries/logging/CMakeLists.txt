ADD_LIBRARY( logging )
TARGET_SOURCES( logging PRIVATE "src/logging.cpp" )
TARGET_INCLUDE_DIRECTORIES( logging PUBLIC "include/" )

TARGET_LINK_LIBRARIES( logging PUBLIC JadeMatrix::ext fmt::fmt )
