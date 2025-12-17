////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of StylesConfigurator wrapper class
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "AssetManager.h"
#include "FailInfo.h"
#include "UIStyle.h"
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace steamrot {

// Forward declaration for FlatBuffers generated type
struct UIStyleData;

////////////////////////////////////////////////////////////
/// @class StylesConfigurator
/// @brief Wrapper class for configuring UI styles from FlatBuffers data
///
/// This class provides a convenient interface for tests and other code
/// to configure UIStyle objects from FlatBuffers UIStyleData.
////////////////////////////////////////////////////////////
class StylesConfigurator {
public:
  ////////////////////////////////////////////////////////////
  /// @brief Default constructor
  ////////////////////////////////////////////////////////////
  StylesConfigurator() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Configure a single UIStyle from FlatBuffers data
  ///
  /// @param style_data FlatBuffers UIStyleData to configure from
  /// @param asset_manager AssetManager for accessing fonts
  /// @return Configured UIStyle or error
  ////////////////////////////////////////////////////////////
  std::expected<UIStyle, FailInfo>
  ConfigureStyle(const UIStyleData &style_data,
                 const AssetManager &asset_manager);

  ////////////////////////////////////////////////////////////
  /// @brief Provide a map of UIStyles by name
  ///
  /// @param asset_manager AssetManager for accessing fonts
  /// @param style_names Vector of style names to load
  /// @return Map of style name to UIStyle, or error
  ////////////////////////////////////////////////////////////
  std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ProvideUIStylesMap(const AssetManager &asset_manager,
                     const std::vector<std::string> &style_names);
};

} // namespace steamrot
