# Detect make
if(CMAKE_HOST_WIN32)
  set(MAKE mingw32-make)
else()
  set(MAKE make)
endif()

# Library LuaJIT
set(LUAJIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/luajit)
set(LUAJIT_LIB ${LUAJIT_DIR}/src/libluajit.a)

if(NOT EXISTS ${LUAJIT_LIB})
  add_custom_command(
    OUTPUT ${LUAJIT_LIB}
    COMMAND ${MAKE} -C ${LUAJIT_DIR}
    WORKING_DIRECTORY ${LUAJIT_DIR}
    COMMENT "Building LuaJIT"
  )
endif()

add_custom_target(luajit_build ALL DEPENDS ${LUAJIT_LIB})

add_library(luajit STATIC IMPORTED)
set_target_properties(
  luajit
  PROPERTIES
    IMPORTED_LOCATION ${LUAJIT_LIB}
    INTERFACE_INCLUDE_DIRECTORIES ${LUAJIT_DIR}/src
)

# Add luajit_build as dependency, so it builds
add_dependencies(luajit luajit_build)

# Header-only library pico_headers
add_library(pico_headers INTERFACE)
target_include_directories(pico_headers INTERFACE libs/pico_headers)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE pico_headers luajit m dl)
