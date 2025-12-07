/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSubscriberViewer.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersSubscriberViewer.h"
#include "subscriber_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersSubscriberViewer::FlatbuffersSubscriberViewer(
    const flatbuffers::Vector<flatbuffers::Offset<SubscriberFbs>>
        *subscribers_fbs)
    : m_subscribers_fbs(subscribers_fbs) {}

/////////////////////////////////////////////////
std::expected<std::vector<std::shared_ptr<Subscriber>>, FailInfo>
FlatbuffersSubscriberViewer::GetSubscribers() const {
  std::vector<std::shared_ptr<Subscriber>> subscribers;

  // Handle null case
  if (!m_subscribers_fbs) {
    return subscribers; // Return empty vector if no data
  }

  // Convert each FlatBuffers SubscriberFbs to Subscriber
  for (const auto *subscriber_fbs : *m_subscribers_fbs) {
    if (!subscriber_fbs) {
      continue; // Skip null entries
    }

    // Use subscriber_factory to convert SubscriberFbs to Subscriber
    auto convert_result = subscriber_factory::CreateSubscriber(subscriber_fbs);
    if (!convert_result.has_value()) {
      // Skip entries with NONE event type
      if (convert_result.error().mode == FailMode::InvalidValue)
        continue;
      return std::unexpected(convert_result.error());
    }

    // Create shared pointer and add to vector
    subscribers.push_back(
        std::make_shared<Subscriber>(convert_result.value()));
  }

  return subscribers;
}

} // namespace steamrot
