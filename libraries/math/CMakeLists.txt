ADD_LIBRARY( math INTERFACE )

SET( HEADERS
    "include/yavsg/math/basic_transforms.hpp"
    "include/yavsg/math/common_transforms.hpp"
    "include/yavsg/math/matrix.hpp"
    "include/yavsg/math/quaternion.hpp"
    "include/yavsg/math/scalar_operations.hpp"
    "include/yavsg/math/trigonometry.hpp"
    "include/yavsg/math/vector.hpp"
)
SET( SOURCES
)
TARGET_SOURCES( math PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( math
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( math
    INTERFACE
        units
)
