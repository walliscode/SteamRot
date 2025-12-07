/////////////////////////////////////////////////
/// @file
/// @brief Concrete implementation of subscriber data viewer.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SubscriberConfig.h"
#include "subscriber_config_generated.h"
#include <expected>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SubscriberDataViewer
/// @brief Concrete viewer class for accessing subscriber data.
///
/// This class provides direct access to subscriber configuration data
/// from FlatBuffers sources. It handles the conversion from FlatBuffers
/// SubscriberConfigFbs to native SubscriberConfig structs.
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
  const flatbuffers::Vector<flatbuffers::Offset<SubscriberConfigFbs>>
      *m_subscriber_configs_fbs;

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
  /// Converts FlatBuffers SubscriberConfigFbs objects to SubscriberConfig structs.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  std::expected<std::vector<SubscriberConfig>, FailInfo>
  GetSubscriberConfigs() const;
};

} // namespace steamrot
