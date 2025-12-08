[README RU](README_RU.md) \* [BUILDING](#building) \* [DEVELOPING](#developing) \* [RESOURCES](#resources)

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
  _If not soecified, the target will be the current platform_

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

# DEVELOPING

# RESOURCES
