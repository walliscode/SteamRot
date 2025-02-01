#include "CTransform.h"

void CTransform::fromFlatbuffers(
    const SteamRot::rawData::TransformComponent *transform) {
  position.x = transform->position()->x();
  position.y = transform->position()->y();
  velocity.x = transform->velocity()->x();
  velocity.y = transform->velocity()->y();
}

json CTransform::toJSON() {
  json j;
  j["position"] = {position.x, position.y};
  j["velocity"] = {velocity.x, velocity.y};
  return j;
}
