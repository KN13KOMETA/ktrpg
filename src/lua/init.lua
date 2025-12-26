game.register_system(function(entities, entity_count)
  print("YO THIS IS SYSTEM FROM LUA")
  print(entity_count)
  for i = 1, entity_count do
    local entity_id = entities[i]
    print(string.format("entity id: %d", entity_id))
  end
end)
game.register_string_component()

-- local component = {
--   health = lt.register_number_component(),
--   damage = lt.register_number_component(),
--   location = lt.register_integer_component(),
-- }
--
-- print(
--   string.format(
--     "COMPONENTS\n" .. "health: %d\n" .. "damage: %d\n" .. "location: %d",
--     component.health,
--     component.damage,
--     component.location
--   )
-- )
--
-- local system = {
--   damage_system = lt.register_system(function()
--     print("pretend like a system is working LOL")
--   end),
-- }

-- print(string.format("SYSTEMS\n" .. "damage_system: %d", system.damage_system))
