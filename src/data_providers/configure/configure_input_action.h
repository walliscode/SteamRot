/////////////////////////////////////////////////
/// @file
/// @brief Forwarding header for input-action configure free functions.
///
/// The input-action configure functions have moved to sfml_event_convert.h
/// in the events library (steamrot::events::convert namespace) so that
/// SFMLEventConverter can coordinate calling them directly on its own registry.
///
/// Include sfml_event_convert.h directly for:
///   - events::convert::ConfigureInputAction
///   - events::convert::ConfigureInputActionMapping
///   - events::convert::ConfigureInputActionRegistry
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "sfml_event_convert.h"
