-- TODO: template engine for strings

local ktrpg = krequire("ktrpg")
---@type GComponent
local GComponent = krequire("class/GComponent")
---@type GCache
local GCache = krequire("class/GCache")

local init_world = krequire("game/world")

local Entity = ktrpg.Entity
local Component = ktrpg.Component
local System = ktrpg.System
local Util = ktrpg.Util

local gcomp = GComponent:new()
local gcache = GCache:new(gcomp)

-- TODO: empty numbers to nvoid

local syst = {
  -- TODO: Fixers/validators system, like is there a player and etc
  move_creatures_move = System:new("MOVE: Creatures Move")
    :requires(gcomp.creature, gcomp.location)
    :on_run(function(entities, entities_count)
      for i = 1, entities_count, 1 do
        local e = entities[i]
        ---@type GCacheCreature
        local creature = gcache.creatures[e:get(gcomp.creature)]

        Util.writenl(creature)

        -- TODO: implement other creatures moving
        if creature.id == gcomp.creature_player then
          Util.writenl("Current location \"" .. gcache.locations[creature.location].name .. "\"")

          local sorted_keys = {}

          for key in pairs(gcache.group.doors_by_location[creature.location]) do
            table.insert(sorted_keys, key)
          end

          table.sort(sorted_keys)

          for _, key in ipairs(sorted_keys) do
            local door = gcache.group.doors_by_location[creature.location][key]
            Util.writenl(":> Door '" .. string.char(door.name) .. "': " .. gcache.locations[door.destination].name)
          end

          Util.write("Select door: ")

          local sel = string.byte(Util.readchar())
          local sel_door = gcache.group.doors_by_location[creature.location][sel]

          if sel_door ~= nil then
            e:set(gcomp.location, sel_door.destination)
          end

        end
      end
    end),
  debug_cache = System:new("Debug Cache"):on_run(function(entities, entities_count)
    Util.writenl(gcache)
    Util.writenl(gcache.group)
  end),
  debug = System:new("__KTRPG_DEBUG"),
}

local function run_move()
  syst.move_creatures_move:run()
end

local function run()
  gcache.system.run_all()
  run_move()

  syst.debug_cache:run()

  syst.debug:run()
end

init_world(function(l_tag, l_name)
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(gcomp.location, l_tag)
  e:set(gcomp.name, l_name)
end, function(d_tag, d_location, d_destination)
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(gcomp.door, d_tag)
  e:set(gcomp.location, d_location)
  e:set(gcomp.destination, d_destination)
end)

do
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(gcomp.creature, gcomp.creature_player)
  e:set(gcomp.location, 0)
end

while true do
  run()

  -- Util.sleep(1)
end
