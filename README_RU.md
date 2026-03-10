[README EN](README.md) \* [ПРЕДУПРЕЖДЕНИЕ О СПОЙЛЕРАХ](SPOILER_ALERT.md) \* [СБОРКА](#сборка) \* [СКРИПТИНГ](#скриптинг) \* [РАЗРАБОТКА](#разработка) \* [РЕСУРСЫ](#ресурсы)

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
   git clone https://github.com/KN13KOMETA/ktrpg
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

# СКРИПТИНГ

Движок использует Lua 5.4 с набором библиотек: `ktrpg` (библиотека движка), `table`, `string`, `math` и ограниченная `base`. Также есть функция `krequire` для загрузки модулей/файлов Lua.

Типы описаны в формате LuaCATS (поддерживается [LuaLS](https://github.com/LuaLS/lua-language-server)) и могут быть экспортироваться командой:

```bash
./ktrpg --export-types=path/to/empty/dir
```

Также можно экспортировать скрипты игры:

```bash
./ktrpg --export-scripts=path/to/empty/dir
```

## Написание первого скрипта

Создайте файл `helloktrpg.lua`:

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

Запустите его:

```bash
./ktrpg --load=helloktrpg.lua
```

## Модуль `ktrpg`

`ktrpg` предоставляет 3 класса для взаимодействия с ECS (`Entity`, `Component`, `System`) и класс `Util` для взаимодействия с пользователем и движком.

### Типы компонентов

- `int` - Lua integer
- `num` - Lua number
- `tag` - Беззнаковое 8-битное целое (обычно используется как тег)
- `str` - Строка

### Отладочная система `__KTRPG_DEBUG`

Системы с этим именем назначаются на функцию C, которая выводит сущности и их компоненты. Обратите внимание, что хотя вы не можете добавить `on_run` к такой системе, все остальное работает как обычно.

## Функция `krequire`

Загружает модуль/файл Lua по указанному имени/пути.

### Поиск пути

Поиск выполняется относительно пути к папке главного скрипта(того, который передан с `--load`)

### Пример

Дерево файлов:

```bash
dir
├── a.lua
├── b
│   └── b.lua
├── c.lua
├── ktrpg
└── init.lua
```

Файл `dir/init.lua`:

```lua
-- Загружает dir/a.lua
local a = krequire("a")
-- Загружает dir/b/b.lua
local b = krequire("b/b")
```

Файл `dir/b/b.lua`

```lua
-- Загружает dir/c.lua
local c = krequire("c")
```

Команда запуска:

```bash
./ktrpg --load=init.lua
```

## Внутреннее устройство

- Скрипты и определения типов находятся в `src/lua/` и `src/lua/types` соответственно.
- Во время сборки CMake они преобразуются в C-заголовки/исходники и встраиваются в исполняемый файл.
- Модуль `ktrpg` находится в `src/luaglue`.
- Модуль `krequire` находится в `src/lua_sandbox.c`.
- Модифицированная `base` находится в `src/lua_sandbox.c`.
- Система модулей может работать с `vfile` (виртуальный файл) - так игра загружает встроенные скрипты.

# РАЗРАБОТКА

## Языковые сервера

- C: [clangd](https://clangd.llvm.org)
- Lua: [lua-language-server](https://github.com/LuaLS/lua-language-server)
- CMake: [cmake-language-server](https://github.com/regen100/cmake-language-server)

## Форматтеры

- C: [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) (встроен в `clang`)
- Lua: [StyLua](https://github.com/JohnnyMorganz/StyLua)
- CMake: [gersemi](https://github.com/BlankSpruce/gersemi)

## Дополнительные заметки

- Соберите проект хотя бы один раз (директория сборки должна быть `build`) для корректной работы `clangd`

# РЕСУРСЫ

## Libraries

- [argparse](https://github.com/cofyc/argparse) - разбор аргументов командной строки
- [Lua](https://github.com/marovira/lua) - форк оффициального Lua с системой сборки CMake
- [pico_headers](https://github.com/empyreanx/pico_headers) - коллекция single-header библиотек  
  _Используется из-за реализации ECS на основе разряженных множеств (sparse set)_

## Тулчейн

- CMake Zig тулчейн основан на [mrexodia/zig-cross](https://github.com/mrexodia/zig-cross)
