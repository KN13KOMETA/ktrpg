/**@param {TileMap} map*/
export const getMapObjects = (map) => {
  /**@type {MapObject[]} */
  const mapObjects = [];

  for (const mapLayer of map.layers) {
    const layers = [mapLayer];

    while (layers.length != 0) {
      const layer = layers.pop();

      if (layer.isObjectLayer) {
        for (const layerObject of layer.objects) mapObjects.push(layerObject);
      } else if (layer.isGroupLayer) {
        for (const childLayer of layer.layers) layers.push(childLayer);
      } else {
        return `Layer "${layer.name}" has unknown type`;
      }
    }
  }

  return mapObjects;
};
