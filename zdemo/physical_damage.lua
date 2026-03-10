local ktrpg = krequire("ktrpg")

return function(entity, enemy)
  local attack = ktrpg.Entity:new()
  attack:set(COMPONENT.attacker, entity:get_id())
  attack:set(COMPONENT.attack_target, enemy:get_id())
  attack:set(COMPONENT.attack_damage, entity:get(COMPONENT.attack))
end
