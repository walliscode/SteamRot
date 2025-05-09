#include "CMeta.h"

void to_json(json &j, const CMeta &meta) {

  j = json{{"m_entity_active", meta.m_entity_active}};
};
void from_json(const json &j, CMeta &meta) {
  j.at("m_entity_active").get_to(meta.m_entity_active);
}
