#include "steamrot_directory_paths.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

TEST_CASE("check error logs", "[error_log]") {

  // get the path to the error log file
  std::filesystem::path error_log_path = getLogsFolder() / "error.json";

  // check that the error log file exists
  REQUIRE(std::filesystem::exists(error_log_path));

  // check that the error log file is not empty
  std::ifstream error_log_file(error_log_path);
  REQUIRE(error_log_file.is_open());
  error_log_file.seekg(0, std::ios::end);
  std::streamsize size = error_log_file.tellg();
  error_log_file.seekg(0, std::ios::beg);
  REQUIRE(size > 0);

  // check the error log file structure
  nlohmann::json error_log_json = nlohmann::json::parse(error_log_file);
  REQUIRE(error_log_json.contains("log"));

  // check the log level
  REQUIRE(error_log_json["log"].front()["level"] == "error");

  // check that first and last contain the set up messages
  REQUIRE(error_log_json["log"].front()["message"] == "Start.");
  REQUIRE(error_log_json["log"].back()["message"] == "Finished.");
}
