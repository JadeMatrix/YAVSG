ADD_LIBRARY( rendering )

SET( HEADERS
    "include/yavsg/rendering/4up_postprocess_step.hpp"
    "include/yavsg/rendering/basic_postprocess_step.hpp"
    "include/yavsg/rendering/camera.hpp"
    "include/yavsg/rendering/dof_postprocess_step.hpp"
    "include/yavsg/rendering/material.hpp"
    "include/yavsg/rendering/multi_postprocess_step.hpp"
    "include/yavsg/rendering/obj.hpp"
    "include/yavsg/rendering/obj_render_step.hpp"
    "include/yavsg/rendering/render_object_manager.hpp"
    "include/yavsg/rendering/render_step.hpp"
    "include/yavsg/rendering/scene.hpp"
    "include/yavsg/rendering/shader_utils.hpp"
    "include/yavsg/rendering/shader_variable_names.hpp"
    "include/yavsg/rendering/texture_reference.hpp"
)
SET( SOURCES
    "src/4up_postprocess_step.cpp"
    "src/basic_postprocess_step.cpp"
    "src/camera.cpp"
    "src/dof_postprocess_step.cpp"
    "src/multi_postprocess_step.cpp"
    "src/obj.cpp"
    "src/obj_render_step.cpp"
    "src/scene.cpp"
    "src/shader_utils.cpp"
    "src/shader_variable_names.cpp"
)
TARGET_SOURCES( rendering PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( rendering
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( rendering
    PUBLIC
        gl
        gl_wrap
        math
        tasking
        SDL2::image # FIXME:For texture_reference.hpp
        tinyobjloader
    PRIVATE
        logging
        doctest::doctest
        fmt::fmt
)
