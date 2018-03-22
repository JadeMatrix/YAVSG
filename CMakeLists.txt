CMAKE_MINIMUM_REQUIRED( VERSION 3.2 )

# DEBUG:
SET( CMAKE_BUILD_TYPE Debug )

SET( CMAKE_CXX_STANDARD          14 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
# SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic" )
# SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wweak-vtables -Wno-c++98-compat -Wno-c++98-compat-pedantic" )


PROJECT( "YetAnotherVoxelSpessGame" )

SET(    ORG_NAME      "JadeMatrix"                   )
SET( AUTHOR_NAME      "Joseph Durel"                 )
SET( BUNDLE_NAME      "Yet Another Voxel Spess Game" )
SET( BUNDLE_VER_NUM   "0.0"                          )
SET( BUNDLE_VER_NAME  "alpha"                        )
SET( BUNDLE_COPYRIGHT "© 2018 ${AUTHOR_NAME}"        )


FIND_PACKAGE( SDL2 REQUIRED )
FIND_PACKAGE( glew REQUIRED )
FIND_PACKAGE( tinyobjloader REQUIRED )

FIND_LIBRARY( SDL2_IMAGE_LIBRARY SDL2_image )

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIR}
    # ${SDL2_TTF_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)


ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE
    src/engine/4up_postprocess_step.cpp
    src/engine/basic_postprocess_step.cpp
    src/engine/dof_postprocess_step.cpp
    src/events/event_listener.cpp
    src/engine/frame.cpp
    src/engine/main.cpp
    src/engine/multi_postprocess_step.cpp
    src/engine/obj_render_step.cpp
    src/engine/obj.cpp
    src/engine/scene.cpp
    src/gl/error.cpp
    src/gl/framebuffer.cpp
    src/gl/shader.cpp
    src/gl/texture_utilities.cpp
    src/rendering/camera.cpp
    src/rendering/shader_variable_names.cpp
    src/sdl/sdl_utils.cpp
    src/tasking/task.cpp
    src/tasking/tasking.cpp
    src/windowsys/window.cpp
)
SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST           "Info.plist.in"
    MACOSX_BUNDLE_INFO_STRING          "${BUNDLE_NAME} v${BUNDLE_VER_NUM} (${BUNDLE_VER_NAME}) ${BUNDLE_COPYRIGHT}"
    MACOSX_BUNDLE_ICON_FILE            "${BUNDLE_NAME}.icns"
    MACOSX_BUNDLE_GUI_IDENTIFIER       "com.${ORG_NAME}.${BUNDLE_NAME}"
    MACOSX_BUNDLE_LONG_VERSION_STRING  "${BUNDLE_NAME} v${BUNDLE_VER_NUM} (${BUNDLE_VER_NAME})"
    MACOSX_BUNDLE_BUNDLE_NAME          "${BUNDLE_NAME}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${BUNDLE_VER_NAME}"
    MACOSX_BUNDLE_BUNDLE_VERSION       "${BUNDLE_VER_NUM}"
    MACOSX_BUNDLE_COPYRIGHT            "${BUNDLE_COPYRIGHT}"
)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARY}
    # ${SDL2_TTF_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${TINYOBJLOADER_LIBRARIES}
    "-framework OpenGL"
)


CONFIGURE_FILE(
    "src/Info.plist"
    "Info.plist.in"
    COPYONLY
)
