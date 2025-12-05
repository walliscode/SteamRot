/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading fragment data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "Fragment.h"
#include <expected>
#include <map>
#include <string>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IFragmentDataProvider
/// @brief Interface for loading fragment data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
///
/// Note: Fragment struct already exists and is used as-is.
///
/// Usage:
/// ```cpp
/// IFragmentDataProvider& provider = GetFragmentDataProvider();
/// auto result = provider.LoadFragment("fragment_name");
/// if (result.has_value()) {
///   const Fragment& fragment = result.value();
///   // Use Fragment struct
/// }
/// ```
/////////////////////////////////////////////////
class IFragmentDataProvider {
public:
  virtual ~IFragmentDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load a single fragment by name.
  ///
  /// @param fragment_name Name of the fragment to load
  /// @return Fragment data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<Fragment, FailInfo>
  LoadFragment(const std::string &fragment_name) const = 0;

  /////////////////////////////////////////////////
  /// @brief Load multiple fragments by name.
  ///
  /// @param fragment_names Vector of fragment names to load
  /// @return Map of fragment names to Fragment objects or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::map<std::string, Fragment>, FailInfo>
  LoadAllFragments(const std::vector<std::string> &fragment_names) const = 0;
};

} // namespace steamrot
