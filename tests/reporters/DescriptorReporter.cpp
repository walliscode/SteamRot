/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the DescriptorReporter class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DescriptorReporter.h"

#include <conmat.h>

#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace {

constexpr std::size_t kKeyWidth{16};

std::string SectionHeader(std::string_view label, conmat::Color color,
                          std::size_t width = 56) {
  std::ostringstream oss;
  oss << conmat::Divider("-", width) << "\n";
  oss << conmat::Colorize(std::string(label), color) << "\n";
  oss << conmat::Divider("-", width);
  return oss.str();
}

void PrintKeyValue(std::ostream &stream, std::string_view key,
                   const std::string &value, std::size_t indent = 1) {
  stream << conmat::Indent(indent) << std::left
         << std::setw(static_cast<int>(kKeyWidth)) << (std::string(key) + ":")
         << value << "\n";
}

void PrintMultiline(std::ostream &stream, const std::string &value,
                    std::size_t indent = 1) {
  std::istringstream input(value);
  std::string line;
  while (std::getline(input, line)) {
    stream << conmat::Indent(indent) << line << "\n";
  }
}

std::string ResultLabel(bool ok) {
  return ok ? conmat::Colorize("PASS", conmat::Color::Green)
            : conmat::Colorize("FAIL", conmat::Color::Red);
}

} // namespace

/////////////////////////////////////////////////
DescriptorReporter::DescriptorReporter(Catch::ReporterConfig &&config)
    : Catch::CumulativeReporterBase(CATCH_MOVE(config)) {
  m_preferences.shouldReportAllAssertions = true;
}

/////////////////////////////////////////////////
void DescriptorReporter::testRunStarting(
    const Catch::TestRunInfo & /*testRunInfo*/) {
  m_stream << "\n";
  m_stream << conmat::Divider("=", 64) << "\n";
  m_stream << conmat::Colorize("Starting Descriptor Trace Tests",
                               conmat::Color::Cyan)
           << "\n";
  m_stream << conmat::Divider("=", 64) << "\n";
}

/////////////////////////////////////////////////
void DescriptorReporter::testCaseStarting(const Catch::TestCaseInfo &testInfo) {
  m_stream << SectionHeader("TestCase", conmat::Color::Blue) << "\n";
  PrintKeyValue(m_stream, "name", testInfo.name);
  PrintKeyValue(m_stream, "file",
                std::filesystem::path(testInfo.lineInfo.file).filename().string());
  PrintKeyValue(m_stream, "line", std::to_string(testInfo.lineInfo.line));
  PrintKeyValue(m_stream, "tags", testInfo.tagsAsString());
}

/////////////////////////////////////////////////
void DescriptorReporter::testCasePartialStarting(
    const Catch::TestCaseInfo & /*testInfo*/, uint64_t /*partNumber*/) {}

/////////////////////////////////////////////////
void DescriptorReporter::sectionStarting(const Catch::SectionInfo &sectionInfo) {
  Catch::CumulativeReporterBase::sectionStarting(sectionInfo);
  m_stream << SectionHeader("Section", conmat::Color::Yellow, 48) << "\n";
  PrintKeyValue(m_stream, "name", sectionInfo.name);
}

/////////////////////////////////////////////////
void DescriptorReporter::assertionStarting(
    const Catch::AssertionInfo & /*assertionInfo*/) {}

/////////////////////////////////////////////////
void DescriptorReporter::assertionEnded(
    const Catch::AssertionStats &assertionStats) {
  Catch::CumulativeReporterBase::assertionEnded(assertionStats);

  const bool ok = assertionStats.assertionResult.isOk();
  m_stream << SectionHeader(ok ? "Assertion PASS" : "Assertion FAIL",
                            ok ? conmat::Color::Green : conmat::Color::Red, 40)
           << "\n";

  if (assertionStats.assertionResult.hasExpression()) {
    PrintKeyValue(
        m_stream, "expression",
        assertionStats.assertionResult.m_resultData.reconstructExpression());
  }

  if (assertionStats.assertionResult.hasMessage()) {
    PrintKeyValue(m_stream, "message",
                  std::string(assertionStats.assertionResult.getMessage()));
  }

  PrintKeyValue(m_stream, "result", ResultLabel(ok));

  for (const auto &message : assertionStats.infoMessages) {
    m_stream << conmat::Indent(1) << "info:\n";
    PrintMultiline(m_stream, std::string(message.message), 2);
  }
}

/////////////////////////////////////////////////
void DescriptorReporter::sectionEnded(const Catch::SectionStats &sectionStats) {
  Catch::CumulativeReporterBase::sectionEnded(sectionStats);
  PrintKeyValue(m_stream, "section result",
                sectionStats.assertions.failed ? ResultLabel(false)
                                               : ResultLabel(true));
}

/////////////////////////////////////////////////
void DescriptorReporter::testCasePartialEnded(
    const Catch::TestCaseStats & /*testCaseStats*/, uint64_t /*partNumber*/) {}

/////////////////////////////////////////////////
void DescriptorReporter::testCaseEnded(
    const Catch::TestCaseStats &testCaseStats) {
  Catch::CumulativeReporterBase::testCaseEnded(testCaseStats);
  m_stream << SectionHeader("TestCase Summary", conmat::Color::Magenta, 48)
           << "\n";
  PrintKeyValue(m_stream, "assertions",
                std::to_string(testCaseStats.totals.assertions.total()));
  PrintKeyValue(m_stream, "failed",
                std::to_string(testCaseStats.totals.assertions.failed));
  PrintKeyValue(m_stream, "result",
                testCaseStats.totals.assertions.failed ? ResultLabel(false)
                                                      : ResultLabel(true));
}

/////////////////////////////////////////////////
void DescriptorReporter::testRunEnded(
    const Catch::TestRunStats &testRunStats) {
  Catch::CumulativeReporterBase::testRunEnded(testRunStats);
}

/////////////////////////////////////////////////
void DescriptorReporter::testRunEndedCumulative() {
  if (!m_testRun) {
    return;
  }

  const auto &totals = m_testRun->value.totals;
  m_stream << conmat::Divider("=", 64) << "\n";
  m_stream << conmat::Colorize("Descriptor Trace Summary",
                               conmat::Color::Cyan)
           << "\n";
  m_stream << conmat::Divider("=", 64) << "\n";
  PrintKeyValue(m_stream, "test cases",
                std::to_string(totals.testCases.total()));
  PrintKeyValue(m_stream, "failed test cases",
                std::to_string(totals.testCases.failed));
  PrintKeyValue(m_stream, "assertions",
                std::to_string(totals.assertions.total()));
  PrintKeyValue(m_stream, "failed assertions",
                std::to_string(totals.assertions.failed));
  PrintKeyValue(m_stream, "result",
                totals.assertions.failed ? ResultLabel(false)
                                         : ResultLabel(true));
}
