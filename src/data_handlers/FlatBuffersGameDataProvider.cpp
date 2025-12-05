////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatBuffersGameDataProvider
////////////////////////////////////////////////////////////

#include "FlatBuffersGameDataProvider.h"

namespace steamrot {

////////////////////////////////////////////////////////////
std::expected<const EngineDataFbs *, FailInfo>
FlatBuffersGameDataProvider::LoadEngineData() const {
  return m_loader.ProvideEngineData();
}

////////////////////////////////////////////////////////////
std::expected<const SceneManagerData *, FailInfo>
FlatBuffersGameDataProvider::LoadSceneManagerData() const {
  return m_loader.ProvideSceneManagerData();
}

////////////////////////////////////////////////////////////
std::expected<const SceneDataData *, FailInfo>
FlatBuffersGameDataProvider::LoadSceneData(const SceneType scene_type) const {
  return m_loader.ProvideDefaultSceneData(scene_type);
}

////////////////////////////////////////////////////////////
std::expected<const UserPreferencesData *, FailInfo>
FlatBuffersGameDataProvider::LoadUserPreferences() const {
  return m_loader.ProvideDefaultUserPreferencesData();
}

////////////////////////////////////////////////////////////
std::expected<const EngineConfigData *, FailInfo>
FlatBuffersGameDataProvider::LoadEngineConfig() const {
  return m_loader.ProvideEngineConfigData();
}

////////////////////////////////////////////////////////////
std::expected<const UIStyleData *, FailInfo>
FlatBuffersGameDataProvider::LoadUIStyle(const std::string &style_name) const {
  return m_loader.ProvideUIStylesData(style_name);
}

////////////////////////////////////////////////////////////
std::expected<const EngineCoreDataFbs *, FailInfo>
FlatBuffersGameDataProvider::LoadEngineCoreData() const {
  return m_loader.ProvideEngineCoreData();
}

////////////////////////////////////////////////////////////
std::expected<const SceneCoreDataFbs *, FailInfo>
FlatBuffersGameDataProvider::LoadSceneCoreData(
    const SceneType scene_type) const {
  return m_loader.ProvideSceneCoreData(scene_type);
}

} // namespace steamrot
