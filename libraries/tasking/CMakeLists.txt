ADD_LIBRARY( tasking )

SET( HEADERS
    "include/yavsg/tasking/task.hpp"
    "include/yavsg/tasking/tasking.hpp"
    "include/yavsg/tasking/utility_tasks.hpp"
)
SET( SOURCES
    "src/task.cpp"
    "src/tasking.cpp"
)
TARGET_SOURCES( tasking PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( tasking
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( tasking
    PUBLIC
        gl
    PRIVATE
        logging
)
