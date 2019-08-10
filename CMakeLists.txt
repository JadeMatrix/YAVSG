CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )

# DEBUG:
SET( CMAKE_BUILD_TYPE Debug )

SET( CMAKE_CXX_STANDARD          14 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
# SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic" )
# SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wweak-vtables -Wno-c++98-compat -Wno-c++98-compat-pedantic" )


PROJECT( "YetAnotherVoxelSpessGame" )

SET( CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${PROJECT_SOURCE_DIR}/CMakeModules/"
)

ADD_SUBDIRECTORY( "libraries/" )
ADD_SUBDIRECTORY( "programs/" )
