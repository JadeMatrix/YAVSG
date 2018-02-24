CMAKE_MINIMUM_REQUIRED( VERSION 3.6 )

# DEBUG:
SET( CMAKE_BUILD_TYPE Debug )
SET( CMAKE_C_COMPILER   /usr/local/Cellar/llvm/5.0.1/bin/clang   )
SET( CMAKE_CXX_COMPILER /usr/local/Cellar/llvm/5.0.1/bin/clang++ )

SET( CMAKE_CXX_STANDARD          #[[11]] 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )


PROJECT( "YetAnotherVoxelSpessGame" )

SET(    ORG_NAME      "JadeMatrix"            )
SET( AUTHOR_NAME      "Joseph Durel"          )
SET( BUNDLE_NAME      "Yet Another Voxel Spess Game"       )
SET( BUNDLE_VER_NUM   "0.0"                   )
SET( BUNDLE_VER_NAME  "alpha"                 )
SET( BUNDLE_COPYRIGHT "© 2018 ${AUTHOR_NAME}" )


FIND_LIBRARY(    SDL_LIBRARY sdl2 )
FIND_LIBRARY( OPENGL_LIBRARY gl   )
FIND_LIBRARY(   GLEW_LIBRARY glew )
# # DEBUG:
# MESSAGE( ${SDL_LIBRARY} )
# MESSAGE( ${OPENGL_LIBRARY} )
# MESSAGE( ${GLEW_LIBRARY} )



ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE
    src/main.cpp
    src/real.cpp
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

# clang++ -std=c++11 -L/Users/JadeMatrix/Developer/SDL/make -lSDL2 -I/Users/JadeMatrix/Developer/SDL/make/include -I/Users/JadeMatrix/Developer/SDL/include ../src/main.cpp -o main
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    ${SDL_LIBRARY}
    ${OPENGL_LIBRARY}
    ${GLEW_LIBRARY}
    "-L/usr/local/Cellar/llvm/5.0.1/lib"
)


# ADD_EXECUTABLE( test_play_song
#     "${CMAKE_CURRENT_SOURCE_DIR}/Source/test_play_song.m"
#     "${CMAKE_CURRENT_SOURCE_DIR}/Source/Yet Another Voxel Spess GameCoreAudio.mm"
#     "${CMAKE_CURRENT_SOURCE_DIR}/Source/Yet Another Voxel Spess GameErrors.mm"
# )
# TARGET_LINK_LIBRARIES( test_play_song
#     "-framework Cocoa"
#     "-framework AudioToolbox"
#     "-framework CoreServices"
#     "-framework Security"
# )


# FIND_PROGRAM(
#     INKSCAPE_PROGRAM
#     inkscape
#     PATHS $ENV{HOME}/Applications/Inkscape.app/Contents/Resources/bin /Applications/Inkscape.app/Contents/Resources/bin
#     # CMake wants to prefer Inkscape.app/Contents/MacOS/Inkscape, which won't
#     # work on OS X -- need to use Inkscape.app/Contents/Resources/bin/inkscape
#     NO_DEFAULT_PATH
# )
# FIND_PROGRAM(
#     ICONUTIL_PROGRAM
#     iconutil
# )


# ADD_CUSTOM_COMMAND(
#     OUTPUT "Resources/${BUNDLE_NAME}.icns"
#     COMMAND ${CMAKE_COMMAND} -E make_directory "Resources/${BUNDLE_NAME}.iconset/"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_16x16@1x.png" --export-dpi 96 --export-area=512:480:528:496 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_16x16@2x.png" --export-dpi 192 --export-area=512:480:528:496 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_32x32@1x.png" --export-dpi 96 --export-area=512:448:544:480 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_32x32@2x.png" --export-dpi 192 --export-area=512:448:544:480 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_128x128@1x.png" --export-dpi 96 --export-area=512:256:640:384 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_128x128@2x.png" --export-dpi 192 --export-area=512:256:640:384 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_256x256@1x.png" --export-dpi 96 --export-area=512:0:768:256 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_256x256@2x.png" --export-dpi 192 --export-area=512:0:768:256 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_512x512@1x.png" --export-dpi 96 --export-area=0:0:512:512 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${INKSCAPE_PROGRAM} --without-gui --export-png="${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset/icon_512x512@2x.png" --export-dpi 192 --export-area=0:0:512:512 "${CMAKE_CURRENT_SOURCE_DIR}/Resources/${BUNDLE_NAME}.svg"
#     COMMAND ${ICONUTIL_PROGRAM} -c icns -o "${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.icns" "${CMAKE_CURRENT_BINARY_DIR}/Resources/${BUNDLE_NAME}.iconset"
#     DEPENDS "Resources/Yet Another Voxel Spess Game.svg"
# )

# SET_SOURCE_FILES_PROPERTIES(
#     "Resources/${BUNDLE_NAME}.icns"
#     PROPERTIES
#     MACOSX_PACKAGE_LOCATION "Resources"
# )


CONFIGURE_FILE(
    "src/Info.plist"
    "Info.plist.in"
    COPYONLY
)
