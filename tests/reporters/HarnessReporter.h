/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the HarnessReporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "catch2/catch_test_case_info.hpp"
#include "catch2/internal/catch_move_and_forward.hpp"
#include "catch2/reporters/catch_reporter_cumulative_base.hpp"
#include "catch2/reporters/catch_reporter_registrars.hpp"

class HarnessReporter : public Catch::CumulativeReporterBase {

public:
  HarnessReporter(Catch::ReporterConfig &&_config)
      : Catch::CumulativeReporterBase(CATCH_MOVE(_config)) {}

  // for ease of access in function naming
  using CumulativeReporterBase::CumulativeReporterBase;

  static std::string getDescription() {
    return "Harness Reporter for testing purposes";
  }
  void testRunStarting(const Catch::TestRunInfo &testRunInfo) override;
  void testCaseStarting(const Catch::TestCaseInfo &testInfo) override;
  void testCasePartialStarting(Catch::TestCaseInfo const &testInfo,
                               uint64_t partNumber) override;

  void testCasePartialEnded(Catch::TestCaseStats const &testCaseStats,
                            uint64_t partNumber) override;
  void testCaseEnded(const Catch::TestCaseStats &testCaseStats) override;
  void testRunEnded(const Catch::TestRunStats &testRunStats) override;

  void testRunEndedCumulative() override;

  void assertionStarting(Catch::AssertionInfo const &assertionInfo) override;
  void assertionEnded(Catch::AssertionStats const &assertionStats) override;
};

CATCH_REGISTER_REPORTER("harness", HarnessReporter);
