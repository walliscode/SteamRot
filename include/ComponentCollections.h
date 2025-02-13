#pragma once
#include "CMeta.h"
#include <cstddef>
#include <magic_enum/magic_enum.hpp>
#include <vector>

//// instructions ////
// for each new component, add a new line to the ComponentTuple and to the
// ComponentFlags enum

// add all component containers here.
typedef std::tuple<CMeta> ComponentTuple;

enum class ComponentFlags : size_t {
  CMeta = 1 << 0,
};

// provides some helper functions for the ComponentFlags enum from magic_enum
template <> struct magic_enum::customize::enum_range<ComponentFlags> {
  static constexpr bool is_flags = true;
};
// primary template for handling containers of types
// currently this can handle any parameter pack, we need to add specializations
template <typename... Types> struct ComponentContainers;

// specialization for a tuple of types
// to create: a tuple of vector of types
// to create: a tuple of optional vector of types
template <typename... Types> struct ComponentContainers<std::tuple<Types...>> {

  // use of ellipsis after <Types> indicates parameter expansion
  using TypeVectorTuple = std::tuple<std::vector<Types>...>;
  using OptionalTypeVectorTuple =
      std::tuple<std::optional<std::vector<Types>>...>;
};

// for createing EntityMemoryPool data
using ComponentCollection =
    ComponentContainers<ComponentTuple>::TypeVectorTuple;

// for passing into systems for archetype specification
using ComponentSpecifier =
    ComponentContainers<ComponentTuple>::OptionalTypeVectorTuple;
