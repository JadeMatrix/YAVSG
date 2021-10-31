CMAKE_MINIMUM_REQUIRED( VERSION 3.12 )


PROJECT( "YetAnotherVoxelSpessGame" )


# DEBUG:
SET( CMAKE_BUILD_TYPE Debug )

SET( CMAKE_CXX_STANDARD          17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )


LIST( APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/dependencies/JadeMatrix/JadeMatrix-CMake-Modules/CMakeModules/"
)
INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings
    OPTIONAL
    RESULT_VARIABLE JMCMM_INCLUDED
)
IF( NOT JMCMM_INCLUDED )
    FIND_PACKAGE( JadeMatrix-CMake-Modules 1.4 REQUIRED )
    INCLUDE( JadeMatrix-CMake-Modules/Util/CompileWarnings )
ENDIF()


LIST( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/" )

ADD_SUBDIRECTORY( "libraries/" )
ADD_SUBDIRECTORY( "programs/" )
