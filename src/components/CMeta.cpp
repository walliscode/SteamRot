#include "CMeta.h"

// returns if the entity is active, this will be for looping purposes
const bool CMeta::getActive() const { return m_active; }

// sets the entity as active
void CMeta::activate() { m_active = true; }

// sets the entity as inactive
void CMeta::deactivate() { m_active = false; }

const SteamRot::ComponentFlags &CMeta::getComponentFlags() const {
  return m_component_flags;
}

// set component data from json
void CMeta::LoadJSONData(const json &component_config) {

  m_active = component_config["active"];
}

// converts the component to json data
json CMeta::toJSON() {

  json j;
  j["active"] = m_active;
  return j;
}
