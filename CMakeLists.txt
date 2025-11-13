cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

# Always export compile commands (needs for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  c_text_rpg
  VERSION 3.0.0
  DESCRIPTION "Text RPG written in C"
  HOMEPAGE_URL https://github.com/KN13KOMETA/c_text_rpg
  LANGUAGES C
)

include("cmake/version.cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "DEBUG")
endif()

# Force CMake to use c99 instead of gnu99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(
  ${PROJECT_NAME}
  src/main.c
  src/functions.c
  src/game/ecs/components.c
  src/game/ecs/systems.c
)

# Detect make
if(CMAKE_HOST_WIN32)
  set(MAKE mingw32-make)
else()
  set(MAKE make)
endif()

# Add libs/luajit
set(LUAJIT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/luajit)
set(LUAJIT_LIB ${LUAJIT_DIR}/src/libluajit.a)

# Check if luajit already built
if(NOT EXISTS ${LUAJIT_LIB})
  add_custom_command(
    OUTPUT ${LUAJIT_LIB}
    COMMAND ${MAKE} -C ${LUAJIT_DIR}
    # COMMAND ${MAKE} all
    WORKING_DIRECTORY ${LUAJIT_DIR}
    COMMENT "Building LuaJIT"
  )
endif()

add_custom_target(luajit_build ALL DEPENDS ${LUAJIT_LIB})

# Импортируем библиотеку
add_library(luajit STATIC IMPORTED)
set_target_properties(
  luajit
  PROPERTIES
    IMPORTED_LOCATION ${LUAJIT_LIB}
    INTERFACE_INCLUDE_DIRECTORIES ${LUAJIT_DIR}/src
)

add_dependencies(luajit luajit_build)

# Add libs/pico_headers
add_library(pico_headers INTERFACE)
target_include_directories(pico_headers INTERFACE libs/pico_headers)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE pico_headers luajit)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
    OUTPUT_NAME
      "${PROJECT_NAME}_${PROJECT_FULL_VERSION}$<$<CONFIG:DEBUG>:_debug>"
)

# Turning on every compiler checks
target_compile_options(
  ${PROJECT_NAME}
  PUBLIC -xc -Wall -Wextra -Wpedantic $<$<CONFIG:RELEASE>:-Werror>
)

target_compile_definitions(
  ${PROJECT_NAME}
  PUBLIC
    PROJECT_NAME=\"${PROJECT_NAME}\"
    PROJECT_FULL_VERSION=\"${PROJECT_FULL_VERSION}\"
    PROJECT_VERSION=\"${PROJECT_FULL_VERSION}\"
    $<$<CONFIG:DEBUG>:DEBUG>
)

get_property(
  TARGET_COMPILE_OPTS
  TARGET ${PROJECT_NAME}
  PROPERTY COMPILE_OPTIONS
)
message(STATUS "Compile options for my_target: ${TARGET_COMPILE_OPTS}")

include("cmake/target/todo.cmake")
