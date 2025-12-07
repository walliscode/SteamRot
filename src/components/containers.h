/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "../core/type_traits.h"
#include <magic_enum/magic_enum.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief ComponentRegister is a tuple of all the components for the game
/// "One source of truth"
/////////////////////////////////////////////////
typedef std::tuple<CMeta, CUserInterface, CMachinaForm, CGrimoireMachina,
                   CUIState>
    ComponentRegister;

/////////////////////////////////////////////////
/// @brief size of the component register
/////////////////////////////////////////////////
constexpr size_t kComponentRegisterSize =
    std::tuple_size<ComponentRegister>::value;

/////////////////////////////////////////////////
/// @brief typedef for EntityMemoryPool
/////////////////////////////////////////////////
using EntityMemoryPool =
    ComponentContainer<ComponentRegister>::ComponentVectorTuple;

/////////////////////////////////////////////////
/// @brief Helper to print a tuple to an ostream
/////////////////////////////////////////////////
template <typename... Args>
std::ostream &operator<<(std::ostream &os, const std::tuple<Args...> &) {
  // Output nothing (or a simple placeholder if you prefer)
  return os;
}

}; // namespace steamrot
