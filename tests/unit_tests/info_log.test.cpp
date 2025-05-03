#include "steamrot_directory_paths.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

TEST_CASE("check info logs", "[info_log]") {

  // get the path to the info log file
  std::filesystem::path info_log_path = getLogsFolder() / "info.json";

  // check that the info log file exists
  REQUIRE(std::filesystem::exists(info_log_path));

  // check that the info log file is not empty
  std::ifstream info_log_file(info_log_path);
  REQUIRE(info_log_file.is_open());
  info_log_file.seekg(0, std::ios::end);
  std::streamsize size = info_log_file.tellg();
  info_log_file.seekg(0, std::ios::beg);
  REQUIRE(size > 0);

  // check the info log file structure
  nlohmann::json info_log_json = nlohmann::json::parse(info_log_file);
  REQUIRE(info_log_json.contains("log"));

  // check the log level (info log initiaied with error level)
  REQUIRE(info_log_json["log"].front()["level"] == "error");

  // check that first and last contain the set up messages
  REQUIRE(info_log_json["log"].front()["message"] == "Start.");
  REQUIRE(info_log_json["log"].back()["message"] == "Finished.");
}
