cmake_path(SET LUA_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
cmake_path(SET LUAH_SOURCE_DIR "${SOURCE_FILES_DIR}/luah")

set_property(
  TARGET ${PROJECT_NAME}
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${LUAH_SOURCE_DIR}
)

function(print_comments_target target_name comment_type)
  set(
    COMMENT_EXCLUDE
    --exclude-dir
    .git
    --exclude-dir
    .cache
    --exclude-dir
    build
    --exclude-dir
    libs
  )

  # TODO: Somehow make it crossplatform
  add_custom_target(
    ${target_name}
    COMMAND
      cd ${CMAKE_SOURCE_DIR} && grep -e "${comment_type}:" -Hnr .
      ${COMMENT_EXCLUDE} | awk -f
      ${CMAKE_SOURCE_DIR}/scripts/awk/print_comments.awk
  )
endfunction()

function(generate_header_from_lua input_file)
  # Get relative path to LUA_SOURCE_DIR
  cmake_path(
    RELATIVE_PATH input_file
    BASE_DIRECTORY ${LUA_SOURCE_DIR}
    OUTPUT_VARIABLE relative_input_path
  )

  # Set header_file
  cmake_path(
    ABSOLUTE_PATH relative_input_path
    BASE_DIRECTORY ${LUAH_SOURCE_DIR}
    OUTPUT_VARIABLE header_file
  )
  string(REGEX REPLACE "\\.[^.]*$" "" header_file ${header_file})
  string(REPLACE "." "_" header_file ${header_file})
  set(header_file "${header_file}.h")
  string(REGEX REPLACE "\\.[^.]*$" ".c" c_file ${header_file})

  # set INCLUDE_FILE
  cmake_path(GET header_file FILENAME INCLUDE_FILE)

  # Set HEADER_GUARD
  cmake_path(
    RELATIVE_PATH header_file
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
    OUTPUT ${header_file} ${c_file}
    COMMAND
      ${CMAKE_COMMAND} -DINPUT_FILE=${input_file} -DHEADER_FILE=${header_file}
      -DC_FILE=${c_file} -DHEADER_GUARD=${HEADER_GUARD}
      -DINCLUDE_FILE=${INCLUDE_FILE} -DVARIABLE_NAME=${VARIABLE_NAME} -P
      ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/write_lua_header.cmake
    DEPENDS ${input_file}
    COMMENT "Generating ${header_file} from ${input_file}"
    VERBATIM
  )

  # Add file to GENERATED so "clean" works
  set_source_files_properties(
    ${header_file}
    ${c_file}
    PROPERTIES GENERATED TRUE
  )

  set(_GENERATED_HEADERS ${GENERATED_HEADERS})
  set(_GENERATED_C_FILES ${GENERATED_C_FILES})

  list(APPEND _GENERATED_HEADERS ${header_file})
  list(APPEND _GENERATED_C_FILES ${c_file})

  set(GENERATED_HEADERS ${_GENERATED_HEADERS} PARENT_SCOPE)
  set(GENERATED_C_FILES ${_GENERATED_C_FILES} PARENT_SCOPE)

  message(STATUS "Registered dependency: ${input_file} -> ${header_file}")
  message(STATUS "Registered dependency: ${input_file} -> ${c_file}")
endfunction()
