/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for general-purpose
/// descriptors/helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <string>

namespace steamrot::logic::descriptors {

////////////////////////////////////////////////
/// Lifting utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Graph-level query builders
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Generic combinators
///
/// Work uniformly with NodeDescriptor and ChainDescriptor. Template
/// deduction ensures that both arguments must be the same descriptor level;
/// mixing levels is a compile error.
///
/// Each combinator:
///  - Composes a synthesised name from the inputs (e.g. "(a AND b)").
///  - Evaluates both operands (no short-circuit) and merges their traces
///    into the returned result's @c m_trace.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when both @p a and @p b are true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) && b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc and_(Desc a, Desc b) {
  std::string name = "(" + a.GetName() + " AND " + b.GetName() + ")";
  return Desc{std::move(name),
              typename Desc::FnType{
                  [fa = std::move(a), fb = std::move(b)](auto &&...args) {
                    auto ra = fa(std::forward<decltype(args)>(args)...);
                    auto rb = fb(std::forward<decltype(args)>(args)...);
                    std::decay_t<decltype(ra)> result{static_cast<bool>(ra) &&
                                                      static_cast<bool>(rb)};
                    Merge(result.m_trace, std::move(ra.m_trace));
                    Merge(result.m_trace, std::move(rb.m_trace));
                    return result;
                  }}};
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that is true when either @p a or @p b is true.
///
/// @tparam Desc Any descriptor type.
/// @param a First descriptor.
/// @param b Second descriptor.
/// @return Combined descriptor returning a(...) || b(...).
/////////////////////////////////////////////////
template <typename Desc> Desc or_(Desc a, Desc b) {
  std::string name = "(" + a.GetName() + " OR " + b.GetName() + ")";
  return Desc{std::move(name),
              typename Desc::FnType{
                  [fa = std::move(a), fb = std::move(b)](auto &&...args) {
                    auto ra = fa(std::forward<decltype(args)>(args)...);
                    auto rb = fb(std::forward<decltype(args)>(args)...);
                    std::decay_t<decltype(ra)> result{static_cast<bool>(ra) ||
                                                      static_cast<bool>(rb)};
                    Merge(result.m_trace, std::move(ra.m_trace));
                    Merge(result.m_trace, std::move(rb.m_trace));
                    return result;
                  }}};
}

/////////////////////////////////////////////////
/// @brief Return a descriptor that negates @p a.
///
/// @tparam Desc Any descriptor type.
/// @param a Descriptor to negate.
/// @return Descriptor returning !a(...).
/////////////////////////////////////////////////
template <typename Desc> Desc not_(Desc a) {
  std::string name = "NOT " + a.GetName();
  return Desc{std::move(name),
              typename Desc::FnType{[fa = std::move(a)](auto &&...args) {
                auto ra = fa(std::forward<decltype(args)>(args)...);
                std::decay_t<decltype(ra)> result{!static_cast<bool>(ra)};
                Merge(result.m_trace, std::move(ra.m_trace));
                return result;
              }}};
}
} // namespace steamrot::logic::descriptors
