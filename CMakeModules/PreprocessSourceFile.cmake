# Preprocess source files to add relative filenames for log messages

FILE( READ "${INPUT_FILENAME}" CONTENT )

FILE( RELATIVE_PATH RELATIVE_FILENAME
    "${PROJECT_SOURCE_DIR}"
    "${INPUT_FILENAME}"
)

IF( RELATIVE_FILENAME MATCHES [[^\.\./.*]] )
    FILE( RELATIVE_PATH BUILD_RELATIVE_FILENAME
        "${PROJECT_BINARY_DIR}"
        "${INPUT_FILENAME}"
    )
    IF( BUILD_RELATIVE_FILENAME MATCHES [[^\.\./.*]] )
        # Fallback to absolute path if source file is external to the project
        SET( RELATIVE_FILENAME "${INPUT_FILENAME}" )
    ELSE()
        SET( RELATIVE_FILENAME "${BUILD_RELATIVE_FILENAME}" )
    ENDIF()
ENDIF()

FILE( WRITE  "${OUTPUT_FILENAME}" "#line 1 \"${RELATIVE_FILENAME}\"\n" )
FILE( APPEND "${OUTPUT_FILENAME}" "${CONTENT}"                         )
