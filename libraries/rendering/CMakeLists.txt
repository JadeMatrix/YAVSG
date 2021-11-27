ADD_LIBRARY( rendering )
TARGET_SOURCES( rendering PRIVATE
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
TARGET_INCLUDE_DIRECTORIES( rendering PUBLIC "include/" )

TARGET_LINK_LIBRARIES( rendering
    PUBLIC
        tinyobjloader
        gl
        gl_wrap
        math
        SDL2::image # FIXME:For texture_reference.hpp
        tasking
    PRIVATE
        logging
)

SET_ALL_COMPILE_WARNINGS( rendering )
