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
    "src/shader_variable_names.cpp"
)
TARGET_INCLUDE_DIRECTORIES( rendering PUBLIC "include/" )

FIND_PACKAGE( SDL2 2.0.8 REQUIRED COMPONENTS image )
FIND_PACKAGE( tinyobjloader 1.0.4 EXACT REQUIRED COMPONENTS tinyobjloader )
TARGET_LINK_LIBRARIES( rendering
    PUBLIC
        tinyobjloader
        gl
        gl_wrap
        math
        SDL2::image # FIXME:For texture_reference.hpp
        tasking
)

TARGET_COMPILE_OPTIONS( rendering PRIVATE ${YAVSG_COMPILE_OPTIONS} )
