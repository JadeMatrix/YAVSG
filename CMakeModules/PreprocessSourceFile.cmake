# Preprocess source files to add relative filenames for log messages

FILE( READ "${INPUT_FILENAME}" CONTENT )

FILE( RELATIVE_PATH RELATIVE_FILENAME
    "${PROJECT_SOURCE_DIR}"
    "${INPUT_FILENAME}"
)

FILE( WRITE  "${OUTPUT_FILENAME}" "#line 1 \"${RELATIVE_FILENAME}\"\n" )
FILE( APPEND "${OUTPUT_FILENAME}" "${CONTENT}"                         )
