import { filterObjects } from "./function/filterObjects.mjs";
import { parseLuaTemplate } from "./function/parseLuaTemplate.mjs";

const getPos = ({ x, y }) => `x${Math.floor(x)}y${Math.floor(y)}`;

const currentFileName = __filename;
const luaTemplatePath = currentFileName.replace(/([^/]*)$/, "template.lua");

const luaTemplate = parseLuaTemplate(
  new TextFile(luaTemplatePath, TextFile.ReadOnly).readAll(),
);

tiled.registerMapFormat("ktrpgworld", {
  name: "ktrpg world",
  extension: "lua",

  write: (map, fileName) => {
    const fobj = filterObjects(map);

    const lmap = new Map();

    const insert = {
      locations: [],
      doors: [],
    };

    for (let li = 0; li < fobj.locations.length; li++) {
      const location = fobj.locations[li];
      const lnew = {
        value: location.property("value")?.value,
        name: `"${location.property("name")}"`,
      };
      let skip = false;

      if (lnew.value == null) {
        tiled.error('Location Object must have "value"');
        skip = true;
      }
      if (lnew.name == null) {
        tiled.error('Location Object must have "name"');
        skip = true;
      }

      if (skip) continue;

      insert.locations.push(lnew);

      lmap.set(getPos(location), lnew);
    }

    for (let di = 0; di < fobj.doors.length; di++) {
      const door = fobj.doors[di];
      const dnew = {
        name: door.property("name").charCodeAt(0),
        location: door.property("location")?.value,
        destination: door.property("destination")?.value,
      };
      let skip = false;

      if (dnew.name == null) {
        tiled.error('Door Object must have "name"');
        skip = true;
      }
      if (dnew.location == null) {
        const x = lmap.get(getPos(door)).value;
        if (x != null) {
          dnew.location = x;
        } else {
          tiled.error('Door Object must have "location"');
          skip = true;
        }
      }
      if (dnew.destination == null) {
        tiled.error('Door Object must have "destination"');
        skip = true;
      }

      if (skip) continue;

      insert.doors.push(dnew);
    }

    tiled.log(luaTemplate(insert.locations, insert.doors));
  },
});
