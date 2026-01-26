local ktrpg = require("ktrpg")

COMPONENT = {
  name = ktrpg.Component:new("str", "name"),
  someint = ktrpg.Component:new("int", "someint"),
  health = ktrpg.Component:new("num", "health"),
  dead = ktrpg.Component:new("tag", "dead"),
}

SYSTEM = {
  print_name = ktrpg.System
    :new("print name")
    :requires(COMPONENT.name)
    :excludes(COMPONENT.dead, COMPONENT.health)
    :set_mask(0)
    :on_run(function(entity_count, entities) end),
}
SYSTEM.print_name:run()
SYSTEM.print_name:disable()
SYSTEM.print_name:enable()
SYSTEM.print_name:get_entity_count()
SYSTEM.print_name:get_mask()

local player = ktrpg.Entity:new()

player:set(COMPONENT.name, "POPPO")

print(player:get(COMPONENT.name))
player:get_id()

ktrpg.Entity:by_id(1)

ktrpg.System:run_debug_system()
