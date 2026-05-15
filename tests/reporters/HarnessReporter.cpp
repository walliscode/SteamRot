/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the HarnessReporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "HarnessReporter.h"
#include <conmat.h>
#include <filesystem>
#include <iostream>

/////////////////////////////////////////////////
void HarnessReporter::testRunStarting(const Catch::TestRunInfo &testRunInfo) {
  std::cout << "\n";
  std::cout << conmat::Divider("=", 40) << "\n";
  std::cout << "Starting Data Driven Harness Tests\n";
  std::cout << conmat::Divider("=", 40) << "\n";
}

void HarnessReporter::testCaseStarting(const Catch::TestCaseInfo &test_info) {
  std::cout << conmat::Divider("-", 40) << "\n";
  // TEST CASE header
  std::cout << conmat::Colorize("TEST CASE: ", conmat::Color::Blue) << "\n";
  // Test case name
  std::cout << "\t" << "name: " << test_info.name << "\n";

  // turn file name into filesystem path
  std::filesystem::path file_path(test_info.lineInfo.file);
  // print out file name only
  std::cout << "\t" << "file: " << file_path.filename().string() << "\n";
  // print out line number
  std::cout << "\t" << "line: " << test_info.lineInfo.line << "\n";
  // Test case tags
  std::cout << "\t" << "tags: " << test_info.tagsAsString() << "\n";

  std::cout << conmat::Divider("-", 40) << "\n";
}

void HarnessReporter::testCasePartialStarting(
    Catch::TestCaseInfo const &test_info, uint64_t part_number) {}

void HarnessReporter::testCasePartialEnded(
    Catch::TestCaseStats const &testCaseStats, uint64_t partNumber) {}

void HarnessReporter::testCaseEnded(const Catch::TestCaseStats &testCaseStats) {
}

void HarnessReporter::testRunEnded(const Catch::TestRunStats &testRunStats) {}

/////////////////////////////////////////////////
void HarnessReporter::testRunEndedCumulative() {}

void HarnessReporter::assertionStarting(
    Catch::AssertionInfo const &assertion_info) {}

void HarnessReporter::assertionEnded(
    Catch::AssertionStats const &assertion_stats) {

  // print out any INFO messages associated with the assertion
  for (const auto &msg : assertion_stats.infoMessages) {
    std::cout << msg.message << "\n";
  }

  std::cout
      << assertion_stats.assertionResult.m_resultData.reconstructExpression()
      << "\n";
}
