local ktrpg = require("ktrpg")

return function()
  local player = ktrpg.Entity:new()

  io.write(":: Enter your nick: ")
  player:set(COMPONENT.name, io.read())
  player:set(COMPONENT.location, LOCATION.player_home)

  player:set(COMPONENT.tag_player, 1)

  player:set(COMPONENT.max_health, 100)
  player:set(COMPONENT.health, 100)
  player:set(COMPONENT.attack, 10)
  player:set(COMPONENT.damage_received, 13)

  player:set(COMPONENT.species, SPECIES.human)

  return player
end
