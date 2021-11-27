CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )


PROJECT( "YetAnotherVoxelSpessGame" )


SET( CMAKE_CXX_STANDARD          17  )
SET( CMAKE_CXX_STANDARD_REQUIRED ON  )
SET( CMAKE_CXX_EXTENSIONS        OFF )


LIST( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/" )


# Bootstrap methods for finding dependencies ###################################

IF( EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
    INCLUDE( "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake" )
    CONAN_SET_FIND_PATHS()
ELSE()
    LIST( APPEND CMAKE_MODULE_PATH
        "${PROJECT_SOURCE_DIR}/dependencies/JadeMatrix/JadeMatrix-CMake-Modules/CMakeModules/"
    )
ENDIF()

INCLUDE( JadeMatrix-CMake-Modules/Util/AddDependency
    OPTIONAL
    RESULT_VARIABLE JMCMM_INCLUDED
)
IF( NOT JMCMM_INCLUDED )
    FIND_PACKAGE( JadeMatrix-CMake-Modules 1.7 REQUIRED )
    INCLUDE( JadeMatrix-CMake-Modules/Util/AddDependency )
ENDIF()


# Find dependencies ############################################################

# System & third-party not (yet) handled by `ADD_DEPENDENCY()`

FIND_PACKAGE( GLEW                      REQUIRED COMPONENTS GLEW          )
FIND_PACKAGE( OpenGL                    REQUIRED COMPONENTS #[[ GL ]]     )
FIND_PACKAGE( SDL2          2.0.8       REQUIRED COMPONENTS sdl image     )
FIND_PACKAGE( tinyobjloader 1.0.4 EXACT REQUIRED COMPONENTS tinyobjloader )

# Third-party via `ADD_DEPENDENCY()`

ADD_DEPENDENCY( fmt
    REQUIRED
    VERSION      7.1.3
    SUBDIRECTORY "dependencies/fmtlib/fmt/"
    COMPONENTS   fmt
)

# First-party

SET( JM_EXT_USE_STD_FORMAT FALSE )
ADD_DEPENDENCY( JadeMatrix-ext
    REQUIRED
    VERSION      0.2.1
    SUBDIRECTORY "dependencies/JadeMatrix/ext/"
    NAMESPACE    JadeMatrix::
    COMPONENTS   ext
)


# Set up project targets #######################################################

ADD_SUBDIRECTORY( "libraries/" )
ADD_SUBDIRECTORY( "programs/"  )

INCLUDE( JadeMatrix-CMake-Modules/Util/TargetUtilities )
INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings )

LIST_TARGETS( PROJECT_TARGETS
    DIRECTORIES
        "libraries/"
        "programs/"
    CATEGORIES  BUILDSYSTEM
)
LIST_TARGETS( ALL_TARGETS
    CATEGORIES  BUILDSYSTEM
)

FOREACH( TARGET IN LISTS PROJECT_TARGETS )
    GET_TARGET_PROPERTY( TARGET_TYPE "${TARGET}" TYPE )
    IF( NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY" )
        SET_ALL_COMPILE_WARNINGS( "${TARGET}" AS_ERRORS )
    ENDIF()
ENDFOREACH()

FOREACH( TARGET IN LISTS ALL_TARGETS )
    PREPROCESS_TARGET_SOURCES( "${TARGET}"
        SCRIPT "CMakeModules/PreprocessSourceFile.cmake"
        NO_SKIP_INCLUDES
    )
ENDFOREACH()
