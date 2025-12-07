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
#include "Subscriber.h"
#include <expected>
#include <memory>
#include <vector>

namespace steamrot {

class ISubscriberViewer {
public:
  virtual ~ISubscriberViewer() = default;

  /////////////////////////////////////////////////
  /// @brief Get subscribers from this data source.
  ///
  /// @return Vector of shared pointers to Subscriber objects or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<std::shared_ptr<Subscriber>>, FailInfo>
  GetSubscribers() const = 0;
};

} // namespace steamrot
