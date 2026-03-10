[README RU](README_RU.md) \* [SPOILER ALERT](SPOILER_ALERT.md) \* [BUILDING](#building) \* [SCRIPTING](#scripting) \* [DEVELOPING](#developing) \* [RESOURCES](#resources)

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

# SCRIPTING

The engine uses Lua 5.4 with a set of libraries: `ktrpg` (engine library), `table`, `string`, `math`, and a limited `base`. Additionally there is `krequire` function for module/lua file loading.

Types are written in LuaCATS format (supported by [LuaLS](https://github.com/LuaLS/lua-language-server)) and can be exported with:

```bash
./c_text_rpg --export-types=path/to/empty/dir
```

You can also export game scripts:

```bash
./c_text_rpg --export-scripts=path/to/empty/dir
```

## Writing first script

Create file `helloktrpg.lua`:

```lua
local ktrpg = krequire("ktrpg")

local Entity = ktrpg.Entity
local Component = ktrpg.Component
local System = ktrpg.System
local Util = ktrpg.Util

local components = {
  player = Component:new("tag", "Player"),
  name = Component:new("str", "Name"),
}

local systems = {
  create_player = System
    :new("Create Player")
    :requires(components.player)
    :on_run(function(entities, entities_count)
      if entities_count == 0 then
        local e = Entity:new()

        if e ~= nil then
          Util.write("Enter your name: ")
          local name = Util.read(16)

          e:set(components.player, 0)
          e:set(components.name, name)
        end
      end
    end),
  action = System:new("Action")
    :requires(components.player)
    :on_run(function(entities, entities_count)
      if entities_count == 0 then
        Util.writenl("No player found")
        return
      end

      local p = entities[1]
      local p_name = p:get(components.name)

      if p_name == nil then
        p_name = "NoName"
      end

      Util.write(p_name .. " action: ")

      local char = Util.readchar()

      if char == "q" then
        Util.exit()
      end
    end),
  debug = System:new("__KTRPG_DEBUG"),
}

while true do
  systems.create_player:run()
  systems.action:run()
  systems.debug:run()
  Util.sleep(1)
end
```

Run it with:

```bash
./c_text_rpg --load=helloktrpg.lua
```

## The `ktrpg` module

`ktrpg` provides 3 classes for interaction with ECS (`Entity`, `Component`, `System`) and `Util` class for interaction with user and engine.

### Component types

- `int` - Lua integer
- `num` - Lua number
- `tag` - Unsigned 8 bit integer (commonly used as a tag)
- `str` - String

### The `__KTRPG_DEBUG` debug system

Systems with this name are assigned to c function that prints entities and their components. Note that while you can't add `on_run` to this, everything else works as expected.

## The `krequire` function

Loads module/lua file by specified name/path.

### Path search

Searches for relative path of the directory of the main script (the one passed with `--load`)

### Example

Filetree:

```bash
dir
├── a.lua
├── b
│   └── b.lua
├── c.lua
├── c_text_rpg
└── init.lua
```

File `dir/init.lua`:

```lua
-- Loads dir/a.lua
local a = krequire("a")
-- Loads dir/b/b.lua
local b = krequire("b/b")
```

File `dir/b/b.lua`

```lua
-- Loads dir/c.lua
local c = krequire("c")
```

Run command:

```bash
./c_text_rpg --load=init.lua
```

## Internal structure

- Scripts and type definitions reside in `src/lua/` and `src/lua/types` respectively.
- During the CMake build, they are converted to C header/source files and embedded into the executable.
- The `ktrpg` module is located in `src/luaglue`.
- The `krequire` function is located in `src/lua_sandbox.c`.
- The modified `base` is located in `src/lua_sandbox.c`.
- Module systems can work with `vfile` (virtual file), this is how game loads embedded scripts.

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
