CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )


PROJECT( "YetAnotherVoxelSpessGame" )


SET( CMAKE_CXX_STANDARD          17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )


LIST( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/" )


# Bootstrap `JadeMatrix-CMake-Modules` #########################################

LIST( APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/dependencies/JadeMatrix/JadeMatrix-CMake-Modules/CMakeModules/"
)
INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings
    OPTIONAL
    RESULT_VARIABLE JMCMM_INCLUDED
)
IF( NOT JMCMM_INCLUDED )
    FIND_PACKAGE( JadeMatrix-CMake-Modules 1.5 REQUIRED )
    INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings )
ENDIF()


# Find dependencies ############################################################

FIND_PACKAGE( GLEW                      REQUIRED COMPONENTS GLEW          )
FIND_PACKAGE( OpenGL                    REQUIRED COMPONENTS #[[ GL ]]     )
FIND_PACKAGE( SDL2          2.0.8       REQUIRED COMPONENTS sdl image     )
FIND_PACKAGE( tinyobjloader 1.0.4 EXACT REQUIRED COMPONENTS tinyobjloader )


# Set up project targets #######################################################

ADD_SUBDIRECTORY( "libraries/" )
ADD_SUBDIRECTORY( "programs/" )
