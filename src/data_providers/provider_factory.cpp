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
#include "FlatbuffersFragmentDataProvider.h"
#include "FlatbuffersSceneDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
IEngineDataProvider &GetEngineDataProvider() {
  static FlatbuffersEngineDataProvider provider;
  return provider;
}

/////////////////////////////////////////////////
ISceneDataProvider &GetSceneDataProvider() {
  static FlatbuffersSceneDataProvider provider;
  return provider;
}

/////////////////////////////////////////////////
IAssetDataProvider &GetAssetDataProvider() {
  static FlatbuffersAssetDataProvider provider;
  return provider;
}

/////////////////////////////////////////////////
IFragmentDataProvider &GetFragmentDataProvider() {
  static FlatbuffersFragmentDataProvider provider;
  return provider;
}

} // namespace steamrot
