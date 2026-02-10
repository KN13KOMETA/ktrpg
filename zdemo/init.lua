--[[ NOT USED IN DEMO
ktrpg.System:new("Name"):get_name()
ktrpg.System:new("Name"):excludes()
ktrpg.System:new("Name"):get_mask()
ktrpg.System:new("Name"):get_entity_count()

local ok, err = ktrpg.Component:set_limit(2000000)
local ok, err = ktrpg.Entity:set_limit(2000000)
local ok, err = ktrpg.System:set_limit(2000000)

ktrpg.Component:get_limit()
ktrpg.Entity:get_limit()
ktrpg.System:get_limit()
--]]

---@type ktrpg
local ktrpg = require("ktrpg")

local physical_damage = require("physical_damage")
local create_player = require("create_player")

TB = "\n:: "
TE = " ::"

NONAME = "Unknown Entity"

LOCATION = {
  player_home = 1,
  forest = 2,
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

  gold = ktrpg.Component:new("int", "Gold"),

  tag_player = ktrpg.Component:new("tag", "Player"),
  tag_dead = ktrpg.Component:new("tag", "Dead"),

  max_health = ktrpg.Component:new("num", "Max Health"),
  health = ktrpg.Component:new("num", "Health"),
  attack = ktrpg.Component:new("num", "Attack"),
  damage_received = ktrpg.Component:new("num", "Damage Received"),
  attacked_by = ktrpg.Component:new("int", "Attacked By"),

  attacker = ktrpg.Component:new("int", "Attacker"),
  attack_target = ktrpg.Component:new("int", "Attack Target"),
  attack_damage = ktrpg.Component:new("num", "Attack Damage"),

  mob_ai = ktrpg.Component:new("tag", "Mob AI"),
  species = ktrpg.Component:new("tag", "Species"),
}

SYSTEM = {
  debug = ktrpg.System:new("__KTRPG_DEBUG"),
  player_turn = ktrpg.System
    :new("Player Turn")
    :requires(
      COMPONENT.tag_player,
      COMPONENT.name,
      COMPONENT.location,
      COMPONENT.gold,
      COMPONENT.max_health,
      COMPONENT.health,
      COMPONENT.attack
    )
    :excludes(COMPONENT.tag_dead)
    :set_mask(0)
    :on_run(function(entities, entity_count)
      if entity_count == 0 then
        print("No players alive, stopping...")
        os.exit(0)
      end

      for i = 1, entity_count, 1 do
        local e = entities[i]
        local player = {
          name = e:get(COMPONENT.name),
          location = e:get(COMPONENT.location),
          gold = e:get(COMPONENT.gold),
          max_health = e:get(COMPONENT.max_health),
          health = e:get(COMPONENT.health),
          attack = e:get(COMPONENT.attack),
          attacked_by = e:get(COMPONENT.attacked_by),
        }

        local enemy
        local enemy_name

        if player.attacked_by ~= nil then
          enemy = ktrpg.Entity:by_id(player.attacked_by)
        end

        if enemy ~= nil then
          enemy_name = enemy:get(COMPONENT.name)
        end

        if enemy_name == nil then
          enemy_name = NONAME
        end

        while true do
          print(TB .. player.name .. " turn" .. TE)

          print("x) exit")
          print("s) Check status (doesn't waste turn)")
          print("i) Idle")
          print("l) Change location")

          if enemy ~= nil then
            print("a) Attack " .. enemy_name .. " (enemy that attacked you)")
          end

          io.write("Your action: ")
          local act = io.read()
          local act_list = {
            x = function()
              os.exit(0)
            end,
            s = function()
              print(TB .. player.name .. " STATUS" .. TE)

              if player.location == LOCATION.player_home then
                print("Location: " .. player.name .. " room")
              elseif player.location == LOCATION.forest then
                print("Location: forest")
              else
                print("Location: void (ah sht here we go again)")
              end

              print("Gold: " .. player.gold)
              print("Health: " .. player.health .. "/" .. player.max_health)
              print("Attack strength: " .. player.attack)

              if player.attacked_by == nil then
                return
              end

              if enemy == nil then
                return
              end

              print("Attacked By " .. enemy_name)
            end,
            i = function()
              print(player.name .. " decided to skip their turn")
              return 1
            end,
            a = function()
              physical_damage(e, enemy)
              return 1
            end,
            l = function()
              print(TB .. "LOCATION SELECT" .. TE)

              local llist = {
                [LOCATION.forest] = {
                  name = "forest",
                  key = "f",
                },
                [LOCATION.player_home] = {
                  name = "Player home",
                  key = "h",
                },
              }

              llist[player.location] = nil

              for key, value in pairs(llist) do
                if value == nil then
                  goto continue
                end

                print(value.key .. ") go to " .. value.name)

                ::continue::
              end

              io.write("Select: ")
              local act = io.read()

              for key, value in pairs(llist) do
                if value == nil then
                  goto continue
                end

                if act == value.key then
                  e:set(COMPONENT.location, key)
                  return 1
                end

                ::continue::
              end
            end,
          }

          local sel = act_list[act]

          if sel == nil then
            print("Unknown action")
            goto continue
          end

          if sel() == 1 then
            break
          end

          ::continue::
        end
      end
    end),
  mob_hunt = ktrpg.System
    :new("Mob Hunt")
    :requires(COMPONENT.location, COMPONENT.attack, COMPONENT.mob_ai, COMPONENT.species)
    :excludes(COMPONENT.tag_dead)
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
              physical_damage(entity, enemy)
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

        if enemy:get(COMPONENT.tag_dead) ~= nil or target:get(COMPONENT.tag_dead) ~= nil then
          target = nil
        end

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

            target:set(COMPONENT.attacked_by, enemy:get_id())
          end
          local target_name = target:get(COMPONENT.name)

          if enemy_name == nil then
            enemy_name = NONAME
          end

          if target_name == nil then
            target_name = NONAME
          end

          print(TB .. enemy_name .. " dealt " .. damage .. " damage to " .. target_name)
        end

        attack:kill()
      end
    end),
  take_damage = ktrpg.System
    :new("Take Damage")
    :requires(COMPONENT.max_health, COMPONENT.health, COMPONENT.damage_received)
    :excludes(COMPONENT.tag_dead)
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
          name = NONAME
        end

        if nh < h then
          print(TB .. name .. " received " .. (h - nh) .. " damage")
        elseif nh > h then
          print(TB .. name .. " healed " .. (nh - h) .. " hp")
        end
      end
    end),
  death = ktrpg.System:new("Death"):requires(COMPONENT.health):set_mask(0):on_run(function(entities, entity_count)
    for i = 1, entity_count, 1 do
      local e = entities[i]

      if e:get(COMPONENT.health) > 0 then
        goto continue
      end

      local entity = {
        name = e:get(COMPONENT.name),
        gold = e:get(COMPONENT.gold),
      }

      local total_gold
      local attacker = {
        id = e:get(COMPONENT.attacked_by),
      }

      if attacker.id ~= nil then
        attacker.e = ktrpg.Entity:by_id(attacker.id)

        attacker.gold = attacker.e:get(COMPONENT.gold)
        attacker.name = attacker.e:get(COMPONENT.name)
      end

      if entity.gold ~= nil and attacker.gold ~= nil then
        total_gold = entity.gold + attacker.gold
      end

      if entity.name == nil then
        entity.name = NONAME
      end

      if attacker.name == nil then
        attacker.name = NONAME
      end

      e:set(COMPONENT.tag_dead, 1)
      if total_gold ~= nil then
        e:set(COMPONENT.gold, 0)
        attacker.e:set(COMPONENT.gold, total_gold)
      end

      if total_gold ~= nil and total_gold < attacker.gold then
        print(TB .. attacker.name .. " killed " .. entity.name .. " and lost " .. (entity.gold * -1) .. " gold")
      elseif total_gold ~= nil and total_gold > attacker.gold then
        print(TB .. attacker.name .. " killed " .. entity.name .. " and got " .. entity.gold .. " gold")
      else
        print(TB .. attacker.name .. " killed " .. entity.name)
      end

      ::continue::
    end
  end),
  clear_attacked_by = ktrpg.System
    :new("Clear Attacked By")
    :requires(COMPONENT.attacked_by)
    :set_mask(0)
    :on_run(function(entities, entity_count)
      for i = 1, entity_count, 1 do
        local e = entities[i]

        local attacker_id = e:get(COMPONENT.attacked_by)

        if attacker_id == nil then
          e:remove(COMPONENT.attacked_by)
          goto continue
        end

        local attacker = ktrpg.Entity:by_id(attacker_id)

        if attacker == nil then
          e:remove(COMPONENT.attacked_by)
          goto continue
        end

        if attacker:get(COMPONENT.tag_dead) == nil then
          goto continue
        end

        ::continue::
      end
    end),
}

local player = create_player()

player:set(COMPONENT.location, LOCATION.forest)

do
  local enemy = ktrpg.Entity:new()

  enemy:set(COMPONENT.name, "Goblin")
  enemy:set(COMPONENT.location, LOCATION.forest)

  enemy:set(COMPONENT.gold, 5)

  enemy:set(COMPONENT.max_health, 30)
  enemy:set(COMPONENT.health, 30)
  enemy:set(COMPONENT.attack, 40)
  enemy:set(COMPONENT.damage_received, 0)

  enemy:set(COMPONENT.mob_ai, MOB_AI.hunt)
  enemy:set(COMPONENT.species, SPECIES.goblin)
end

local function run()
  SYSTEM.player_turn:run()

  SYSTEM.mob_hunt:run()

  SYSTEM.physical_damage:run()
  SYSTEM.take_damage:run()
  SYSTEM.death:run()
  SYSTEM.clear_attacked_by:run()

  SYSTEM.debug:run()
end

for i = 1, 22, 1 do
  run()
end
