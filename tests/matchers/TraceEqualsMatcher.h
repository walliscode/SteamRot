/////////////////////////////////////////////////
/// @file
/// @brief Catch2 matcher that compares AnalysisTrace values via a formatter.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnalysisEvent.h"
#include "DescriptorFormatter.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TraceEqualsMatcher
/// @brief Catch2 matcher that compares two @c AnalysisTrace values by
///        formatting both with a @c DescriptorFormatter and diffing the output.
///
/// On mismatch, @c describe() reports both the expected and actual formatted
/// outputs and highlights the first differing line, so failures pinpoint
/// exactly which event diverged without requiring raw string comparisons
/// in test code.
///
/// The formatter is injected at construction time, so any concrete
/// @c DescriptorFormatter subclass (e.g. @c TerminalDescriptorFormatter) can
/// be used.  Both the expected trace and the formatter must outlive the
/// matcher.
///
/// Example:
/// @code
/// steamrot::logic::descriptors::TerminalDescriptorFormatter fmt;
/// REQUIRE_THAT(result.m_trace,
///              steamrot::tests::EqualsTrace(expected_trace, fmt));
/// @endcode
/////////////////////////////////////////////////
class TraceEqualsMatcher : public Catch::Matchers::MatcherBase<
                               steamrot::logic::descriptors::AnalysisTrace> {
public:
  /////////////////////////////////////////////////
  /// @brief Construct a matcher with an expected trace and a formatter.
  ///
  /// @param expected  The expected @c AnalysisTrace to match against.
  /// @param formatter Formatter used to render both traces for comparison.
  /////////////////////////////////////////////////
  TraceEqualsMatcher(
      const steamrot::logic::descriptors::AnalysisTrace &expected,
      const steamrot::logic::descriptors::DescriptorFormatter &formatter);

  /////////////////////////////////////////////////
  /// @brief Compare @p actual against the expected trace.
  ///
  /// Formats both traces with the stored formatter.  If the outputs differ,
  /// stores a full diff description (expected output, actual output, and the
  /// first diverging line) for use by @c describe().
  ///
  /// @param actual The @c AnalysisTrace produced by the descriptor under test.
  /// @return @c true if the formatted outputs match exactly.
  /////////////////////////////////////////////////
  bool match(
      const steamrot::logic::descriptors::AnalysisTrace &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Return a description for Catch2 failure output.
  ///
  /// Returns @c "equals trace" on success.  On failure, includes the full
  /// expected and actual formatted outputs and identifies the first
  /// diverging line.
  /////////////////////////////////////////////////
  std::string describe() const override;

private:
  const steamrot::logic::descriptors::AnalysisTrace &m_expected;
  const steamrot::logic::descriptors::DescriptorFormatter &m_formatter;
  mutable std::string m_mismatch_description{};
};

/////////////////////////////////////////////////
/// @brief Helper factory function for @c TraceEqualsMatcher.
///
/// @param expected  Expected @c AnalysisTrace.
/// @param formatter Formatter to use for comparison.
/// @return @c TraceEqualsMatcher instance.
/////////////////////////////////////////////////
inline TraceEqualsMatcher EqualsTrace(
    const steamrot::logic::descriptors::AnalysisTrace &expected,
    const steamrot::logic::descriptors::DescriptorFormatter &formatter) {
  return TraceEqualsMatcher(expected, formatter);
}

} // namespace steamrot::tests

namespace Catch {

template <> struct StringMaker<steamrot::logic::descriptors::AnalysisEvent> {
  static std::string
  convert(const steamrot::logic::descriptors::AnalysisEvent &event) {
    using steamrot::logic::descriptors::TraceEventKind;

    switch (event.kind) {
    case TraceEventKind::EmtpyPartGraph:
      return "AnalysisEvent{kind=EmptyPartGraph}";
    case TraceEventKind::EmtpyChainSteps:
      return "AnalysisEvent{kind=EmptyChainSteps}";
    case TraceEventKind::NodeEval:
      return "AnalysisEvent{kind=NodeEval, part_id=" +
             std::to_string(event.part_id) + ", predicate=\"" +
             event.predicate_name + "\"}";
    case TraceEventKind::MovingToNeighbour:
      return "AnalysisEvent{kind=MovingToNeighbour, from_id=" +
             std::to_string(event.from_id) +
             ", to_id=" + std::to_string(event.to_id) + "}";
    case TraceEventKind::Backtracking:
      return "AnalysisEvent{kind=Backtracking, from_id=" +
             std::to_string(event.from_id) +
             ", to_id=" + std::to_string(event.to_id) + "}";
    case TraceEventKind::ScopeBegin:
      return "AnalysisEvent{kind=ScopeBegin, scope=\"" + event.scope_name +
             "\"}";
    case TraceEventKind::ScopeEnd:
      return "AnalysisEvent{kind=ScopeEnd, scope=\"" + event.scope_name +
             "\", result=" + (event.result ? "true" : "false") + "}";
    case TraceEventKind::MachinaPartResult:
      return "AnalysisEvent{kind=MachinaPartResult, part=\"" +
             event.predicate_name +
             "\", result=" + (event.result ? "true" : "false") + "}";
    case TraceEventKind::ValidSubgraphIsolated:
      return "AnalysisEvent{kind=ValidSubgraphIsolated}";
    case TraceEventKind::InvalidSubgraphIsolated:
      return "AnalysisEvent{kind=InvalidSubgraphIsolated}";
    default:
      return "AnalysisEvent{kind=<unknown>}";
    }
  }
};

template <> struct StringMaker<steamrot::logic::descriptors::AnalysisTrace> {
  static std::string
  convert(const steamrot::logic::descriptors::AnalysisTrace &trace) {
    return "AnalysisTrace{" + std::to_string(trace.size()) +
           " events; see EqualsTrace diff below}";
  }
};

} // namespace Catch
