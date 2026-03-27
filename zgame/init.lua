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
  creature_move = System:new("Move: Creature move")
    :requires(gcomp.creature, gcomp.location, gcomp.destination)
    :on_run(function(entities, entities_count)
      for i = 1, entities_count, 1 do
        local e = entities[i]
        ---@type GCacheCreature
        local creature = gcache.creatures[e:get(gcomp.creature)]
        ---@type GCacheLocation
        local creature_location = gcache.locations[creature.location]
        ---@type GCacheDoorTable
        local location_doors = gcache.group.doors_by_location[creature.location]

        if creature.id == gcomp.creature_player then
          Util.writenl("Player at location :" .. creature_location.name)

          local sorted_keys = {}

          for key in pairs(location_doors) do
            table.insert(sorted_keys, key)
          end

          table.sort(sorted_keys)

          for _, key in ipairs(sorted_keys) do
            local door = location_doors[key]
            Util.writenl(":> Door '" .. string.char(door.name) .. "': " .. gcache.locations[door.destination].name)
          end

          Util.write("Select door (enter to stay): ")

          local select = string.byte(Util.readchar())
          local select_door = location_doors[select]

          if select_door ~= nil then
            e:set(gcomp.location, select_door.destination)
          end
        else
          if Util.random() % 3 == 0 then
            local door_keys = {}

            for key in pairs(location_doors) do
              table.insert(door_keys, key)
            end

            local r = (Util.random() % #door_keys) + 1
            local rdoor = location_doors[door_keys[r]]

            Util.writenl(
              "Creature "
                .. creature.id
                .. " moved from "
                .. creature_location.name
                .. " to "
                .. gcache.locations[rdoor.destination].name
            )

            e:set(gcomp.location, rdoor.destination)
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
  syst.creature_move:run()
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
