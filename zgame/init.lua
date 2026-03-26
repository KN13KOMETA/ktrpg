-- TODO: template engine for strings

local ktrpg = krequire("ktrpg")

local init_world = krequire("game/world")

---@type GameComponents
local GComponents = krequire("game/components")

---@type GComponent
local GComponent = krequire("class/GComponent")
local gcomp = GComponent:new()

local Entity = ktrpg.Entity
local Component = ktrpg.Component
local System = ktrpg.System
local Util = ktrpg.Util

-- TODO: empty numbers to nvoid

local comp = GComponents.create(Component)

local syst_cache = {
  move = {
    location = {},
    clocation = -1,
    doors = {},
  },
}

local sc = {
  locations = {},
  creatures = {},
  items = {},
  doors = {},
  group = {
    creatures_by_location = {},
    items_by_location = {},
    doors_by_location = {},
    items_by_creature = {},
  },
}

local syst = {
  -- TODO: Fixers/validators system, like is there a player and etc
  cache_locations = System:new("CACHE: Locations")
    :requires(gcomp.location, gcomp.name)
    :excludes(gcomp.creature, gcomp.item, gcomp.door)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]

        t[e:get(gcomp.location)] = {
          id = e:get(gcomp.location),
          name = e:get(gcomp.name),
        }
      end

      sc.locations = t
    end),
  cache_creatures = System:new("CACHE: Creatures")
    :requires(gcomp.creature)
    :excludes(gcomp.item)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]
        local id = assert(e:get(gcomp.creature), "unreachable")

        if t[id] == nil then
          t[id] = {}
        end

        t[id].id = id
        t[id].location = e:get(gcomp.location)
        t[id].next_location = e:get(gcomp.destination)
      end

      sc.creatures = t
    end),
  -- TODO: ITEMS
  cache_doors = System:new("CACHE: Doors")
    :requires(gcomp.door, gcomp.location, gcomp.destination)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]

        t[e:get(gcomp.door)] = {
          id = e:get(gcomp.door),
          location = e:get(gcomp.location),
          destination = e:get(gcomp.destination),
        }
      end

      sc.doors = t
    end),
  cache_group_data = System:new("CACHE: Group data"):on_run(function()
    do
      local t = {}

      for id, value in pairs(sc.creatures) do
        local lid = value.location

        if t[lid] == nil then
          t[lid] = {}
        end

        t[lid][id] = value
      end

      sc.group.creatures_by_location = t
    end
    do
      local t = {}
      -- TODO: ITEMS
      sc.group.items_by_location = t
    end
    do
      local t = {}

      for id, value in pairs(sc.doors) do
        local lid = value.location

        if t[lid] == nil then
          t[lid] = {}
        end

        t[lid][id] = value
      end

      sc.group.doors_by_location = t
    end
    do
      local t = {}
      -- TODO: ITEMS
      sc.group.items_by_creature = t
    end
  end),
  move_get_location_names = System:new("MOVE: Get Location Names")
    :requires(comp.location, comp.name)
    :on_run(function(entities, entities_count)
      for i = 1, entities_count, 1 do
        local e = entities[i]
        syst_cache.move.location[e:get(comp.location)] = e:get(comp.name)
      end
    end),
  move_get_current_location = System:new("MOVE: Get Current Location")
    :requires(comp.player, comp.location)
    :on_run(function(entities, entities_count)
      if entities_count == 0 then
        syst_cache.move.clocation = -1
        return
      end

      syst_cache.move.clocation = entities[1]:get(comp.location)
    end),
  move_find_doors = System:new("MOVE: Find Doors")
    :requires(comp.door, comp.location, comp.location_next)
    :on_run(function(entities, entities_count)
      local doors = {}

      if syst_cache.move.clocation ~= -1 then
        for i = 1, entities_count, 1 do
          local e = entities[i]
          local cl = e:get(comp.location)
          if cl == syst_cache.move.clocation then
            doors[e:get(comp.door)] = e:get(comp.location_next)
          end
        end
      end

      syst_cache.move.doors = doors
    end),
  move_player_move = System:new("MOVE: Player Move")
    :requires(comp.player, comp.location, comp.movable)
    :on_run(function(entities, entities_count)
      if entities_count == 0 then
        return
      end

      Util.writenl("Current location \"" .. syst_cache.move.location[syst_cache.move.clocation] .. "\"")

      for door, next in pairs(syst_cache.move.doors) do
        Util.writenl(":> Door '" .. string.char(door) .. "': " .. syst_cache.move.location[next])
      end

      Util.write("Select door: ")

      local sel = string.byte(Util.readchar())
      local sel_door = syst_cache.move.doors[sel]

      if sel_door ~= nil then
        Util.writenl("select " .. sel_door)
        entities[1]:set(comp.location, sel_door)
        entities[1]:set(gcomp.location, sel_door)
      end
    end),
  debug_cache = System:new("Debug Cache"):on_run(function(entities, entities_count)
    Util.writenl(syst_cache)
    Util.writenl(sc)
    Util.writenl(sc.group)
  end),
  debug = System:new("__KTRPG_DEBUG"),
}

local function run_cache()
  syst.cache_locations:run()
  syst.cache_creatures:run()
  syst.cache_doors:run()
  syst.cache_group_data:run()
end

local function run_move()
  syst.move_get_location_names:run()
  syst.move_get_current_location:run()
  syst.move_find_doors:run()
  syst.move_player_move:run()
end

local function run()
  run_cache()
  run_move()

  syst.debug_cache:run()

  syst.debug:run()
end

-- for i = 1, #location_doors, 1 do
--   local door = location_doors[i]
--   local e = Entity:new()
--
--   if e == nil then
--     return Util.exit()
--   end
--
--   e:set(comp.door, string.byte(door[1]))
--   e:set(comp.location, door[2])
--   e:set(comp.location_next, door[3])
-- end

init_world(function(l_tag, l_name)
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(gcomp.location, l_tag)
  e:set(gcomp.name, l_name)

  e:set(comp.location, l_tag)
  e:set(comp.name, l_name)
end, function(d_tag, d_location, d_destination)
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(gcomp.door, d_tag)
  e:set(gcomp.location, d_location)
  e:set(gcomp.destination, d_destination)

  e:set(comp.door, d_tag)
  e:set(comp.location, d_location)
  e:set(comp.location_next, d_destination)

end)

do
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

  e:set(comp.player, 0)
  e:set(comp.location, 0)
  e:set(comp.movable, 0)

  e:set(gcomp.creature, gcomp.creature_player)
  e:set(gcomp.location, 0)
end

while true do
  run()

  -- Util.sleep(1)
end
