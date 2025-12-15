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
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersSceneManagerDataProvider.h"

namespace steamrot {

// forward declaration
class FlatbuffersSceneConfigurator;
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
ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}
} // namespace steamrot
