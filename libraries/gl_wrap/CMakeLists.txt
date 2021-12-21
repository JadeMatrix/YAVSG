ADD_LIBRARY( gl_wrap )

SET( HEADERS
    "include/yavsg/gl/error.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/include/yavsg/gl_wrap.hpp"
)
SET( SOURCES
    "${CMAKE_CURRENT_BINARY_DIR}/src/gl_wrap.cpp"
)
TARGET_SOURCES( gl_wrap PRIVATE ${HEADERS} ${SOURCES} )
SOURCE_GROUP( "C++ Headers" FILES ${HEADERS} )
SOURCE_GROUP( "C++ Sources" FILES ${SOURCES} )

TARGET_INCLUDE_DIRECTORIES( gl_wrap
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>"
        "$<INSTALL_INTERFACE:include/>"
)

TARGET_LINK_LIBRARIES( gl_wrap
    PUBLIC
        JadeMatrix::ext
        GLEW::GLEW
        OpenGL::GL
    PRIVATE
        logging
        fmt::fmt
)


SET( GL_WRAP_WRAPPER_DECLARATIONS "" )
SET( GL_WRAP_WRAPPER_DEFINITIONS  "" )

FOREACH( GL_FUNC IN ITEMS
    "ActiveTexture,void,::GLenum texture"
    "AttachShader,void,::GLuint program,::GLuint shader"
    "BindBuffer,void,::GLenum target,::GLuint buffer"
    "BindFragDataLocation,void,::GLuint program,::GLuint colorNumber,char const* name"
    "BindFramebuffer,void,::GLenum target,::GLuint framebuffer"
    "BindTexture,void,::GLenum target,::GLuint texture"
    "BindVertexArray,void,::GLuint array"
    "BlendEquationSeparate,void,::GLenum modeRGB,::GLenum modeAlpha"
    "BlendEquationSeparatei,void,::GLuint buf,::GLenum modeRGB,::GLenum modeAlpha"
    "BlendFuncSeparate,void,::GLenum srcRGB,::GLenum dstRGB,::GLenum srcAlpha,::GLenum dstAlpha"
    "BlendFuncSeparatei,void,::GLuint buf,::GLenum srcRGB,::GLenum dstRGB,::GLenum srcAlpha,::GLenum dstAlpha"
    "BufferData,void,::GLenum target,::GLsizeiptr size,void const* data,::GLenum usage"
    "CheckFramebufferStatus,::GLenum,::GLenum target"
    "Clear,void,::GLbitfield mask"
    "ClearColor,void,::GLfloat red,::GLfloat green,::GLfloat blue,::GLfloat alpha"
    "CompileShader,void,::GLuint shader"
    "CreateProgram,::GLuint"
    "CreateShader,::GLuint,::GLenum shaderType"
    "DeleteBuffers,void,::GLsizei n,::GLuint const* buffers"
    "DeleteFramebuffers,void,::GLsizei n,::GLuint* framebuffers"
    "DeleteProgram,void,::GLuint program"
    "DeleteShader,void,::GLuint shader"
    "DeleteTextures,void,::GLsizei n,::GLuint const* textures"
    "DeleteVertexArrays,void,::GLsizei n,::GLuint const* arrays"
    "Disable,void,::GLenum cap"
    "Disablei,void,::GLenum cap,::GLuint index"
    "DrawArrays,void,::GLenum mode,::GLint first,::GLsizei count"
    "DrawBuffers,void,::GLsizei n,::GLenum const* bufs"
    "DrawElements,void,::GLenum mode,::GLsizei count,::GLenum type,void const* indices"
    "Enable,void,::GLenum cap"
    "EnableVertexAttribArray,void,::GLuint index"
    "Enablei,void,::GLenum cap,::GLuint index"
    "FramebufferTexture2D,void,::GLenum target,::GLenum attachment,::GLenum textarget,::GLuint texture,::GLint level"
    "GenBuffers,void,::GLsizei n,::GLuint* buffers"
    "GenFramebuffers,void,::GLsizei n,::GLuint* ids"
    "GenTextures,void,::GLsizei n,::GLuint* textures"
    "GenVertexArrays,void,::GLsizei n,::GLuint* arrays"
    "GenerateMipmap,void,::GLenum target"
    "GetAttribLocation,::GLint,::GLuint program,::GLchar const* name"
    "GetFloatv,void,::GLenum pname,::GLfloat* data"
    "GetIntegerv,void,::GLenum pname,::GLint* data"
    "GetProgramInfoLog,void,::GLuint program,::GLsizei maxLength,::GLsizei* length,::GLchar* infoLog"
    "GetProgramiv,void,::GLuint program,::GLenum pname,::GLint* params"
    "GetShaderInfoLog,void,::GLuint shader,::GLsizei maxLength,::GLsizei* length,::GLchar* infoLog"
    "GetShaderiv,void,::GLuint shader,::GLenum pname,::GLint* params"
    "GetUniformLocation,::GLint,::GLuint program,::GLchar const* name"
    "LinkProgram,void,::GLuint program"
    "ReadPixels,void,::GLint x,::GLint y,::GLsizei width,::GLsizei height,::GLenum format,::GLenum type,void* data"
    "Scissor,void,::GLint x,::GLint y,::GLsizei width,::GLsizei height"
    "ShaderSource,void,::GLuint shader,::GLsizei count,::GLchar const** string,::GLint const* length"
    "TexImage2D,void,::GLenum target,::GLint level,::GLint internalFormat,::GLsizei width,::GLsizei height,::GLint border,::GLenum format,::GLenum type,void const* data"
    "TexParameterf,void,::GLenum target,::GLenum pname,::GLfloat param"
    "TexParameteri,void,::GLenum target,::GLenum pname,::GLint param"
    "Uniform1fv,void,::GLint location,::GLsizei count,::GLfloat const* value"
    "Uniform2fv,void,::GLint location,::GLsizei count,::GLfloat const* value"
    "Uniform3fv,void,::GLint location,::GLsizei count,::GLfloat const* value"
    "Uniform4fv,void,::GLint location,::GLsizei count,::GLfloat const* value"
    "Uniform1iv,void,::GLint location,::GLsizei count,::GLint const* value"
    "Uniform2iv,void,::GLint location,::GLsizei count,::GLint const* value"
    "Uniform3iv,void,::GLint location,::GLsizei count,::GLint const* value"
    "Uniform4iv,void,::GLint location,::GLsizei count,::GLint const* value"
    "Uniform1uiv,void,::GLint location,::GLsizei count,::GLuint const* value"
    "Uniform2uiv,void,::GLint location,::GLsizei count,::GLuint const* value"
    "Uniform3uiv,void,::GLint location,::GLsizei count,::GLuint const* value"
    "Uniform4uiv,void,::GLint location,::GLsizei count,::GLuint const* value"
    "UniformMatrix2fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix3fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix4fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix2x3fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix3x2fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix2x4fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix4x2fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix3x4fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "UniformMatrix4x3fv,void,::GLint location,::GLsizei count,::GLboolean transpose,::GLfloat const* value"
    "Uniform1f,void,::GLint location,::GLfloat v0"
    "Uniform1i,void,::GLint location,::GLint v0"
    "Uniform1ui,void,::GLint location,::GLuint v0"
    "UseProgram,void,::GLuint program"
    "VertexAttribPointer,void,::GLuint index,::GLint size,::GLenum type,::GLboolean normalized,::GLsizei stride,void const* pointer"
    "Viewport,void,::GLint x,::GLint y,::GLsizei width,::GLsizei height"
)
    STRING( REPLACE "," ";" GL_FUNC "${GL_FUNC}" )
    
    LIST( LENGTH GL_FUNC GL_FUNC_NUM_PARTS )
    IF( GL_FUNC_NUM_PARTS LESS 2 )
        MESSAGE( FATAL_ERROR "Invalid GL function (${GL_FUNC})" )
    ENDIF()
    
    LIST( GET GL_FUNC 0 GL_FUNC_BASENAME   )
    LIST( GET GL_FUNC 1 GL_FUNC_RETURNTYPE )
    IF( GL_FUNC_NUM_PARTS GREATER 2 )
        LIST( SUBLIST GL_FUNC 2 -1 GL_FUNC_ARGUMENTS )
    ELSE()
        SET( GL_FUNC_ARGUMENTS )
    ENDIF()
    
    IF( GL_FUNC_BASENAME STREQUAL "GetError" )
        MESSAGE( FATAL_ERROR "glGetError must not be wrapped" )
    ENDIF()
    
    SET( GL_FUNC_ARGS_SIGNATURE )
    SET( GL_FUNC_ARGS_FORWARD   )
    SET( GL_FUNC_ARGS_FORMAT    )
    SET( GL_FUNC_ARGS_FMT_ARGS  )
    foreach( ARG IN LISTS GL_FUNC_ARGUMENTS )
        STRING( REGEX REPLACE [[ +]] ";" ARG "${ARG}" )
        
        LIST( LENGTH ARG ARG_NUM_PARTS )
        IF( ARG_NUM_PARTS LESS 2 )
            MESSAGE( FATAL_ERROR "Invalid GL function (${GL_FUNC})" )
        ENDIF()
        
        LIST( POP_BACK ARG ARG_NAME )
        LIST( JOIN ARG " " ARG_TYPE )
        
        STRING( APPEND GL_FUNC_ARGS_SIGNATURE "${ARG_TYPE} ${ARG_NAME}, " )
        LIST  ( APPEND GL_FUNC_ARGS_FORWARD               "${ARG_NAME}"   )
        
        IF( ARG_TYPE STREQUAL "::GLenum" )
            LIST( APPEND GL_FUNC_ARGS_FORMAT "(GLenum)0x{:X}" )
        ELSE()
            LIST( APPEND GL_FUNC_ARGS_FORMAT "{}" )
        ENDIF()
        
        IF( ARG_TYPE MATCHES [[char const\*$]] )
            STRING( APPEND GL_FUNC_ARGS_FMT_ARGS
                ", std::quoted( ${ARG_NAME} )"
            )
        ELSEIF( ARG_TYPE MATCHES [[\*]] )
            STRING( APPEND GL_FUNC_ARGS_FMT_ARGS
                ", static_cast< void const* >( ${ARG_NAME} )"
            )
        ELSE()
            STRING( APPEND GL_FUNC_ARGS_FMT_ARGS ", ${ARG_NAME}" )
        ENDIF()
    endforeach()
    
    LIST( JOIN GL_FUNC_ARGS_FORWARD ", " GL_FUNC_ARGS_FORWARD )
    LIST( JOIN GL_FUNC_ARGS_FORMAT  ", " GL_FUNC_ARGS_FORMAT  )
    
    STRING( APPEND GL_WRAP_WRAPPER_DECLARATIONS "    " )
    IF( NOT GL_FUNC_RETURNTYPE STREQUAL "void" )
        STRING( APPEND GL_WRAP_WRAPPER_DECLARATIONS "[[nodiscard]] " )
    ENDIF()
    
    STRING( APPEND GL_WRAP_WRAPPER_DECLARATIONS
        "${GL_FUNC_RETURNTYPE} ${GL_FUNC_BASENAME}( "
        "${GL_FUNC_ARGS_SIGNATURE}ext::source_location = "
        "ext::source_location::current() );\n"
    )
    STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS
        "${GL_FUNC_RETURNTYPE} JadeMatrix::yavsg::gl::${GL_FUNC_BASENAME}( "
        "${GL_FUNC_ARGS_SIGNATURE}ext::source_location where_ )\n"
        "{\n"
        "    if( ::log_every_opengl_call )\n"
        "    {\n"
        "        log_.verbose(\n"
        "            {\n"
        "                \"gl${GL_FUNC_BASENAME}(${GL_FUNC_ARGS_FORMAT})\"sv,\n"
        "                std::chrono::system_clock::now(),\n"
        "                where_,\n"
        "            }\n"
        "            ${GL_FUNC_ARGS_FMT_ARGS}\n"
        "        );\n"
        "    }\n"
        "    "
    )
    IF( NOT GL_FUNC_RETURNTYPE STREQUAL "void" )
        STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS
            "auto return_value_ = gl${GL_FUNC_BASENAME}( "
            "${GL_FUNC_ARGS_FORWARD} );"
        )
    ELSE()
        STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS
            "gl${GL_FUNC_BASENAME}( ${GL_FUNC_ARGS_FORWARD} );"
        )
    ENDIF()
    STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS
        "\n"
        "    if( ::check_opengl_errors )\n"
        "    {\n"
        "        if( auto summaries_ = pop_opengl_error_summaries(); summaries_ )\n"
        "        {\n"
        "            throw gl::error( fmt::format(\n"
        "                \"gl${GL_FUNC_BASENAME}(${GL_FUNC_ARGS_FORMAT}) failed for {} ({}:{}): {}\"sv\n"
        "                ${GL_FUNC_ARGS_FMT_ARGS},\n"
        "                where_.function_name,\n"
        "                where_.file_name,\n"
        "                where_.line,\n"
        "                *summaries_\n"
        "            ) );\n"
        "        }\n"
        "    }\n"
    )
    IF( NOT GL_FUNC_RETURNTYPE STREQUAL "void" )
        STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS "    return return_value_;" )
    ENDIF()
    STRING( APPEND GL_WRAP_WRAPPER_DEFINITIONS "}\n\n" )
ENDFOREACH()


CONFIGURE_FILE(
    "config/gl_wrap.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/yavsg/gl_wrap.hpp"
    @ONLY
)
CONFIGURE_FILE(
    "config/gl_wrap.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src/gl_wrap.cpp"
    @ONLY
)
