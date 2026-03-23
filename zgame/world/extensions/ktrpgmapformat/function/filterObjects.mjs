/**
 * @typedef {object} filteredObjects
 * @property {MapObject[]} locations
 * @property {MapObject[]} doors
 */

/**
 * @param {TileMap} map
 * @returns {filteredObjects}
 */
export const filterObjects = (map) => {
  const filteredObjects = {
    locations: [],
    doors: [],
  };

  for (let li = 0; li < map.layerCount; li++) {
    /**@type ObjectGroup */
    const layer = map.layers[li];

    if (!layer.isObjectLayer) return;

    for (let oi = 0; oi < layer.objectCount; oi++) {
      const obj = layer.objects[oi];

      switch (obj.className) {
        case "Location": {
          filteredObjects.locations.push(obj);
          break;
        }
        case "Door": {
          filteredObjects.doors.push(obj);
          break;
        }
        default:
          tiled.error(
            `Unknown Class: object (${obj.id}) at layer ${layer.name}`,
          );
      }
    }
  }

  return filteredObjects;
};
