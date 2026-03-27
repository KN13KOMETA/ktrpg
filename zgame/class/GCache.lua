local ktrpg = krequire("ktrpg")

---@class GCacheLocation
---@field id integer
---@field name string

---@class GCacheCreature
---@field id integer
---@field location? integer
---@field destination? integer

---@class GCacheItem

---@class GCacheDoor
---@field name integer
---@field destination integer
---@field location integer

---@alias GCacheLocationTable table<integer, GCacheLocation>
---@alias GCacheCreatureTable table<integer, GCacheCreature>
---@alias GCacheItemTable table<integer, GCacheItem>
---@alias GCacheDoorTable table<integer, GCacheDoor>
---@alias GCacheDoorArray GCacheDoor[]

---@class GCacheGroup
---@field creatures_by_location table<integer, GCacheCreatureTable>
---@field items_by_location table<integer, GCacheItemTable>
---@field doors_by_location table<integer, GCacheDoorTable>
---@field items_by_creature table<integer, GCacheItemTable>

---@class GcacheSystem
---@field cache_locations System
---@field cache_creatures System
---@field cache_doors System
---@field group_data System
---@field run_all function

---@class GCache
---@field locations GCacheLocationTable
---@field creatures GCacheCreatureTable
---@field items     GCacheItemTable
---@field doors     GCacheDoorArray
---@field group     GCacheGroup
---@field system    GcacheSystem
local GCache = {}
GCache.__index = GCache

---@param gcomponent GComponent
---@return GCache
function GCache:new(gcomponent)
  local instance = {
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
    system = {
      cache_locations = ktrpg.System:new("GCache: Cache locations"),
      cache_creatures = ktrpg.System:new("GCache: Cache creatures"),
      cache_doors = ktrpg.System:new("GCache: Cache doors"),
      group_data = ktrpg.System:new("GCache: Group data"),
    },
  }

  for key, value in pairs(instance.system) do
    if value == nil then
      ktrpg.Util.writenl("FATAL: Failed to create system " .. key)
      ---@diagnostic disable-next-line: return-type-mismatch
      return ktrpg.Util.exit()
    end
  end

  instance.system.cache_locations
    :requires(gcomponent.location, gcomponent.name)
    :excludes(gcomponent.creature, gcomponent.item, gcomponent.door)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]

        t[e:get(gcomponent.location)] = {
          id = e:get(gcomponent.location),
          name = e:get(gcomponent.name),
        }
      end

      instance.locations = t
    end)

  instance.system.cache_creatures
    :requires(gcomponent.creature)
    :excludes(gcomponent.item)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]
        local id = assert(e:get(gcomponent.creature), "unreachable")

        if t[id] == nil then
          t[id] = {}
        end

        t[id].id = id
        t[id].location = e:get(gcomponent.location)
        t[id].destination = e:get(gcomponent.destination)
      end

      instance.creatures = t
    end)

  -- TODO: ITEMS

  instance.system.cache_doors
    :requires(gcomponent.door, gcomponent.location, gcomponent.destination)
    :on_run(function(entities, entities_count)
      local t = {}

      for i = 1, entities_count, 1 do
        local e = entities[i]

        table.insert(t, {
          name = e:get(gcomponent.door),
          location = e:get(gcomponent.location),
          destination = e:get(gcomponent.destination),
        })
      end

      instance.doors = t
    end)

  instance.system.group_data:on_run(function()
    do
      local t = {}

      for id, value in pairs(instance.creatures) do
        local lid = value.location

        if lid ~= nil then

          if t[lid] == nil then
            t[lid] = {}
          end

          t[lid][id] = value

        end
      end

      instance.group.creatures_by_location = t
    end
    do
      local t = {}
      -- TODO: ITEMS
      instance.group.items_by_location = t
    end
    do
      local t = {}

      for _, value in pairs(instance.doors) do
        local lid = value.location

        if t[lid] == nil then
          t[lid] = {}
        end

        t[lid][value.name] = value
      end

      instance.group.doors_by_location = t
    end
    do
      local t = {}
      -- TODO: ITEMS
      instance.group.items_by_creature = t
    end
  end)

  instance.system.run_all = function()
    instance.system.cache_locations:run()
    instance.system.cache_creatures:run()
    instance.system.cache_doors:run()
    instance.system.group_data:run()
  end

  return setmetatable(instance, GCache)
end

return GCache
