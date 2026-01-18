/////////////////////////////////////////////////
/// @file
/// @brief unit tests for configure_asset_config functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_asset_config.h"
#include "AssetConfig.h"
#include "asset_config_generated.h"
#include "flatbuffers/buffer.h"
#include "flatbuffers/vector.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureAssetConfig handles nullptr Flatbuffers data gracefully",
          "[unit][configure_asset_config]") {
  steamrot::AssetConfig asset_config;
  const steamrot::AssetConfigFbs *null_data = nullptr;
  auto result =
      steamrot::data::configure::ConfigureAssetConfig(asset_config, null_data);
  REQUIRE(!result.has_value());

  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "AssetConfigFbs data is null.");
}

TEST_CASE("ConfigureAssetConfig correctly configures AssetConfig from "
          "flatbuffers data",
          "[unit][configure_asset_config]") {

  // create flatbuffers data for testing
  flatbuffers::FlatBufferBuilder builder;
  // set up FontDataFbs
  flatbuffers::Offset<flatbuffers::String> font_name_one =
      builder.CreateString("font_one");
  flatbuffers::Offset<steamrot::FontDataFbs> font_one =
      steamrot::CreateFontDataFbs(builder, font_name_one);
  flatbuffers::Offset<flatbuffers::String> font_name_two =
      builder.CreateString("font_two");
  flatbuffers::Offset<steamrot::FontDataFbs> font_two =
      steamrot::CreateFontDataFbs(builder, font_name_two);

  // set up ui styles
  flatbuffers::Offset<
      flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>>
      styles = builder.CreateVector(
          std::vector<flatbuffers::Offset<flatbuffers::String>>{
              builder.CreateString("style_one"),
              builder.CreateString("style_two")});

  // set up AssetConfigFbs
  std::vector<flatbuffers::Offset<steamrot::FontDataFbs>> fonts_vector = {
      font_one, font_two};
  flatbuffers::Offset<
      flatbuffers::Vector<flatbuffers::Offset<steamrot::FontDataFbs>>>
      fonts_offset = builder.CreateVector(fonts_vector);
  flatbuffers::Offset<steamrot::AssetConfigFbs> asset_config_offset =
      steamrot::CreateAssetConfigFbs(builder, fonts_offset, styles);

  // finish the buffer
  builder.Finish(asset_config_offset);

  // get a pointer to the root object
  const steamrot::AssetConfigFbs *asset_config_fbs =
      steamrot::GetAssetConfigFbs(builder.GetBufferPointer());

  REQUIRE(asset_config_fbs != nullptr);

  // test that the configure function correctly configures AssetConfig
  steamrot::AssetConfig asset_config;

  auto configure_result = steamrot::data::configure::ConfigureAssetConfig(
      asset_config, asset_config_fbs);
  if (!configure_result.has_value()) {
    FAIL(configure_result.error().message);
  }

  // verify fonts
  REQUIRE(asset_config.fonts.size() == 2);
  REQUIRE(asset_config.fonts[0].name == "font_one");
  REQUIRE(asset_config.fonts[1].name == "font_two");

  // verify ui styles
  REQUIRE(asset_config.ui_styles.size() == 2);
  REQUIRE(asset_config.ui_styles[0] == "style_one");
  REQUIRE(asset_config.ui_styles[1] == "style_two");
}
