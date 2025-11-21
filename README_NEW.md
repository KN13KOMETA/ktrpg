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

## Creating build directory

```bash
cmake -S . -B build-win -G Ninja -DCMAKE_BUILD_TYPE=RELEASE --toolchain x86_64-windows-gnu.cmake
```

Since it's quite a big command here's explanation of what it's doing

- `-S .` is required. It specifies source directory
- `-B build-win` is required. It specifies build directory  
  _<span color="FF6">Note:</span> for correct work of `clangd` it's required to be `build`_
- `-G Ninja` is optional. It specifies build system  
  _If not specified, CMake will decide which one to use_
- `-DCMAKE_BUILD_TYPE=RELEASE` is optional. It specifies build type  
  _If not specified, `DEBUG` will be used as build type_
- `--toolchain x86_64-windows-gnu.cmake` is optional. It specifies toolchain and target platform  
  _If not specified, target will be current platform_  
  _<span color="FF6">Note:</span> there's more toolchains in root directory (they ends with `.cmake`)_  
  _<span color="FF6">Note:</span> if you specify `--toolchain`, zig will be used as compiler_

## Build

- Build project:
  ```bash
  cmake --build build-win --parallel
  ```
- Clean build files:
  ```bash
  cmake --build build-win --parallel --target clean
  ```
- Print todo comments:
  ```bash
  cmake --build build-win --parallel --target todo
  ```

# DEVELOPING

# RESOURCES
