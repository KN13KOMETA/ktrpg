[README RU](README_RU.md) \* [SPOILER ALERT](SPOILER_ALERT.md) \* [BUILDING](#building) \* [DEVELOPING](#developing) \* [RESOURCES](#resources)

# PREVIEW

<!-- TODO: put some screenshots here and some details about game-->

# BUILDING

## Requirements

- CMake (3.30+)
- Build system (Ninja, Make or other)
- C compiler (Zig is required for cross-compiling)

## Preparations

1. Clone a repository:
   ```bash
   git clone https://github.com/KN13KOMETA/c_text_rpg
   ```
2. Install submodules:
   ```bash
   git submodule update --init --recursive
   ```
   _This will install every required library as git submodules_

## Configuring and building

### For current platform

1. Configure:
   ```bash
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```
2. Build:
   ```bash
   cmake --build build --parallel
   ```

### For cross-compiling (e.g., to Windows)

1. Configure:
   ```bash
   cmake -S . -B build-win --toolchain x86_64-windows-gnu.cmake -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```
2. Build:
   ```bash
   cmake --build build-win --parallel
   ```

### Command explanation

- `-S .` is required, specifies source directory
- `-B <dir>` is required, specifies build directory
- `-G Ninja` specifies the build system  
  _If not specified, CMake will decide which one to use_
- `-DCMAKE_BUILD_TYPE=Release` specifies build type  
  _If not specified, `Debug` will be used as build type_
- `--toolchain <file>` specifies the toolchain and target platform  
  _If not specified, the target will be the current platform_

## Additional commands

- List all build targets:
  ```bash
  cmake --build build --target help
  ```
- Clean built files:
  ```bash
  cmake --build build --target clean
  ```
- Print TODO comments (requires awk and grep):
  ```bash
  cmake --build build --target todo
  ```
- Print NOTE comments (requires awk and grep):
  ```bash
  cmake --build build --target note
  ```
- Print WARN comments (requires awk and grep):
  ```bash
  cmake --build build --target warn
  ```

# DEVELOPING

## Language servers

- C: [clangd](https://clangd.llvm.org)
- Lua: [lua-language-server](https://github.com/LuaLS/lua-language-server)
- CMake: [cmake-language-server](https://github.com/regen100/cmake-language-server)

## Formatters

- C: [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) (built in `clang`)
- Lua: [StyLua](https://github.com/JohnnyMorganz/StyLua)
- CMake: [gersemi](https://github.com/BlankSpruce/gersemi)

## Additional notes

- Build project at least once (directory name should be `build`) for correct work of `clangd`

# RESOURCES

## Libraries

- [argparse](https://github.com/cofyc/argparse) - command-line argument parsing
- [Lua](https://github.com/marovira/lua) - a fork of the official Lua with a CMake build system
- [pico_headers](https://github.com/empyreanx/pico_headers) - a collection of single-header libraries  
  _It's used for its sparse-set based ECS implementation_

## Toolchain

- CMake Zig toolchain based on [mrexodia/zig-cross](https://github.com/mrexodia/zig-cross)
