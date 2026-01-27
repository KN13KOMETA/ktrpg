--[[ NOT USED IN DEMO
ktrpg.System:new("Name"):get_name()
ktrpg.System:new("Name"):excludes()
ktrpg.System:new("Name"):get_mask()
ktrpg.System:new("Name"):get_entity_count()
--]]

local ktrpg = require("ktrpg")

local create_player = require("create_player")

LOCATION = {
  player_home = 1,
  forest = 2,
  mountain = 3,
}

MOB_AI = {
  neutral = 1,
  hunt = 2,
}

SPECIES = {
  human = 1,
  goblin = 2,
}

COMPONENT = {
  name = ktrpg.Component:new("str", "Name"),
  location = ktrpg.Component:new("tag", "Location"),

  tag_player = ktrpg.Component:new("tag", "Player"),

  max_health = ktrpg.Component:new("num", "Max Health"),
  health = ktrpg.Component:new("num", "Health"),
  attack = ktrpg.Component:new("num", "Attack"),
  damage_received = ktrpg.Component:new("num", "Damage Received"),

  attacker = ktrpg.Component:new("int", "Attacker"),
  attack_target = ktrpg.Component:new("int", "Attack Target"),
  attack_damage = ktrpg.Component:new("num", "Attack Damage"),

  mob_ai = ktrpg.Component:new("tag", "Mob AI"),
  species = ktrpg.Component:new("tag", "Species"),
}

SYSTEM = {
  mob_hunt = ktrpg.System
    :new("Mob Hunt")
    :requires(COMPONENT.location, COMPONENT.attack, COMPONENT.mob_ai, COMPONENT.species)
    :set_mask(0)
    :on_run(function(entities, entity_count)
      local gentities, gentity_count = ktrpg.Entity:all()

      for gi = 1, gentity_count, 1 do
        local enemy = gentities[gi]
        local enemy_species = enemy:get(COMPONENT.species)

        if enemy_species == nil then
          goto continue
        end

        for i = 1, entity_count, 1 do
          local entity = entities[i]
          local mob_ai = entity:get(COMPONENT.mob_ai)

          if mob_ai == MOB_AI.hunt then
            if enemy_species ~= entity:get(COMPONENT.species) then
              local attack = ktrpg.Entity:new()
              attack:set(COMPONENT.attacker, entity:get_id())
              attack:set(COMPONENT.attack_target, enemy:get_id())
              attack:set(COMPONENT.attack_damage, entity:get(COMPONENT.attack))
            end
          end
        end

        ::continue::
      end
    end),
  physical_damage = ktrpg.System
    :new("Physical Damage")
    :requires(COMPONENT.attacker, COMPONENT.attack_target, COMPONENT.attack_damage)
    :set_mask(0)
    :on_run(function(entities, entity_count)
      for i = 1, entity_count, 1 do
        local attack = entities[i]

        local enemy = ktrpg.Entity:by_id(attack:get(COMPONENT.attacker))
        local target = ktrpg.Entity:by_id(attack:get(COMPONENT.attack_target))

        if target ~= nil then
          local total_damage = target:get(COMPONENT.damage_received)
          local damage = attack:get(COMPONENT.attack_damage)

          if total_damage == nil then
            total_damage = 0
          end

          total_damage = total_damage + damage

          target:set(COMPONENT.damage_received, total_damage)

          local enemy_name
          if enemy ~= nil then
            enemy_name = enemy:get(COMPONENT.name)
          end
          local target_name = target:get(COMPONENT.name)

          if enemy_name == nil then
            enemy_name = "Unknown Entity"
          end

          if target_name == nil then
            target_name = "Unknown Entity"
          end

          print(":: " .. enemy_name .. " dealt " .. damage .. " damage to " .. target_name)
        end

        attack:kill()
      end
    end),
  take_damage = ktrpg.System
    :new("Take Damage")
    :requires(COMPONENT.max_health, COMPONENT.health, COMPONENT.damage_received)
    :set_mask(0)
    :on_run(function(entities, entity_count)
      for i = 1, entity_count, 1 do
        local e = entities[i]

        local mh = e:get(COMPONENT.max_health)
        local h = e:get(COMPONENT.health)
        local nh = e:get(COMPONENT.health) - e:get(COMPONENT.damage_received)
        e:set(COMPONENT.damage_received, 0)

        if nh < 0 then
          nh = 0
        elseif nh > mh then
          nh = mh
        end

        e:set(COMPONENT.health, nh)

        local name = e:get(COMPONENT.name)

        if name == nil then
          name = "Unknown Entity"
        end

        if nh < h then
          print(":: " .. name .. " received " .. (h - nh) .. " damage")
        elseif nh > h then
          print(":: " .. name .. " healed " .. (nh - h) .. " hp")
        end
      end
    end),
}

local player = create_player()

player:set(COMPONENT.location, LOCATION.forest)

do
  local enemy = ktrpg.Entity:new()

  enemy:set(COMPONENT.name, "Goblin")
  enemy:set(COMPONENT.location, LOCATION.forest)

  enemy:set(COMPONENT.max_health, 30)
  enemy:set(COMPONENT.health, 30)
  enemy:set(COMPONENT.attack, 4)
  enemy:set(COMPONENT.damage_received, 0)

  enemy:set(COMPONENT.mob_ai, MOB_AI.hunt)
  enemy:set(COMPONENT.species, SPECIES.goblin)
end

local function run()
  SYSTEM.mob_hunt:run()

  ktrpg.System:run_debug_system()

  SYSTEM.physical_damage:run()
  SYSTEM.take_damage:run()

  ktrpg.System:run_debug_system()
end

for i = 1, 3, 1 do
  run()
end
