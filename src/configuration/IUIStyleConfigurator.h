////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of IUIStyleConfigurator interface
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

namespace steamrot {

// Forward declaration for UIStyleData (will be defined when implementing
// FlatBuffers configurator)
struct UIStyleData;

////////////////////////////////////////////////////////////
/// @class IUIStyleConfigurator
/// @brief Interface for configuring UIStyle instances from style data
////////////////////////////////////////////////////////////
class IUIStyleConfigurator {
protected:
  ////////////////////////////////////////////////////////////
  /// @brief Reference to AssetManager for font loading
  ////////////////////////////////////////////////////////////
  const AssetManager &m_asset_manager;

public:
  ////////////////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param asset_manager Reference to AssetManager
  ////////////////////////////////////////////////////////////
  IUIStyleConfigurator(const AssetManager &asset_manager)
      : m_asset_manager(asset_manager) {}

  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor
  ////////////////////////////////////////////////////////////
  virtual ~IUIStyleConfigurator() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Configure a UIStyle from style data
  ///
  /// This method should be implemented by concrete configurators to handle
  /// their specific data format (FlatBuffers, JSON, etc.)
  ///
  /// @param style_name Name of the style to configure
  /// @return Configured UIStyle or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<UIStyle, FailInfo>
  ConfigureStyle(const std::string &style_name) = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Configure multiple UIStyles
  ///
  /// @param style_names Vector of style names to configure
  /// @return Map of style name to UIStyle, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
  ConfigureStyles(const std::vector<std::string> &style_names) = 0;
};

} // namespace steamrot
