////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <DateTimeLogger.h>
#include <chrono>

////////////////////////////////////////////////////////////
DateTimeLogger::DateTimeLogger() {
  // Get the current time as a time_point
  auto now = std::chrono::system_clock::now();

  // Convert to time_t, which represents the time in seconds since epoch
  std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

  // Format the time into a string
  std::ostringstream oss;
  oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
  current_timestamp = oss.str();
}

////////////////////////////////////////////////////////////
std::string DateTimeLogger::getTimestamp() const { return current_timestamp; }
