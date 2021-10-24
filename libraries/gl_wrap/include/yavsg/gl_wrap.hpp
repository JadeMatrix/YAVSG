#pragma once
#ifndef YAVSG_GL_WRAP_GL_HPP
#define YAVSG_GL_WRAP_GL_HPP


// As including OpenGL can be finnicky (order-dependent, breaks on multiple-
// include, platform specific, etc.), this header ensures OpenGL is included
// properly in any source file that requires access to its types and functions.

// See https://gist.github.com/cbmeeks/5587a11e7856baf819b7
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <OpenGL/gl3.h>
    #include <OpenGL/gl3ext.h>
#else
    #include <GL/glew.h>
#endif


#endif
