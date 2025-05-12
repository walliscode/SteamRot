////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "log_handler.h"
#include "spdlog/spdlog.h"

namespace steamrot {
namespace log_handler {
////////////////////////////////////////////////////////////
void ProcessLog(const spdlog::level::level_enum &log_level,
                const LogCode &log_code, const std::string &message) {
  switch (log_level) {
  case spdlog::level::info:
    ProcessInfoLog(log_code, message);
    break;
  case spdlog::level::err:
    ProcessErrorLog(log_code, message);
    break;
  default:
    break;
  }
};

////////////////////////////////////////////////////////////
void ProcessInfoLog(const LogCode &log_level, const std::string &message) {
  // log info level messages
  spdlog::get("global_logger")->info(message);
};

////////////////////////////////////////////////////////////
void ProcessErrorLog(const LogCode &log_level, const std::string &message) {

  // log error level messages
  spdlog::get("global_logger")->error(message);

  std::string jsonlastlogpattern = {
      "{\"time\": \"%Y-%m-%dT%H:%M:%S.%f%z\", \"name\": \"%n\", \"level\": "
      "\"%^%l%$\", \"process\": %P, \"thread\": %t, \"message\": \"%v\"}]\n}"};
  spdlog::set_pattern(jsonlastlogpattern);

  // add in finish message as deconstruction won't happen properly
  spdlog::get("global_logger")->error("Finished.");

  spdlog::drop("global_logger");

  // throw exception to cause call back on stack
  throw std::runtime_error(message);
};

} // namespace log_handler
} // namespace steamrot
