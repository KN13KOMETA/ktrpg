import { parseLuaTemplate } from "./ktrpgmapformat/function/parseLuaTemplate.mjs";

const getPos = ({ x, y }) => `x${Math.floor(x)}y${Math.floor(y)}`;

const currentFileName = __filename;
const luaTemplatePath = currentFileName.replace(/([^/]*)$/, "template.lua");

const luaTemplate = parseLuaTemplate(
  new TextFile(luaTemplatePath, TextFile.ReadOnly).readAll(),
);

tiled.registerMapFormat("ktrpgmap", {
  name: "ktrpg map",
  extension: "lua",

  write: (map, fileName) => {
    const locationObjects = [];
    const doorObjects = [];

    for (let li = 0; li < map.layerCount; li++) {
      /**@type ObjectGroup */
      const layer = map.layers[li];

      if (!layer.isObjectLayer) return;

      for (let oi = 0; oi < layer.objectCount; oi++) {
        const object = layer.objects[oi];

        switch (object.className) {
          case "Location": {
            locationObjects.push(layer.objects[oi]);
            break;
          }
          case "Door": {
            doorObjects.push(layer.objects[oi]);
            break;
          }
          default:
            tiled.error(
              `Unknown Class: object (${object.id}) at layer ${layer.name}`,
            );
        }
      }
    }

    const lmap = new Map();

    const insert = {
      locations: [],
      doors: [],
    };

    for (let li = 0; li < locationObjects.length; li++) {
      const lobj = locationObjects[li];
      const lnew = {
        value: lobj.property("value")?.value,
        name: `"${lobj.property("name")}"`,
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

      lmap.set(getPos(lobj), lnew);
    }

    tiled.log(lmap.size);
    lmap.forEach((v, k) => {
      tiled.log(k);
    });

    for (let di = 0; di < doorObjects.length; di++) {
      const dobj = doorObjects[di];
      const dnew = {
        name: dobj.property("name").charCodeAt(0),
        location: dobj.property("location")?.value,
        destination: dobj.property("destination")?.value,
      };
      let skip = false;

      if (dnew.name == null) {
        tiled.error('Door Object must have "name"');
        skip = true;
      }
      if (dnew.location == null) {
        const x = lmap.get(getPos(dobj)).value;
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
    tiled.log(insert.doors.length);
  },
});
