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

/////////////////////////////////////////////////
/// @class ISubscriberDataViewer
/// @brief Mixin interface for viewing subscriber data from providers.
///
/// This interface can be inherited by any data provider that contains
/// subscriber configuration data. It provides a standard way to access
/// subscriber data from various data sources (EngineState, UIState,
/// LogicData, etc.).
///
/// This enables layered data access - different data providers can
/// expose their subscriber data through this common interface.
///
/// Usage:
/// ```cpp
/// // A data provider that contains subscriber data
/// class MyDataProvider : public IMyDataProvider, 
///                        public ISubscriberDataViewer {
/// public:
///   std::expected<std::vector<SubscriberConfig>, FailInfo>
///   GetSubscriberConfigs() const override {
///     // Implementation
///   }
/// };
///
/// // Using the viewer interface
/// ISubscriberDataViewer& viewer = GetDataProviderWithSubscribers();
/// auto configs = viewer.GetSubscriberConfigs();
/// ```
/////////////////////////////////////////////////
class ISubscriberDataViewer {
public:
  virtual ~ISubscriberDataViewer() = default;

  /////////////////////////////////////////////////
  /// @brief Get subscriber configurations from this data source.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const = 0;
};

} // namespace steamrot
