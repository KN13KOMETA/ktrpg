# Add argparse
# TODO: CMakeLists.txt in argparse is deprecated, do something with it
add_subdirectory(libs/argparse)

# Add Lua
add_subdirectory(libs/lua)

# Header-only library pico_headers
add_library(pico_headers INTERFACE)
target_include_directories(pico_headers INTERFACE libs/pico_headers)

# Link libraries
target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE argparse_static lua::lua pico_headers m
)
