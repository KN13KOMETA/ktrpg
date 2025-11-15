cmake_path(SET LUA_SOURCE_DIR "${SOURCE_FILES_DIR}/lua")
cmake_path(SET LUAH_SOURCE_DIR "${SOURCE_FILES_DIR}/luah")

set_property(
  TARGET ${PROJECT_NAME}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${LUAH_SOURCE_DIR}
)

function(generate_header_from_lua input_file)
  # Get relative path to LUA_SOURCE_DIR
  cmake_path(
    RELATIVE_PATH input_file
    BASE_DIRECTORY ${LUA_SOURCE_DIR}
    OUTPUT_VARIABLE relative_input_path
  )

  # Set output_file
  cmake_path(
    ABSOLUTE_PATH relative_input_path
    BASE_DIRECTORY ${LUAH_SOURCE_DIR}
    OUTPUT_VARIABLE output_file
  )
  cmake_path(REPLACE_EXTENSION output_file "h" OUTPUT_VARIABLE output_file)

  # Set HEADER_GUARD
  cmake_path(
    RELATIVE_PATH output_file
    BASE_DIRECTORY ${SOURCE_FILES_DIR}
    OUTPUT_VARIABLE HEADER_GUARD
  )
  string(REPLACE "." "_" HEADER_GUARD ${HEADER_GUARD})
  string(REPLACE "/" "_" HEADER_GUARD ${HEADER_GUARD})
  string(REPLACE "\\" "_" HEADER_GUARD ${HEADER_GUARD})
  string(TOUPPER ${HEADER_GUARD} HEADER_GUARD)

  # Set VARIABLE_NAME
  string(REPLACE "." "_" VARIABLE_NAME ${relative_input_path})
  string(REPLACE "/" "_" VARIABLE_NAME ${VARIABLE_NAME})
  string(REPLACE "\\" "_" VARIABLE_NAME ${VARIABLE_NAME})

  # Read and Write file
  file(READ ${input_file} FILE_CONTENT)

  # Escape \, " and \n
  string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
  string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
  string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

  # Add command instead of writing file here
  add_custom_command(
    OUTPUT ${output_file}
    COMMAND
      ${CMAKE_COMMAND} -DINPUT_FILE=${input_file} -DOUTPUT_FILE=${output_file}
      -DHEADER_GUARD=${HEADER_GUARD} -DVARIABLE_NAME=${VARIABLE_NAME} -P
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/write_lua_header.cmake
    DEPENDS ${input_file}
    COMMENT "Generating ${output_file} from ${input_file}"
    VERBATIM
  )

  # Add file to GENERATED so "clean" works
  set_source_files_properties(${output_file} PROPERTIES GENERATED TRUE)

  message(STATUS "Registered dependency: ${input_file} -> ${output_file}")
endfunction()
