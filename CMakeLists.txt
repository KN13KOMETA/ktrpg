cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

cmake_path(SET SOURCE_FILES_DIR "${CMAKE_SOURCE_DIR}/src")

include("cmake/functions.cmake")

generate_header_from_lua(${SOURCE_FILES_DIR}/lua/init.lua)

set(GENERATED_HEADERS ${LUAH_SOURCE_DIR}/init.h)

add_custom_target(generated_files ALL DEPENDS ${GENERATED_HEADERS})

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
  src/tgg/components.c
  src/tgg/systems.c
  src/tgg/tgg.c
  src/lt.c
  src/functions.c
  src/game/ecs/components.c
  src/game/ecs/systems.c
)

# Generated files will be regenarated
add_dependencies(${PROJECT_NAME} generated_files)

# Detect make
if(CMAKE_HOST_WIN32)
  set(MAKE mingw32-make)
else()
  set(MAKE make)
endif()

include("cmake/libraries.cmake")

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
message(STATUS "Compile options for ${PROJECT_NAME}: ${TARGET_COMPILE_OPTS}")

include("cmake/target/todo.cmake")
