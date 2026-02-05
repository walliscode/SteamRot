/////////////////////////////////////////////////
/// @file
/// @brief Untit tests for configure_fragment
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_fragment.h"
#include "FailInfo.h"
#include "types_generated.h"
#include "view_direction_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureFragment returns unexpected when given nullptr",
          "[ConfigureFragment]") {
  steamrot::Fragment fragment;
  auto result = steamrot::data::configure::ConfigureFragment(fragment, nullptr);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message == "FragmentFbs pointer is null");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureFragment configures Fragment successfully with valid data",
          "[ConfigureFragment]") {
  // Create a mock FragmentFbs object
  flatbuffers::FlatBufferBuilder builder;

  // create string
  auto name_offset = builder.CreateString("test_fragment");

  // create 2 Vector2fData for sockets
  auto socket1 = steamrot::CreateVector2fData(builder, 0.0f, 89.0f);
  auto socket2 = steamrot::CreateVector2fData(builder, 1.0f, 34.0f);
  // create vector of sockets
  auto sockets_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::Vector2fData>>(
          {socket1, socket2});

  // create ColorData for color
  auto red_color_offset =
      steamrot::CreateColorData(builder, 255, 0, 0, 255); // Red color
  auto green_color_offset =
      steamrot::CreateColorData(builder, 0, 255, 0, 255); // Green color
  auto blue_color_offset =
      steamrot::CreateColorData(builder, 0, 0, 255, 255); // Blue color

  // create 3 vertices for triangle 1
  auto vertex1 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 0.0, 0.0),
      red_color_offset);
  auto vertex2 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 34.0, 0.0),
      green_color_offset);
  auto vertex3 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 34.0, 89.0),
      blue_color_offset);
  auto triangle1 = steamrot::CreateTriangle(
      builder, builder.CreateVector<flatbuffers::Offset<steamrot::Vertex>>(
                   {vertex1, vertex2, vertex3}));
  // create 3 vertices for triangle 2
  auto vertex4 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 0.0, 0.0),
      red_color_offset);
  auto vertex5 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 0.0, 89.0),
      green_color_offset);
  auto vertex6 = steamrot::CreateVertex(
      builder, steamrot::CreateVector2fData(builder, 34.0, 89.0),
      blue_color_offset);
  auto triangle2 = steamrot::CreateTriangle(
      builder, builder.CreateVector<flatbuffers::Offset<steamrot::Vertex>>(
                   {vertex4, vertex5, vertex6}));

  // create vector of triangles
  auto triangles_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::Triangle>>(
          {triangle1, triangle2});
  // create a ViewFbs
  auto view_fbs_offset = steamrot::CreateViewFbs(
      builder, steamrot::ViewDirectionFbs_FRONT, triangles_vector);

  // create a vector of views
  auto views_vector =
      builder.CreateVector<flatbuffers::Offset<steamrot::ViewFbs>>(
          {view_fbs_offset});

  // create the FragmentFbs
  auto fragment_fbs_offset = steamrot::CreateFragmentFbs(
      builder, name_offset, sockets_vector, views_vector);

  builder.Finish(fragment_fbs_offset);
  auto fragment_fbs =
      flatbuffers::GetRoot<steamrot::FragmentFbs>(builder.GetBufferPointer());

  steamrot::Fragment fragment;
  // Call the ConfigureFragment function
  auto result =
      steamrot::data::configure::ConfigureFragment(fragment, fragment_fbs);
  REQUIRE(result.has_value());

  // Verify that the Fragment is configured correctly
  REQUIRE(fragment.name == "test_fragment");
  REQUIRE(fragment.sockets.size() == 2);
}
