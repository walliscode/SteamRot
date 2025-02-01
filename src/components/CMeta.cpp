#include "CMeta.h"

// returns if the entity is active, this will be for looping purposes
const bool CMeta::getActive() const { return m_active; }

// sets the entity as active
void CMeta::activate() { m_active = true; }

// sets the entity as inactive
void CMeta::deactivate() { m_active = false; }

// // sets the component from flatbuffers data
// void CMeta::fromFlatbuffers(const SteamRot::rawData::MetaComponent* meta) {
//
// 	m_active = meta->active();
// }

// converts the component to json data
json CMeta::toJSON() {

  json j;
  j["active"] = m_active;
  return j;
}
