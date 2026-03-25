local ktrpg = krequire("ktrpg")

---@class GComponent
---@field name          StrComponentInstance
---@field location      TagComponentInstance
---@field creature      IntComponentInstance
---@field item          IntComponentInstance
---@field door          TagComponentInstance
---@field next_location TagComponentInstance
local GComponent = {}
GComponent.__index = GComponent

-- CONSTANTS
GComponent.creature_player = 0

---@return GComponent
function GComponent:new()
  local instance = {
    name = ktrpg.Component:new("str", "Name Z"),
    location = ktrpg.Component:new("tag", "Location Z"),
    creature = ktrpg.Component:new("int", "Creature Z"),
    item = ktrpg.Component:new("int", "Item Z"),
    door = ktrpg.Component:new("tag", "Door Z"),
    next_location = ktrpg.Component:new("tag", "Next Location Z"),
  }

  for key, value in pairs(instance) do
    if value == nil then
      ktrpg.Util.writenl("FATAL: Failed to create component " .. key)
      ---@diagnostic disable-next-line: return-type-mismatch
      return ktrpg.Util.exit()
    end
  end

  return setmetatable(instance, GComponent)
end

return GComponent
