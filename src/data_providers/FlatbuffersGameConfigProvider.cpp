/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersGameConfigProvider.
/////////////////////////////////////////////////

#include "FlatbuffersGameConfigProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<const EngineDataFbs *, FailInfo>
FlatbuffersGameConfigProvider::LoadEngineConfig() const {
  return m_loader.ProvideEngineData();
}

/////////////////////////////////////////////////
std::expected<const SceneManagerData *, FailInfo>
FlatbuffersGameConfigProvider::LoadSceneManagerConfig() const {
  return m_loader.ProvideSceneManagerData();
}

} // namespace steamrot
