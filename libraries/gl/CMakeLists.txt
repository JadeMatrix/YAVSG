ADD_LIBRARY( gl )
TARGET_SOURCES( gl PRIVATE
    "src/error.cpp"
    "src/framebuffer.cpp"
    "src/shader.cpp"
    "src/texture_utilities.cpp"
)
TARGET_INCLUDE_DIRECTORIES( gl PUBLIC "include/" )
TARGET_LINK_LIBRARIES( gl PUBLIC gl_wrap math sdl )
TARGET_COMPILE_OPTIONS( gl PRIVATE ${YAVSG_COMPILE_OPTIONS} )
