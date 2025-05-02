////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Logger.h"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "steamrot_directory_paths.h"
#include <memory>

////////////////////////////////////////////////////////////
Logger::Logger(const std::string &logger_name) {

  // all sink creation functions need to be called before logger registry
  CreateInfoSink();
  CreateErrorSink();

  // create global logger and register sinks
  m_logger =
      std::make_shared<spdlog::logger>(logger_name, begin(sinks), end(sinks));
  // finally, register globally
  ;
  spdlog::register_logger(m_logger);

  // set up logger for json output and formatting
  m_logger->set_pattern("{\n \"log\": [");

  // intialise log files
  m_logger->info("");
  m_logger->error("");

  std::string jsonpattern = {
      "{\"time\": \"%Y-%m-%dT%H:%M:%S.%f%z\", \"name\": \"%n\", \"level\": "
      "\"%^%l%$\", \"process\": %P, \"thread\": %t, \"message\": \"%v\"},"};

  m_logger->set_pattern(jsonpattern);
};

////////////////////////////////////////////////////////////
void Logger::CloseLogger() {
  // All we're doing below is setting the same log format, without the "," at
  // the end
  std::string jsonlastlogpattern = {
      "{\"time\": \"%Y-%m-%dT%H:%M:%S.%f%z\", \"name\": \"%n\", \"level\": "
      "\"%^%l%$\", \"process\": %P, \"thread\": %t, \"message\": \"%v\"}"};
  spdlog::set_pattern(jsonlastlogpattern);

  // below is our last log entry
  m_logger->info("Finished.");
  m_logger->error("Finished.");

  // set the last pattern to close out the "log" json array and the closing
  // brace
  spdlog::set_pattern("]\n}");

  // this writes out the closed array to the file
  m_logger->info("");
  m_logger->error("");

  spdlog::drop("global_logger");
}
////////////////////////////////////////////////////////////
void Logger::CreateInfoSink() {

  // info sink output file
  std::string info_log_path = getLogsFolder().string() + "/" + "info.json";

  // create info sink, boolean is for file truncation (reset file)
  auto info_sink{
      std::make_shared<spdlog::sinks::basic_file_sink_st>(info_log_path, true)};

  // modify info_sink
  info_sink->set_level(spdlog::level::info);

  // add to Logger sinks vector
  sinks.push_back(info_sink);
}
////////////////////////////////////////////////////////////
void Logger::CreateErrorSink() {

  // error sink output file
  std::string error_log_path = getLogsFolder().string() + "/" + "error.json";

  // create error sink, boolean is for file truncation (reset file)
  auto error_sink{std::make_shared<spdlog::sinks::basic_file_sink_st>(
      error_log_path, true)};

  // modify error_sink
  error_sink->set_level(spdlog::level::err);

  // sort json pattern for sink
  error_sink->set_pattern("[");

  // add to Logger sinks vector
  sinks.push_back(error_sink);
}
