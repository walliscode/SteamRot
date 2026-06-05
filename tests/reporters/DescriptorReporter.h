/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the DescriptorReporter class.
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

class DescriptorReporter : public Catch::CumulativeReporterBase {
public:
  DescriptorReporter(Catch::ReporterConfig &&config);

  using CumulativeReporterBase::CumulativeReporterBase;

  static std::string getDescription() {
    return "Descriptor reporter with sectioned key-value failure output";
  }

  void testRunStarting(const Catch::TestRunInfo &testRunInfo) override;
  void testCaseStarting(const Catch::TestCaseInfo &testInfo) override;
  void testCasePartialStarting(const Catch::TestCaseInfo &testInfo,
                               uint64_t partNumber) override;
  void sectionStarting(const Catch::SectionInfo &sectionInfo) override;
  void assertionStarting(const Catch::AssertionInfo &assertionInfo) override;
  void assertionEnded(const Catch::AssertionStats &assertionStats) override;
  void sectionEnded(const Catch::SectionStats &sectionStats) override;
  void testCasePartialEnded(const Catch::TestCaseStats &testCaseStats,
                            uint64_t partNumber) override;
  void testCaseEnded(const Catch::TestCaseStats &testCaseStats) override;
  void testRunEnded(const Catch::TestRunStats &testRunStats) override;
  void testRunEndedCumulative() override;
};

CATCH_REGISTER_REPORTER("descriptor", DescriptorReporter);
