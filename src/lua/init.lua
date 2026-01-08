local component = {
  name = game.register_string_component(),
  health = game.register_integer_component(),
  num = game.register_number_component(),
}

local system = {
  print_name = game.register_system(function(entities, entity_count)
    print(string.format("PRINT NAME SYSTEM (%d)", entity_count))

    for i = 1, entity_count do
      local entity_id = entities[i]
      print(string.format("entity id: %d", entity_id))
    end
  end),
  death = game.register_system(function(entities, entity_count)
    print(string.format("DEATH SYSTEM (%d)", entity_count))

    for i = 1, entity_count do
      local entity_id = entities[i]
      print(string.format("s entity id: %d", entity_id))
    end
  end),
}

local entity1 = game.ecs_entity_create()
local entity2 = game.ecs_entity_create()

print(string.format("Created entity with id %d", entity1))
print(string.format("Created entity with id %d", entity2))

local h1 = game.ecs_comp_add(entity1, component.health)
print(string.format("Added component with address %p", h1))

game.ecs_comp_set(component.health, h1, 199)

local n1 = game.ecs_comp_add(entity2, component.name)
game.ecs_comp_set(component.name, n1, "hello world")
