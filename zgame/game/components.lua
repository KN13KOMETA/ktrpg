---@class GameComponents
local M = {}

---@class GameComponentsInstance
---@field name StrComponentInstance any
---@field player TagComponentInstance any
---@field movable TagComponentInstance any
---@field location TagComponentInstance id
---@field location_next TagComponentInstance next id
---@field door TagComponentInstance char

---@alias GameComponentsCreate fun(Component: Component): GameComponentsInstance
---@type GameComponentsCreate
function M.create(Component)
  return {
    name = Component:new("str", "Name"),

    player = Component:new("tag", "Player"),

    movable = Component:new("tag", "Movable"),

    location = Component:new("tag", "Location ID"),

    location_next = Component:new("tag", "Location Next ID"),
    door = Component:new("tag", "Door"),
  }
end

return M
