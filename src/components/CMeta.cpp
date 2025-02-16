#include "CMeta.h"

void to_json(json &j, const CMeta &meta) {};

void from_json(const json &j, CMeta &meta) {
  j.at("m_entity_active").get_to(meta.m_entity_active);
}
