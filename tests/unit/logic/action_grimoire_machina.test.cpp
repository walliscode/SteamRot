/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "FailInfo.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto result = steamrot::logic::action::grimoire_machina::
      InitialiseActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  auto result =
      steamrot::logic::action::grimoire_machina::ClearActiveMachinaFormScaffold(
          grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("SetColor GrowthPoint sets origin color to hover_color "
          "when is_mouse_over is true",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrowthPoint growth_point;
  growth_point.is_mouse_over = true;
  growth_point.hover_color = sf::Color::Red;
  growth_point.base_color = sf::Color::Blue;
  steamrot::logic::action::grimoire_machina::SetColor(growth_point);
  REQUIRE(growth_point.origin.getFillColor() == growth_point.hover_color);
}

TEST_CASE("SetColor GrowthPoint sets origin color to base_color "
          "when is_mouse_over is false",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrowthPoint growth_point;
  growth_point.is_mouse_over = false;
  growth_point.hover_color = sf::Color::Red;
  growth_point.base_color = sf::Color::Blue;
  steamrot::logic::action::grimoire_machina::SetColor(growth_point);
  REQUIRE(growth_point.origin.getFillColor() == growth_point.base_color);
}

TEST_CASE("SetColor Socket sets circle color to hover_color "
          "when is_mouse_over is true",
          "[unit][actions][grimoire_machina]") {
  steamrot::Socket socket;
  socket.is_mouse_over = true;
  socket.hover_color = sf::Color::Cyan;
  socket.base_color = sf::Color::White;
  steamrot::logic::action::grimoire_machina::SetColor(socket);
  REQUIRE(socket.circle.getFillColor() == socket.hover_color);
}

TEST_CASE("SetColor Socket sets circle color to base_color "
          "when is_mouse_over is false",
          "[unit][actions][grimoire_machina]") {
  steamrot::Socket socket;
  socket.is_mouse_over = false;
  socket.hover_color = sf::Color::Cyan;
  socket.base_color = sf::Color::White;
  steamrot::logic::action::grimoire_machina::SetColor(socket);
  REQUIRE(socket.circle.getFillColor() == socket.base_color);
}

TEST_CASE("SetColor FragmentInstance sets all socket colors based on "
          "is_mouse_over",
          "[unit][actions][grimoire_machina]") {
  steamrot::Fragment fragment;
  steamrot::Socket socket_hovered;
  socket_hovered.is_mouse_over = true;
  socket_hovered.hover_color = sf::Color::Cyan;
  socket_hovered.base_color = sf::Color::White;

  steamrot::Socket socket_not_hovered;
  socket_not_hovered.is_mouse_over = false;
  socket_not_hovered.hover_color = sf::Color::Cyan;
  socket_not_hovered.base_color = sf::Color::White;

  steamrot::FragmentInstance fragment_instance{
      fragment, {}, {socket_hovered, socket_not_hovered}};
  steamrot::logic::action::grimoire_machina::SetColor(fragment_instance);

  REQUIRE(fragment_instance.sockets[0].circle.getFillColor() ==
          socket_hovered.hover_color);
  REQUIRE(fragment_instance.sockets[1].circle.getFillColor() ==
          socket_not_hovered.base_color);
}

TEST_CASE(
    "SetColor JointInstance sets all socket colors based on is_mouse_over",
    "[unit][actions][grimoire_machina]") {
  steamrot::Joint joint;
  steamrot::Socket socket_hovered;
  socket_hovered.is_mouse_over = true;
  socket_hovered.hover_color = sf::Color::Cyan;
  socket_hovered.base_color = sf::Color::White;

  steamrot::Socket socket_not_hovered;
  socket_not_hovered.is_mouse_over = false;
  socket_not_hovered.hover_color = sf::Color::Cyan;
  socket_not_hovered.base_color = sf::Color::White;

  steamrot::JointInstance joint_instance{
      joint, {}, {socket_hovered, socket_not_hovered}};
  steamrot::logic::action::grimoire_machina::SetColor(joint_instance);

  REQUIRE(joint_instance.sockets[0].circle.getFillColor() ==
          socket_hovered.hover_color);
  REQUIRE(joint_instance.sockets[1].circle.getFillColor() ==
          socket_not_hovered.base_color);
}

TEST_CASE("GetAllFragmentNames returns the string names of all fragments in "
          "the GrimoireMachina",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  SECTION("No fragments in GrimoireMachina") {
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::GetAllFragmentNames(
            grimoire_machina);
    REQUIRE(fragment_names.empty());
  }

  SECTION("Multiple fragments in GrimoireMachina") {
    grimoire_machina.m_all_fragments = {{"Fragment1", steamrot::Fragment{}},
                                        {"Fragment2", steamrot::Fragment{}},
                                        {"Fragment3", steamrot::Fragment{}}};
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::GetAllFragmentNames(
            grimoire_machina);
    REQUIRE(fragment_names.size() == 3);
    REQUIRE(fragment_names[0] == "Fragment1");
    REQUIRE(fragment_names[1] == "Fragment2");
    REQUIRE(fragment_names[2] == "Fragment3");
  }
}

TEST_CASE("AddFragmentToScaffold returns error for unknown fragment name",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto result = steamrot::logic::action::grimoire_machina::AddFragmentToScaffold(
      grimoire_machina, "NonExistentFragment");

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
}

TEST_CASE("AddFragmentToScaffold adds a FragmentInstance to the scaffold",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::Fragment fragment;
  fragment.name = "TestFragment";
  fragment.sockets = {sf::Vector2f{10.f, 20.f}, sf::Vector2f{30.f, 40.f}};
  grimoire_machina.m_all_fragments["TestFragment"] = fragment;

  auto result = steamrot::logic::action::grimoire_machina::AddFragmentToScaffold(
      grimoire_machina, "TestFragment");

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments[0].sockets.size() == 2);
}

TEST_CASE(
    "AddFragmentToScaffold initialises the scaffold when none is present",
    "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);

  steamrot::Fragment fragment;
  fragment.name = "TestFragment";
  grimoire_machina.m_all_fragments["TestFragment"] = fragment;

  auto result = steamrot::logic::action::grimoire_machina::AddFragmentToScaffold(
      grimoire_machina, "TestFragment");

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
}
