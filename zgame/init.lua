-- TODO: template engine for strings

local ktrpg = krequire("ktrpg")

local init_world = krequire("game/world")

---@type GameComponents
local GComponents = krequire("game/components")

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

local syst = {
  -- TODO: Fixers/validators system, like is there a player and etc
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
      end
    end),
  debug_cache = System:new("Debug Cache"):on_run(function(entities, entities_count)
    Util.writenl(syst_cache)
  end),
  debug = System:new("__KTRPG_DEBUG"),
}

local function run_move()
  syst.move_get_location_names:run()
  syst.move_get_current_location:run()
  syst.move_find_doors:run()
  syst.move_player_move:run()
end

local function run()
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

  e:set(comp.location, l_tag)
  e:set(comp.name, l_name)
end, function(d_tag, d_location, d_destination)
  local e = Entity:new()

  if e == nil then
    return Util.exit()
  end

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
end

while true do
  run()

  -- Util.sleep(1)
end
