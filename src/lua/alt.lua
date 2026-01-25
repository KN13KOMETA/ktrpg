local ktrpg = require("ktrpg")

COMPONENT = {
  name = ktrpg.Component:new("str", "Name"),
  is_player = ktrpg.Component:new("tag", "Is Player"),

  max_health = ktrpg.Component:new("num", "Max Health"),
  health = ktrpg.Component:new("num", "Health"),
  attack = ktrpg.Component:new("num", "Attack"),
  attack_target = ktrpg.Component:new("int", "Attack Target"),
  damage_received = ktrpg.Component:new("num", "Damage Received"),
}

SYSTEMS = {
  print_name = ktrpg.System:new("Print Name"):requires(COMPONENT.name),
}

print(SYSTEMS.print_name:get_name())
print(SYSTEMS.print_name:get_mask())
