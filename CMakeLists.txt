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

# TODO:

# First-party

# TODO:


# Set up project targets #######################################################

INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings )

ADD_SUBDIRECTORY( "libraries/" )
ADD_SUBDIRECTORY( "programs/" )
