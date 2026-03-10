# Should be called from generate_header_from_lua (functions.cmake)
file(READ ${INPUT_FILE} FILE_CONTENT)

# Escape \, " and \n
string(REPLACE "\\" "\\\\" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\"" "\\\"" FILE_CONTENT "${FILE_CONTENT}")
string(REPLACE "\n" "\\n\"\n\"" FILE_CONTENT "${FILE_CONTENT}")

file(
  WRITE ${HEADER_FILE}
  "\
#pragma once
#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

extern const char* ${VARIABLE_NAME};

#endif // !${HEADER_GUARD}
"
)

file(
  WRITE ${C_FILE}
  "\
#include \"${HEADER_FILE}\"

const char* ${VARIABLE_NAME} = \"${FILE_CONTENT}\";
"
)
