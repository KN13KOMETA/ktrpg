/**@param {string} content */
export const parseLuaTemplate = (content) => {
  const template = {
    text: [],
    location: { index: -1, text: [] },
    door: { index: -1, text: [] },
  };

  const lines = content.split(/\n/g);

  let state = "";

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    switch (state) {
      case "location": {
        if (line.indexOf("</> LOCATION TEMPLATE") != -1) {
          state = "";
          continue;
        }
        template.location.text.push(line);
        break;
      }
      case "door": {
        if (line.indexOf("</> DOOR TEMPLATE") != -1) {
          state = "";
          continue;
        }
        template.door.text.push(line);
        break;
      }
      default: {
        if (line.indexOf("<> LOCATION TEMPLATE") != -1) {
          state = "location";
          template.location.index = template.text.length;
          continue;
        }

        if (line.indexOf("<> DOOR TEMPLATE") != -1) {
          state = "door";
          template.door.index = template.text.length;
          continue;
        }

        template.text.push(line);
      }
    }
  }

  /**
   * @param {MapObject[]} locationObjects
   * @param {MapObject[]} doorObjects
   */
  return (locations, doors) => {
    let content = [];

    for (let i = 0; i < template.text.length; i++) {
      if (i == template.location.index) {
        for (let li = 0; li < locations.length; li++) {
          content.push(
            template.location.text
              .join("\n")
              .replace("TEMPLATE_LOCATION_TAG", locations[li].value)
              .replace("TEMPLATE_LOCATION_NAME", locations[li].name),
          );
        }
      }

      if (i == template.door.index) {
        for (let di = 0; di < doors.length; di++) {
          content.push(
            template.door.text
              .join("\n")
              .replace("TEMPLATE_DOOR_TAG", doors[di].name)
              .replace("TEMPLATE_DOOR_LOCATION", doors[di].location)
              .replace("TEMPLATE_DOOR_DESTINATION", doors[di].destination),
          );
        }
      }

      content.push(template.text[i]);
    }

    return content.join("\n");
  };
};
