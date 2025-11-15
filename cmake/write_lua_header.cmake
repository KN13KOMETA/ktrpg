# Should be called from generate_header_from_lua (functions.cmake)
file(READ ${INPUT_FILE} FILE_CONTENT)

# Escape \, " and \n
string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

file(
  WRITE ${OUTPUT_FILE}
  "\
#pragma once
#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

static const char* ${VARIABLE_NAME} = \"${FILE_CONTENT}\";

#endif // !${HEADER_GUARD}
"
)
