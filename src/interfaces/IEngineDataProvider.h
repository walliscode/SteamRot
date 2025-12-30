/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading engine data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineData.h"
#include "FailInfo.h"
#include <expected>

// Forward declaration
namespace steamrot {
class FlatbuffersSubscriberViewer;
}

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEngineDataProvider
/// @brief Interface for loading all engine data types.
/////////////////////////////////////////////////
class IEngineDataProvider {

public:
  virtual ~IEngineDataProvider() = default;

  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};

} // namespace steamrot
