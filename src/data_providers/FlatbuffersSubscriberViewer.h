/////////////////////////////////////////////////
/// @file
/// @brief Concrete implementation of subscriber data viewer.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "ISubscriberViewer.h"
#include "Subscriber.h"
#include "subscriber_generated.h"
#include <expected>
#include <memory>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersSubscriberViewer
/// @brief Concrete viewer class for accessing subscriber data.
///
/// This class provides direct access to subscriber data
/// from FlatBuffers sources. It handles the conversion from FlatBuffers
/// SubscriberFbs to native Subscriber structs.
///
/// Usage:
/// ```cpp
/// // Create viewer with FlatBuffers data
/// FlatbuffersSubscriberViewer viewer(flatbuffers_data);
/// auto subscribers = viewer.GetSubscribers();
/// ```
/////////////////////////////////////////////////
class FlatbuffersSubscriberViewer : public ISubscriberViewer {
private:
  const flatbuffers::Vector<flatbuffers::Offset<SubscriberFbs>>
      *m_subscribers_fbs;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a FlatBuffers vector of SubscriberFbs.
  ///
  /// @param subscribers_fbs Pointer to FlatBuffers vector of SubscriberFbs
  /////////////////////////////////////////////////
  FlatbuffersSubscriberViewer(
      const flatbuffers::Vector<flatbuffers::Offset<SubscriberFbs>>
          *subscribers_fbs);

  /////////////////////////////////////////////////
  /// @brief Get subscribers from this data source.
  ///
  /// Converts FlatBuffers SubscriberFbs objects to Subscriber structs
  /// and wraps them in shared pointers.
  ///
  /// @return Vector of shared pointers to Subscriber objects or failure
  /// information
  /////////////////////////////////////////////////
  std::expected<std::vector<std::shared_ptr<Subscriber>>, FailInfo>
  GetSubscribers() const override;
};

} // namespace steamrot
