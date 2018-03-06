CMAKE_MINIMUM_REQUIRED( VERSION 3.2 )

# DEBUG:
SET( CMAKE_BUILD_TYPE Debug )

SET( CMAKE_CXX_STANDARD          14 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
# SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic" )


PROJECT( "YetAnotherVoxelSpessGame" )

SET(    ORG_NAME      "JadeMatrix"                   )
SET( AUTHOR_NAME      "Joseph Durel"                 )
SET( BUNDLE_NAME      "Yet Another Voxel Spess Game" )
SET( BUNDLE_VER_NUM   "0.0"                          )
SET( BUNDLE_VER_NAME  "alpha"                        )
SET( BUNDLE_COPYRIGHT "© 2018 ${AUTHOR_NAME}"        )


# FIND_LIBRARY(    SDL_LIBRARY sdl2 )
# FIND_LIBRARY( OPENGL_LIBRARY gl   )
# FIND_LIBRARY(   GLEW_LIBRARY glew )
# FIND_LIBRARY(   GLEW_LIBRARY glm  )
FIND_PACKAGE( SDL2 REQUIRED )
# FIND_PACKAGE( SDL2_image REQUIRED )
FIND_PACKAGE( glew REQUIRED )
FIND_PACKAGE( tinyobjloader REQUIRED )

FIND_LIBRARY( SDL2_IMAGE_LIBRARY SDL2_image )

INCLUDE_DIRECTORIES(
    ${SDL2_INCLUDE_DIR}
    # ${SDL2_IMAGE_INCLUDE_DIR}
    # ${SDL2_TTF_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
)

# # DEBUG:
# MESSAGE( ${SDL2_INCLUDE_DIRS} )
# MESSAGE( ${SDL2_LIBRARIES} )
# MESSAGE( ${OPENGL_LIBRARY} )
# MESSAGE( ${GLEW_LIBRARY} )



ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE
    # src/gl/main.cpp
    # src/gl/sdl_utils.cpp
    # src/gl/gl_utils.cpp
    # src/gl/render_steps.cpp
    src/main.cpp
    src/sdl/sdl_utils.cpp
    src/gl/framebuffer.cpp
    src/gl/texture.cpp
    src/gl/shader.cpp
    # src/rendering/gl_tut_scene_render_step.cpp
    src/rendering/basic_postprocess_step.cpp
    src/rendering/4up_postprocess_step.cpp
    src/rendering/obj_render_step.cpp
    # src/rendering/scene_render_step.cpp
    src/rendering/shader_variable_names.cpp
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

# clang++ -std=c++11 -L/Users/JadeMatrix/Developer/SDL/make -lSDL2 -I/Users/JadeMatrix/Developer/SDL/make/include -I/Users/JadeMatrix/Developer/SDL/include ../src/main.cpp -o main
# TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
#     ${SDL_LIBRARY}
#     ${OPENGL_LIBRARY}
#     ${GLEW_LIBRARY}
#     "-L/usr/local/Cellar/llvm/5.0.1/lib"
# )


CONFIGURE_FILE(
    "src/Info.plist"
    "Info.plist.in"
    COPYONLY
)
