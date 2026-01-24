local ktrpg = require("ktrpg")

print("sx")
print(ktrpg.Component:new("tag", "alt"))
print("sx 2")
print(ktrpg.Component:new("str", "sex ncomp"):get_name())

SYSTEMS = {
  print_name = ktrpg.System:new("Print Name System"),
}

print(SYSTEMS.print_name:get_name())
