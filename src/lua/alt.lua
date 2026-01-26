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

SYSTEM = {
  print_name = ktrpg
    .System
    :new("Print Name")
    :requires(COMPONENT.name)
    -- :excludes(COMPONENT.attack_target)
    :set_mask(math.maxinteger)
    :on_run(function(entities, entity_count)
      print("Yo this is ma print name function from lua")
      io.write("we have that much en titties: ")
      print(entity_count)

      for i = 1, entity_count, 1 do
        print("entity " .. i .. " name is " .. entities[i]:get(COMPONENT.name))
      end
    end),
}

local player = ktrpg.Entity:new()
print(player:set(COMPONENT.name, "POPPO"))
print(player:set(COMPONENT.is_player, 20))
print(player:set(COMPONENT.max_health, 1.3))
print(player:set(COMPONENT.attack_target, 69))

print(SYSTEM.print_name:get_name())

print(player:get(COMPONENT.name, "POPPO"))
print(player:get(COMPONENT.is_player, 20))
print(player:get(COMPONENT.max_health, 1.3))
print(player:get(COMPONENT.attack_target, 69))
print(player:get(COMPONENT.attack, 69))

SYSTEM.print_name:run():run():run():run()

print(SYSTEM.print_name:get_mask())

print(SYSTEM.print_name:get_entity_count())
