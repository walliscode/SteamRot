/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of ISubscriberDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISubscriberDataProvider.h"
#include "subscriber_config_generated.h"
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersSubscriberDataProvider
/// @brief FlatBuffers implementation of ISubscriberDataProvider.
///
/// Converts FlatBuffers SubscriberConfigFbs objects to SubscriberConfig structs.
/// This provider is designed to work with FlatBuffers vectors embedded in other
/// data structures (e.g., LogicData, EngineState, UIState).
/////////////////////////////////////////////////
class FlatbuffersSubscriberDataProvider : public ISubscriberDataProvider {
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
  FlatbuffersSubscriberDataProvider(
      const flatbuffers::Vector<flatbuffers::Offset<SubscriberConfigFbs>>
          *subscriber_configs_fbs);

  /////////////////////////////////////////////////
  /// @brief Load all subscriber configurations from the FlatBuffers data.
  ///
  /// @return Vector of SubscriberConfig objects or failure information
  /////////////////////////////////////////////////
  std::expected<std::vector<SubscriberConfig>, FailInfo>
  LoadSubscriberConfigs() const override;
};

} // namespace steamrot
