/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SubscriberDataViewer.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SubscriberDataViewer.h"

namespace steamrot {

/////////////////////////////////////////////////
SubscriberDataViewer::SubscriberDataViewer(
    const flatbuffers::Vector<flatbuffers::Offset<SubscriberConfigFbs>>
        *subscriber_configs_fbs)
    : m_provider(std::make_unique<FlatbuffersSubscriberDataProvider>(
          subscriber_configs_fbs)) {}

/////////////////////////////////////////////////
std::expected<std::vector<SubscriberConfig>, FailInfo>
SubscriberDataViewer::GetSubscriberConfigs() const {
  return m_provider->LoadSubscriberConfigs();
}

} // namespace steamrot
