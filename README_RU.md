[README EN](README.md) \* [ПРЕДУПРЕЖДЕНИЕ О СПОЙЛЕРАХ](SPOILER_ALERT.md) \* [СБОРКА](#сборка) \* [РАЗРАБОТКА](#разработка) \* [РЕСУРСЫ](#РЕСУРСЫ)

> Русский перевод README. Актуальность перевода не гарантируется.

# ПРЕВЬЮ

<!-- TODO: put some screenshots here and some details about game-->

# СБОРКА

## Требования

- CMake (3.30+)
- Система сборки (Ninja, Make или другая)
- Компилятор C (Zig необходим для кросс-компиляции)

## Приготовления

1. Склонируйте репозиторий:
   ```bash
   git clone https://github.com/KN13KOMETA/c_text_rpg
   ```
2. Установите подмодули:
   ```bash
   git submodule update --init --recursive
   ```
   _Это установит все необходимые библиотеки как git-подмодули_

## Настройка и сборка

### Для текущей платформы

1. Настройка:
   ```bash
   cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```
2. Сборка:
   ```bash
   cmake --build build --parallel
   ```

### Для кросс-компиляции (пр., на Windows)

1. Настройка:
   ```bash
   cmake -S . -B build-win --toolchain x86_64-windows-gnu.cmake -G Ninja -DCMAKE_BUILD_TYPE=Release
   ```
2. Сборка:
   ```bash
   cmake --build build-win --parallel
   ```

### Объяснения команды

- `-S .` необходим, указывает директорию исходников
- `-B <dir>` необходим, указывает директорию сборки
- `-G Ninja` указывает систему сборки  
  _Если не указано, CMake сам решит какую использовать_
- `-DCMAKE_BUILD_TYPE=Release` указывает тип сборки  
  _Если не указано, `Debug` будет использован как тип сборки_
- `--toolchain <file>` указывает тулчейн и целевую платформу  
  _Если не указано, целью будет текущая платформа_

## Дополнительные команды

- Список всех целей сборки:
  ```bash
  cmake --build build --target help
  ```
- Очистить файлы сборки:
  ```bash
  cmake --build build --target clean
  ```
- Напечатать TODO комментарии (требует awk и grep):
  ```bash
  cmake --build build --target todo
  ```
- Напечатать NOTE комментарии (требует awk и grep):
  ```bash
  cmake --build build --target note
  ```
- Напечатать WARN комментарии (требует awk и grep):
  ```bash
  cmake --build build --target warn
  ```

# РАЗРАБОТКА

## Языковые сервера

- C: [clangd](https://clangd.llvm.org)
- Lua: [lua-language-server](https://github.com/LuaLS/lua-language-server)
- CMake: [cmake-language-server](https://github.com/regen100/cmake-language-server)

## Форматтеры

- C: [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) (built in `clang`)
- Lua: [StyLua](https://github.com/JohnnyMorganz/StyLua)
- CMake: [gersemi](https://github.com/BlankSpruce/gersemi)

## Дополнительные заметки

- Соберите проект хотя бы один раз (директория сборки должна быть `build`) для корректной работы `clangd`

# РЕСУРСЫ

- CMake Zig тулчейн украден из https://github.com/mrexodia/zig-cross
