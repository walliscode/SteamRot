/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the HarnessReporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "HarnessReporter.h"
#include <conmat.h>
#include <iostream>

/////////////////////////////////////////////////
void HarnessReporter::testRunStarting(const Catch::TestRunInfo &testRunInfo) {
  std::cout << "HarnessReporter::testRunStarting" << "\n";
}

void HarnessReporter::testCaseStarting(const Catch::TestCaseInfo &testInfo) {
  std::cout << "HarnessReporter::testCaseStarting: " << "\n";
}

void HarnessReporter::testCasePartialStarting(
    Catch::TestCaseInfo const &testInfo, uint64_t partNumber) {
  std::cout << conmat::Colorize("HarnessReporter::testCasePartialStarting",
                                conmat::Color::Green)
            << testInfo.name << " partNumber: " << partNumber << "\n";
}

void HarnessReporter::testCasePartialEnded(
    Catch::TestCaseStats const &testCaseStats, uint64_t partNumber) {
  std::cout << "HarnessReporter::testCasePartialEnded: "
            << testCaseStats.testInfo << " partNumber: " << partNumber << "\n";
}
void HarnessReporter::testCaseEnded(const Catch::TestCaseStats &testCaseStats) {
  std::cout << "HarnessReporter::testCaseEnded: " << testCaseStats.testInfo
            << "\n";
}

void HarnessReporter::testRunEnded(const Catch::TestRunStats &testRunStats) {
  std::cout << "HarnessReporter::testRunEnded";
}

/////////////////////////////////////////////////
void HarnessReporter::testRunEndedCumulative() {
  std::cout << "HarnessReporter::testRunEndedCumulative";
}

void HarnessReporter::assertionStarting(
    Catch::AssertionInfo const &assertionInfo) {
  std::cout << "HarnessReporter::assertionStarting: " << assertionInfo.macroName
            << "\n";
}

void HarnessReporter::assertionEnded(
    Catch::AssertionStats const &assertionStats) {
  std::cout << "HarnessReporter::assertionEnded: " << "\n";
}
