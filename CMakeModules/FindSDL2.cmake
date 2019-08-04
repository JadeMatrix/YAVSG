INCLUDE( YAVSGFindUtilities )

FIND_ORIGINAL_PACKAGE( SDL2 SKIP_VERSION )


SET( _SDL2_REQUIRED_VARS SDL2_VERSION )
IF( SDL2_FIND_COMPONENTS AND NOT SDL2_FIND_COMPONENTS STREQUAL "config" )
    # A.k.a any library components requested
    SET( _SDL2_NEED_INCLUDES TRUE )
ENDIF()
IF( _SDL2_NEED_INCLUDES )
    LIST( APPEND _SDL2_REQUIRED_VARS SDL2_INCLUDE_DIRS )
ENDIF()


IF( DEFINED SDL2_PREFIX )
    # The official SDL2 Config script is pretty bare-bones
    SET( _SDL2_LIBRARY_FIND_HINTS
        HINTS "${SDL2_LIBDIR}"
    )
    SET( _SDL2_PROGRAM_FIND_HINTS
        HINTS "${SDL2_EXEC_PREFIX}"
        PATH_SUFFIXES "bin"
    )
    SET( SDL2_INCLUDE_DIRS
        "${SDL2_INCLUDE_DIRS}"
        CACHE PATH "Location of SDl2 headers"
    )
ELSE()
    IF( _SDL2_NEED_INCLUDES )
        FIND_PATH(
            SDL2_INCLUDE_DIRS
            "SDL.h"
            DOC "Location of SDl2 headers"
        )
    ENDIF()
ENDIF()


IF( NOT "config" IN_LIST SDL2_FIND_COMPONENTS )
    LIST( APPEND SDL2_FIND_COMPONENTS "config" )
ENDIF()
IF( NOT SDL2_FIND_REQUIRED_config )
    SET( SDL2_FIND_REQUIRED_config TRUE )
ENDIF()


FIND_PROGRAM(
    SDL2_CONFIG_LOCATION
    NAMES "sdl2-config"
    ${_SDL2_PROGRAM_FIND_HINTS}
    DOC "Location of the `sdl2-config` executable"
)
LIST( APPEND _SDL2_REQUIRED_VARS SDL2_CONFIG_LOCATION )
IF( SDL2_CONFIG_LOCATION )
    ADD_EXECUTABLE( SDL2::config IMPORTED )
    SET_TARGET_PROPERTIES( SDL2::config
        PROPERTIES
            IMPORTED_LOCATION "${SDL2_CONFIG_LOCATION}"
    )
    EXECUTE_PROCESS(
        COMMAND "${SDL2_CONFIG_LOCATION}" "--version"
        OUTPUT_VARIABLE SDL2_VERSION
    )
    SET( SDL2_config_FOUND TRUE )
ENDIF()


MACRO( _SDL2_ADD_COMPONENT_LIBRARY COMPONENT TYPE )
    CMAKE_PARSE_ARGUMENTS(
        _SDL2_ADD_COMPONENT_LIBRARY
        ""
        "DOC"
        "NAMES"
        ${ARGN}
    )
    
    IF( DEFINED _SDL2_ADD_COMPONENT_LIBRARY_DOC )
        SET( _SDL2_ADD_COMPONENT_LIBRARY_DOC
            "DOC" "${_SDL2_ADD_COMPONENT_LIBRARY_DOC}"
        )
    ENDIF()
    
    IF( "${COMPONENT}" IN_LIST SDL2_FIND_COMPONENTS )
        FIND_LIBRARY(
            SDL2_${COMPONENT}_LOCATION
            NAMES ${_SDL2_ADD_COMPONENT_LIBRARY_NAMES}
            ${_SDL2_LIBRARY_FIND_HINTS}
            ${_SDL2_ADD_COMPONENT_LIBRARY_DOC}
        )
        IF( SDL2_${COMPONENT}_LOCATION )
            ADD_LIBRARY( SDL2::${COMPONENT} ${TYPE} IMPORTED )
            SET_TARGET_PROPERTIES( SDL2::${COMPONENT}
                PROPERTIES
                    IMPORTED_LOCATION "${SDL2_${COMPONENT}_LOCATION}"
                    INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIRS}
                    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            )
            LIST( APPEND _SDL2_REQUIRED_VARS SDL2_${COMPONENT}_LOCATION )
            SET( SDL2_${COMPONENT}_FOUND TRUE )
        ENDIF()
    ENDIF()
ENDMACRO()

_SDL2_ADD_COMPONENT_LIBRARY(
    sdl
    SHARED
    NAMES
        "libSDL2.so"
        "libSDL2.dll"
        "libSDL2.dylib"
    DOC "Location of `libSDL2` (shared)"
)
_SDL2_ADD_COMPONENT_LIBRARY(
    sdl-static
    STATIC
    NAMES
        "libSDL2.a"
        "libSDL2.lib"
    DOC "Location of `libSDL2` (static)"
)
_SDL2_ADD_COMPONENT_LIBRARY(
    main
    UNKNOWN
    NAMES "SDL2main"
    DOC "Location of `libSDL2main`"
)
_SDL2_ADD_COMPONENT_LIBRARY(
    test
    UNKNOWN
    NAMES "SDL2_test"
    DOC "Location of `libSDL2_test`"
)
_SDL2_ADD_COMPONENT_LIBRARY(
    image
    SHARED
    NAMES
        "libSDL2_image.so"
        "libSDL2_image.dll"
        "libSDL2_image.dylib"
    DOC "Location of `libSDL2_image` (shared)"
)
_SDL2_ADD_COMPONENT_LIBRARY(
    image-static
    STATIC
    NAMES
        "libSDL2_image.a"
        "libSDL2_image.lib"
    DOC "Location of `libSDL2_image` (static)"
)


INCLUDE( FindPackageHandleStandardArgs )
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    SDL2
    REQUIRED_VARS ${_SDL2_REQUIRED_VARS}
    HANDLE_COMPONENTS
)
