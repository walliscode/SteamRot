////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Logger.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "steamrot_directory_paths.h"
#include <memory>

////////////////////////////////////////////////////////////
Logger::Logger() {

  // create vector for all sinks for global logger
  std::vector<spdlog::sink_ptr> sinks;

  // add sinks to vector, along with custom file for each sink

  std::string error_log_path = getLogsFolder().string() + "/" + "error.txt";
  sinks.push_back(
      std::make_shared<spdlog::sinks::basic_file_sink_st>(error_log_path));

  // create global logger and register sinks
  m_global_logger = std::make_shared<spdlog::logger>(m_global_logger_name,
                                                     begin(sinks), end(sinks));
  // finally, register globally
  ;
  spdlog::register_logger(m_global_logger);
};
