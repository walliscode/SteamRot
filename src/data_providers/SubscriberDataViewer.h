/////////////////////////////////////////////////
/// @file
/// @brief Concrete implementation of subscriber data viewer.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSubscriberDataProvider.h"
#include "FailInfo.h"
#include "SubscriberConfig.h"
#include <expected>
#include <memory>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SubscriberDataViewer
/// @brief Concrete viewer class for accessing subscriber data.
///
/// This class provides a composition-based approach to viewing subscriber
/// data from various data sources. It wraps a FlatbuffersSubscriberDataProvider
/// to provide access to subscriber configurations.
///
/// Usage:
/// ```cpp
/// // Create viewer with FlatBuffers data
/// SubscriberDataViewer viewer(flatbuffers_data);
/// auto configs = viewer.GetSubscriberConfigs();
/// ```
/////////////////////////////////////////////////
class SubscriberDataViewer {
private:
  std::unique_ptr<FlatbuffersSubscriberDataProvider> m_provider;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a FlatBuffers vector of SubscriberConfigFbs.
  ///
  /// @param subscriber_configs_fbs Pointer to FlatBuffers vector of
  /// SubscriberConfigFbs
  /////////////////////////////////////////////////
  SubscriberDataViewer(
      const flatbuffers::Vector<flatbuffers::Offset<SubscriberConfigFbs>>
          *subscriber_configs_fbs);

  /////////////////////////////////////////////////
  /// @brief Get subscriber configurations from this data source.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const;
};

} // namespace steamrot
