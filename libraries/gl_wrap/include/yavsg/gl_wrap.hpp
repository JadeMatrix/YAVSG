#pragma once


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

#include <functional>


namespace JadeMatrix::yavsg::gl
{
    // TODO: Generate this at build time instead of using a macro (to improve
    //      compile times)
    #define YAVSG_FOREACH_SUPPORTED_GL_FUNCTION( MACRO ) \
        MACRO( ActiveTexture, void, GLenum /* texture */ ) \
        MACRO( AttachShader, void, GLuint /* program */, GLuint /* shader */ ) \
        MACRO( BindBuffer, void, GLenum /* target */, GLuint /* buffer */ ) \
        MACRO( BindFragDataLocation, void, GLuint /* program */, GLuint /* colorNumber */, char const* /* name */ ) \
        MACRO( BindFramebuffer, void, GLenum /* target */, GLuint /* framebuffer */ ) \
        MACRO( BindTexture, void, GLenum /* target */, GLuint /* texture */ ) \
        MACRO( BindVertexArray, void, GLuint /* array */ ) \
        MACRO( BlendEquationSeparate, void, GLenum /* modeRGB */, GLenum /* modeAlpha */ ) \
        MACRO( BlendEquationSeparatei, void, GLuint /* buf */, GLenum /* modeRGB */, GLenum /* modeAlpha */ ) \
        MACRO( BlendFuncSeparate, void, GLenum /* srcRGB */, GLenum /* dstRGB */, GLenum /* srcAlpha */, GLenum /* dstAlpha */ ) \
        MACRO( BlendFuncSeparatei, void, GLuint /* buf */, GLenum /* srcRGB */, GLenum /* dstRGB */, GLenum /* srcAlpha */, GLenum /* dstAlpha */ ) \
        MACRO( BufferData, void, GLenum /* target */, GLsizeiptr /* size */, void const* /* data */, GLenum /* usage */ ) \
        MACRO( CheckFramebufferStatus, GLenum, GLenum /* target */ ) \
        MACRO( Clear, void, GLbitfield /* mask */ ) \
        MACRO( ClearColor, void, GLfloat /* red */, GLfloat /* green */, GLfloat /* blue */, GLfloat /* alpha */ ) \
        MACRO( CompileShader, void, GLuint /* shader */ ) \
        MACRO( CreateProgram, GLuint, void ) \
        MACRO( CreateShader, GLuint, GLenum /* shaderType */ ) \
        MACRO( DeleteBuffers, void, GLsizei /* n */, GLuint const* /* buffers */ ) \
        MACRO( DeleteFramebuffers, void, GLsizei /* n */, GLuint* /* framebuffers */ ) \
        MACRO( DeleteProgram, void, GLuint /* program */ ) \
        MACRO( DeleteShader, void, GLuint /* shader */ ) \
        MACRO( DeleteTextures, void, GLsizei /* n */, GLuint const* /* textures */ ) \
        MACRO( DeleteVertexArrays, void, GLsizei /* n */, GLuint const* /* arrays */ ) \
        MACRO( Disable, void, GLenum /* cap */ ) \
        MACRO( Disablei, void, GLenum /* cap */, GLuint /* index */ ) \
        MACRO( DrawArrays, void, GLenum /* mode */, GLint /* first */, GLsizei /* count */ ) \
        MACRO( DrawBuffers, void, GLsizei /* n */, GLenum const* /* bufs */ ) \
        MACRO( DrawElements, void, GLenum /* mode */, GLsizei /* count */, GLenum /* type */, void const* /* indices */ ) \
        MACRO( Enable, void, GLenum /* cap */ ) \
        MACRO( EnableVertexAttribArray, void, GLuint /* index */ ) \
        MACRO( Enablei, void, GLenum /* cap */, GLuint /* index */ ) \
        MACRO( FramebufferTexture2D, void, GLenum /* target */, GLenum /* attachment */, GLenum /* textarget */, GLuint /* texture */, GLint /* level */ ) \
        MACRO( GenBuffers, void, GLsizei /* n */, GLuint* /* buffers */ ) \
        MACRO( GenFramebuffers, void, GLsizei /* n */, GLuint* /* ids */ ) \
        MACRO( GenTextures, void, GLsizei /* n */, GLuint* /* textures */ ) \
        MACRO( GenVertexArrays, void, GLsizei /* n */, GLuint* /* arrays */ ) \
        MACRO( GenerateMipmap, void, GLenum /* target */ ) \
        MACRO( GetAttribLocation, GLint, GLuint /* program */, GLchar const* /* name */ ) \
        MACRO( GetError, GLenum, void ) \
        MACRO( GetFloatv, void, GLenum /* pname */, GLfloat* /* data */ ) \
        MACRO( GetIntegerv, void, GLenum /* pname */, GLint* /* data */ ) \
        MACRO( GetProgramInfoLog, void, GLuint /* program */, GLsizei /* maxLength */, GLsizei* /* length */, GLchar* /* infoLog */ ) \
        MACRO( GetProgramiv, void, GLuint /* program */, GLenum /* pname */, GLint* /* params */ ) \
        MACRO( GetShaderInfoLog, void, GLuint /* shader */, GLsizei /* maxLength */, GLsizei* /* length */, GLchar* /* infoLog */ ) \
        MACRO( GetShaderiv, void, GLuint /* shader */, GLenum /* pname */, GLint* /* params */ ) \
        MACRO( GetUniformLocation, GLint, GLuint /* program */, GLchar const* /* name */ ) \
        MACRO( LinkProgram, void, GLuint /* program */ ) \
        MACRO( ReadPixels, void, GLint /* x */, GLint /* y */, GLsizei /* width */, GLsizei /* height */, GLenum /* format */, GLenum /* type */, void* /* data */ ) \
        MACRO( Scissor, void, GLint /* x */, GLint /* y */, GLsizei /* width */, GLsizei /* height */ ) \
        MACRO( ShaderSource, void, GLuint /* shader */, GLsizei /* count */, GLchar const** /* string */, GLint const* /* length */ ) \
        MACRO( TexImage2D, void, GLenum /* target */, GLint /* level */, GLint /* internalFormat */, GLsizei /* width */, GLsizei /* height */, GLint /* border */, GLenum /* format */, GLenum /* type */, void const* /* data */ ) \
        MACRO( TexParameterf, void, GLenum /* target */, GLenum /* pname */, GLfloat /* param */ ) \
        MACRO( TexParameteri, void, GLenum /* target */, GLenum /* pname */, GLint /* param */ ) \
        MACRO( Uniform1fv, void, GLint /* location */, GLsizei /* count */, GLfloat const* /* value */ ) \
        MACRO( Uniform2fv, void, GLint /* location */, GLsizei /* count */, GLfloat const* /* value */ ) \
        MACRO( Uniform3fv, void, GLint /* location */, GLsizei /* count */, GLfloat const* /* value */ ) \
        MACRO( Uniform4fv, void, GLint /* location */, GLsizei /* count */, GLfloat const* /* value */ ) \
        MACRO( Uniform1iv, void, GLint /* location */, GLsizei /* count */, GLint const* /* value */ ) \
        MACRO( Uniform2iv, void, GLint /* location */, GLsizei /* count */, GLint const* /* value */ ) \
        MACRO( Uniform3iv, void, GLint /* location */, GLsizei /* count */, GLint const* /* value */ ) \
        MACRO( Uniform4iv, void, GLint /* location */, GLsizei /* count */, GLint const* /* value */ ) \
        MACRO( Uniform1uiv, void, GLint /* location */, GLsizei /* count */, GLuint const* /* value */ ) \
        MACRO( Uniform2uiv, void, GLint /* location */, GLsizei /* count */, GLuint const* /* value */ ) \
        MACRO( Uniform3uiv, void, GLint /* location */, GLsizei /* count */, GLuint const* /* value */ ) \
        MACRO( Uniform4uiv, void, GLint /* location */, GLsizei /* count */, GLuint const* /* value */ ) \
        MACRO( UniformMatrix2fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix3fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix4fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix2x3fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix3x2fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix2x4fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix4x2fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix3x4fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( UniformMatrix4x3fv, void, GLint /* location */, GLsizei /* count */, GLboolean /* transpose */, GLfloat const* /* value */ ) \
        MACRO( Uniform1f, void, GLint /* location */, GLfloat /* v0 */ ) \
        MACRO( Uniform1i, void, GLint /* location */, GLint /* v0 */ ) \
        MACRO( Uniform1ui, void, GLint /* location */, GLuint /* v0 */ ) \
        MACRO( UseProgram, void, GLuint /* program */ ) \
        MACRO( VertexAttribPointer, void, GLuint /* index */, GLint /* size */, GLenum /* type */, GLboolean /* normalized */, GLsizei /* stride */, void const* /* pointer */ ) \
        MACRO( Viewport, void, GLint /* x */, GLint /* y */, GLsizei /* width */, GLsizei /* height */ )
    
    #define DECLARE_GL_WRAPPER( NAME, RETURN, ... ) \
        inline std::function< RETURN( __VA_ARGS__ ) > NAME = gl##NAME;
    YAVSG_FOREACH_SUPPORTED_GL_FUNCTION( DECLARE_GL_WRAPPER )
    #undef DECLARE_GL_WRAPPER
}
