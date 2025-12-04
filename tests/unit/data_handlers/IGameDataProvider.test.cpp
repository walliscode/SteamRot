////////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for IGameDataProvider and FlatBuffersGameDataProvider
////////////////////////////////////////////////////////////

#include "FlatBuffersGameDataProvider.h"
#include "data_file_utils.h"
#include "paths.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatBuffersGameDataProvider can be instantiated",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  // Successful instantiation is the test
  SUCCEED();
}

TEST_CASE("FlatBuffersGameDataProvider::LoadEngineData returns expected data",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadEngineData();

  if (!result.has_value()) {
    FAIL("LoadEngineData failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadEngineData returned nullptr");
  }
}

TEST_CASE(
    "FlatBuffersGameDataProvider::LoadSceneManagerData returns expected data",
    "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadSceneManagerData();

  if (!result.has_value()) {
    FAIL("LoadSceneManagerData failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadSceneManagerData returned nullptr");
  }
}

TEST_CASE("FlatBuffersGameDataProvider::LoadSceneData for TITLE scene returns "
          "expected data",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadSceneData(steamrot::SceneType::SceneType_TITLE);

  if (!result.has_value()) {
    FAIL("LoadSceneData(TITLE) failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadSceneData(TITLE) returned nullptr");
  }
}

TEST_CASE("FlatBuffersGameDataProvider::LoadSceneData for CRAFTING scene "
          "returns expected data",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadSceneData(steamrot::SceneType::SceneType_CRAFTING);

  if (!result.has_value()) {
    FAIL("LoadSceneData(CRAFTING) failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadSceneData(CRAFTING) returned nullptr");
  }
}

TEST_CASE(
    "FlatBuffersGameDataProvider::LoadUserPreferences returns expected data",
    "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadUserPreferences();

  if (!result.has_value()) {
    FAIL("LoadUserPreferences failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadUserPreferences returned nullptr");
  }
}

TEST_CASE("FlatBuffersGameDataProvider::LoadEngineConfig returns expected data",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadEngineConfig();

  if (!result.has_value()) {
    FAIL("LoadEngineConfig failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadEngineConfig returned nullptr");
  }
}

TEST_CASE("FlatBuffersGameDataProvider::LoadUIStyle returns expected data for "
          "default style",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadUIStyle("default");

  if (!result.has_value()) {
    FAIL("LoadUIStyle(default) failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadUIStyle(default) returned nullptr");
  }
}

TEST_CASE(
    "FlatBuffersGameDataProvider::LoadEngineCoreData returns expected data",
    "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result = provider.LoadEngineCoreData();

  if (!result.has_value()) {
    FAIL("LoadEngineCoreData failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadEngineCoreData returned nullptr");
  }
}

TEST_CASE("FlatBuffersGameDataProvider::LoadSceneCoreData for TITLE scene "
          "returns expected data",
          "[unit][IGameDataProvider]") {
  steamrot::FlatBuffersGameDataProvider provider;
  auto result =
      provider.LoadSceneCoreData(steamrot::SceneType::SceneType_TITLE);

  if (!result.has_value()) {
    FAIL("LoadSceneCoreData(TITLE) failed: " << result.error().message);
  }
  if (result.value() == nullptr) {
    FAIL("LoadSceneCoreData(TITLE) returned nullptr");
  }
}

TEST_CASE("data::file::FileExists returns true for existing file",
          "[unit][data_file_utils]") {
  // Test with a file we know exists
  auto engine_data_path =
      steamrot::paths::GetDefaultEngineDirectory() / "engine_data.bin";

  bool exists = steamrot::data::file::FileExists(engine_data_path);
  REQUIRE(exists == true);
}

TEST_CASE("data::file::FileExists returns false for non-existent file",
          "[unit][data_file_utils]") {
  std::filesystem::path non_existent = "/non/existent/file.bin";

  bool exists = steamrot::data::file::FileExists(non_existent);
  REQUIRE(exists == false);
}

TEST_CASE("data::file::LoadBinaryFile successfully loads existing file",
          "[unit][data_file_utils]") {
  auto engine_data_path =
      steamrot::paths::GetDefaultEngineDirectory() / "engine_data.bin";

  auto result = steamrot::data::file::LoadBinaryFile(engine_data_path);

  REQUIRE(result.has_value());
  REQUIRE(result.value().size() > 0);
}

TEST_CASE("data::file::LoadBinaryFile returns error for non-existent file",
          "[unit][data_file_utils]") {
  std::filesystem::path non_existent = "/non/existent/file.bin";

  auto result = steamrot::data::file::LoadBinaryFile(non_existent);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("data::file::GetFileSize returns correct size for existing file",
          "[unit][data_file_utils]") {
  auto engine_data_path =
      steamrot::paths::GetDefaultEngineDirectory() / "engine_data.bin";

  auto result = steamrot::data::file::GetFileSize(engine_data_path);

  REQUIRE(result.has_value());
  REQUIRE(result.value() > 0);
}

TEST_CASE("data::file::GetFileSize returns error for non-existent file",
          "[unit][data_file_utils]") {
  std::filesystem::path non_existent = "/non/existent/file.bin";

  auto result = steamrot::data::file::GetFileSize(non_existent);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}
