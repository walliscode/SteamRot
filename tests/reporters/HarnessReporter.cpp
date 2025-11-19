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
#include <format>
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
    Catch::TestCaseInfo const &test_info, uint64_t part_number) {

  std::cout << conmat::Divider("-", 40) << "\n";
  std::cout << conmat::Colorize(
                   std::format("\tSTARTING GENERATION [{}]", part_number),
                   conmat::Color::Cyan)
            << "\n";
  std::cout << conmat::Divider("-", 40) << "\n";
}

void HarnessReporter::testCasePartialEnded(
    Catch::TestCaseStats const &testCaseStats, uint64_t partNumber) {

  std::cout << conmat::Divider("-", 40) << "\n";
  std::cout << conmat::Colorize(
                   std::format("\tENDING GENERATION [{}]", partNumber),
                   conmat::Color::Cyan)
            << "\n";
  std::cout << conmat::Divider("-", 40) << "\n";
}
void HarnessReporter::testCaseEnded(const Catch::TestCaseStats &testCaseStats) {

  std::cout << conmat::Divider("=", 40) << "\n";
  std::cout << (testCaseStats.totals.assertions.allOk()
                    ? conmat::Colorize("[PASSED]", conmat::Color::Green)
                    : conmat::Colorize("[FAILED]", conmat::Color::Red))
            << "\n";
  std::cout << conmat::Divider("=", 40) << "\n";
}

void HarnessReporter::testRunEnded(const Catch::TestRunStats &testRunStats) {

  std::cout << "\n";
  std::cout << conmat::Divider("=", 40) << "\n";
  std::cout << "Completed Data Driven Harness Tests\n";
  std::cout << conmat::Divider("=", 40) << "\n";
}

/////////////////////////////////////////////////
void HarnessReporter::testRunEndedCumulative() {
  std::cout << "HarnessReporter::testRunEndedCumulative";
}

void HarnessReporter::assertionStarting(
    Catch::AssertionInfo const &assertion_info) {
  std::cout << "Assertion starting: " << assertion_info.lineInfo << "\n";
  std::cout << "\t" << "macro name: " << assertion_info.macroName << "\n";
  std::cout << "\t"
            << "description: " << assertion_info.capturedExpression << "\n";
  std::cout << "\t"
            << "result disposition: " << assertion_info.resultDisposition
            << "\n";
}

void HarnessReporter::assertionEnded(
    Catch::AssertionStats const &assertion_stats) {

  std::cout << "Assertion ended: "
            << assertion_stats.assertionResult.getMessage() << "\n";
}
