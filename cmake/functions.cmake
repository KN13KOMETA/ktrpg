cmake_path(SET LUA_SOURCE_DIR "${SOURCE_FILES_DIR}/lua")
cmake_path(SET LUAH_SOURCE_DIR "${SOURCE_FILES_DIR}/luah")

function(generate_header_from_lua input_file output_file)
  file(READ ${input_file} FILE_CONTENT)

  cmake_path(
    RELATIVE_PATH
    input_file
    BASE_DIRECTORY ${SOURCE_FILES_DIR}
    OUTPUT_VARIABLE relative_input_path
  )

  cmake_path(HASH relative_input_path HEADER_GUARD)
  string(TOUPPER ${HEADER_GUARD} HEADER_GUARD)
  set(HEADER_GUARD "_${HEADER_GUARD}_H")

  message(NOTICE "ASDPATH: ${relative_input_path}")
  message(NOTICE "ASDPATH HASH: ${HEADER_GUARD}")

  # Escape \, " and \n
  string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
  string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
  string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

  file(
    WRITE
    ${output_file}
    "\
#pragma once
#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

const char* lua_script = \"${FILE_CONTENT}\";

#endif // !${HEADER_GUARD}
"
  )
endfunction()
