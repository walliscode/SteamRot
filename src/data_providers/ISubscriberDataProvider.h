/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading subscriber configuration data.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SubscriberConfig.h"
#include <expected>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ISubscriberDataProvider
/// @brief Interface for loading subscriber configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, etc.).
///
/// Note: SubscriberConfig struct is used as the intermediate format
/// between data sources and the SubscriberFactory.
///
/// Usage:
/// ```cpp
/// ISubscriberDataProvider& provider = GetSubscriberDataProvider();
/// auto result = provider.LoadSubscriberConfigs();
/// if (result.has_value()) {
///   const std::vector<SubscriberConfig>& configs = result.value();
///   // Use configs with SubscriberFactory
/// }
/// ```
/////////////////////////////////////////////////
class ISubscriberDataProvider {
public:
  virtual ~ISubscriberDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load all subscriber configurations.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<SubscriberConfig>, FailInfo>
  LoadSubscriberConfigs() const = 0;
};

} // namespace steamrot
