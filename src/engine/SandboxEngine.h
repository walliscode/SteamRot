/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SandboxEngine class.
///
/// SandboxEngine is a thin wrapper around GameEngine that starts directly
/// into the UIExplorerScene instead of the normal TitleScene.  It is used
/// exclusively by the ui_sandbox executable.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SandboxEngine
/// @brief Engine variant that boots directly into UIExplorerScene.
/////////////////////////////////////////////////
class SandboxEngine : public GameEngine {

public:
  SandboxEngine() = default;

  /////////////////////////////////////////////////
  /// @brief Start up the engine and load UIExplorerScene.
  ///
  /// Calls the base Engine::StartUp() for common configuration
  /// (window, event handler, asset manager, etc.) then loads
  /// UIExplorerScene instead of TitleScene.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp() override;
};

} // namespace steamrot
