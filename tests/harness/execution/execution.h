/////////////////////////////////////////////////
/// @file
/// @brief Main header for the execution module
///
/// This module provides level-based executors for testing at different
/// granularities:
/// - Level 1: Logic Execution (single Logic class or free function)
/// - Level 2: Scene Tick (all logics for a scene)
/// - Level 3: SceneManager Update (process subscriptions + update scenes)
/// - Level 4: Full Game Loop
///
/// Include this header to access all executor levels.
/////////////////////////////////////////////////

#pragma once

// Level 1: Logic Execution
#include "logic_executor.h"

// Level 2: Scene Tick Execution
#include "scene_executor.h"

// Level 3: SceneManager Update Execution
#include "scene_manager_executor.h"

// Level 4: Full Game Loop Execution
#include "game_loop_executor.h"

// Unified Execution Runner
#include "execution_runner.h"

namespace steamrot::tests::execution {

// All executor declarations are in their respective headers

} // namespace steamrot::tests::execution
