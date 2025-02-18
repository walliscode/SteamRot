#include "CMeta.h"

void to_json(json &j, const CMeta &meta) {

  j = json{{"m_entity_active", meta.m_entity_active},
           {"m_component_flags", static_cast<size_t>(meta.m_component_flags)}};
};

void from_json(const json &j, CMeta &meta) {
  j.at("m_entity_active").get_to(meta.m_entity_active);
}
