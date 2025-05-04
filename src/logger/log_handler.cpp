////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "log_handler.h"
#include "spdlog/spdlog.h"

////////////////////////////////////////////////////////////
void steamrot::log_handler::ProcessLog(
    const spdlog::level::level_enum &log_level,
    const steamrot::log_handler::LogCode &log_code,
    const std::string &message) {
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
void steamrot::log_handler::ProcessInfoLog(

    const steamrot::log_handler::LogCode &log_level,
    const std::string &message) {
  // log info level messages
  spdlog::get("global_logger")->info(message);
};

////////////////////////////////////////////////////////////
void steamrot::log_handler::ProcessErrorLog(

    const steamrot::log_handler::LogCode &log_level,

    const std::string &message) {

  // log error level messages
  spdlog::get("global_logger")->error(message);

  // throw exception to cause call back on stack
  throw std::runtime_error(message);
};
