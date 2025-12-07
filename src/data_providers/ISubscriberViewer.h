/////////////////////////////////////////////////
/// @file
/// @brief Mixin interface for data providers that contain subscriber data.
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

class ISubscriberViewer {
public:
  virtual ~ISubscriberViewer() = default;

  /////////////////////////////////////////////////
  /// @brief Get subscriber configurations from this data source.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const = 0;
};

} // namespace steamrot
