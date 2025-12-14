/////////////////////////////////////////////////
/// @file
/// @brief Implementation of provider factory functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "provider_factory.h"
#include "FlatbuffersAssetDataProvider.h"
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSceneConfigurator.h"
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersSceneManagerDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}

/////////////////////////////////////////////////
ISceneManagerDataProvider &GetSceneManagerDataProvider() {
  static FlatbuffersSceneManagerDataProvider provider;
  return provider;
}

/////////////////////////////////////////////////
IAssetDataProvider &GetAssetDataProvider() {
  static FlatbuffersAssetDataProvider provider;
  return provider;
}
/////////////////////////////////////////////////
ISceneConfigurator &GetSceneConfigurator() {
  static FlatbuffersSceneConfigurator configurator;
  return configurator;
}

/////////////////////////////////////////////////
ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}
} // namespace steamrot
