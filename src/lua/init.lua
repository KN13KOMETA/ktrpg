local component = {
  name = game.register_string_component(),
  health = game.register_integer_component(),
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
