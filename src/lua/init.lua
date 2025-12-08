local component = {
  health = lt.register_number_component(),
  damage = lt.register_number_component(),
  location = lt.register_integer_component(),
}

print(
  string.format(
    "COMPONENTS\n" .. "health: %d\n" .. "damage: %d\n" .. "location: %d",
    component.health,
    component.damage,
    component.location
  )
)

local system = {
  damage_system = lt.register_system(function()
    print("pretend like a system is working LOL")
  end),
}

-- print(string.format("SYSTEMS\n" .. "damage_system: %d", system.damage_system))
