////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of IUIStyleDataProvider interface
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FailInfo.h"
#include <expected>
#include <memory>
#include <string>
#include <vector>

namespace steamrot {

// Forward declaration - concrete type depends on implementation
// (FlatBuffers UIStyleData, JSON data, etc.)
struct UIStyleDataWrapper;

////////////////////////////////////////////////////////////
/// @class IUIStyleDataProvider
/// @brief Interface for loading UI style data from various sources
////////////////////////////////////////////////////////////
class IUIStyleDataProvider {
public:
  ////////////////////////////////////////////////////////////
  /// @brief Virtual destructor
  ////////////////////////////////////////////////////////////
  virtual ~IUIStyleDataProvider() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Load UI style data for a specific style name
  ///
  /// @param style_name Name of the style to load
  /// @return Pointer to style data wrapper or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<UIStyleDataWrapper>, FailInfo>
  ProvideUIStyleData(const std::string &style_name) = 0;

  ////////////////////////////////////////////////////////////
  /// @brief Load multiple UI styles by name
  ///
  /// @param style_names Vector of style names to load
  /// @return Vector of style data wrappers, or error
  ////////////////////////////////////////////////////////////
  virtual std::expected<std::vector<std::unique_ptr<UIStyleDataWrapper>>,
                        FailInfo>
  ProvideUIStylesData(const std::vector<std::string> &style_names) = 0;
};

} // namespace steamrot
