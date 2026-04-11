/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
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
  steamrot::Socket socket{sf::Vector2f{0.f, 0.f}};
  socket.is_mouse_over = true;
  socket.hover_color = sf::Color::Cyan;
  socket.base_color = sf::Color::White;
  steamrot::logic::action::grimoire_machina::SetColor(socket);
  REQUIRE(socket.circle.getFillColor() == socket.hover_color);
}

TEST_CASE("SetColor Socket sets circle color to base_color "
          "when is_mouse_over is false",
          "[unit][actions][grimoire_machina]") {
  steamrot::Socket socket{sf::Vector2f{0.f, 0.f}};
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
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}};

  steamrot::FragmentInstance fragment_instance{fragment};
  fragment_instance.sockets[0].is_mouse_over = true;
  fragment_instance.sockets[0].hover_color = sf::Color::Cyan;
  fragment_instance.sockets[0].base_color = sf::Color::White;
  fragment_instance.sockets[1].is_mouse_over = false;
  fragment_instance.sockets[1].hover_color = sf::Color::Cyan;
  fragment_instance.sockets[1].base_color = sf::Color::White;

  steamrot::logic::action::grimoire_machina::SetColor(fragment_instance);

  REQUIRE(fragment_instance.sockets[0].circle.getFillColor() ==
          sf::Color::Cyan);
  REQUIRE(fragment_instance.sockets[1].circle.getFillColor() ==
          sf::Color::White);
}

TEST_CASE(
    "SetColor JointInstance sets all socket colors based on is_mouse_over",
    "[unit][actions][grimoire_machina]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance joint_instance{joint};
  joint_instance.sockets[0].is_mouse_over = true;
  joint_instance.sockets[0].hover_color = sf::Color::Cyan;
  joint_instance.sockets[0].base_color = sf::Color::White;
  joint_instance.sockets[1].is_mouse_over = false;
  joint_instance.sockets[1].hover_color = sf::Color::Cyan;
  joint_instance.sockets[1].base_color = sf::Color::White;

  steamrot::logic::action::grimoire_machina::SetColor(joint_instance);

  REQUIRE(joint_instance.sockets[0].circle.getFillColor() == sf::Color::Cyan);
  REQUIRE(joint_instance.sockets[1].circle.getFillColor() == sf::Color::White);
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

TEST_CASE("GetAllJointNames returns the string names of all joints in "
          "the GrimoireMachina",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  SECTION("No joints in GrimoireMachina") {
    auto joint_names =
        steamrot::logic::action::grimoire_machina::GetAllJointNames(
            grimoire_machina);
    REQUIRE(joint_names.empty());
  }

  SECTION("Multiple joints in GrimoireMachina") {
    grimoire_machina.m_all_joints = {{"Joint1", steamrot::Joint{}},
                                     {"Joint2", steamrot::Joint{}},
                                     {"Joint3", steamrot::Joint{}}};
    auto joint_names =
        steamrot::logic::action::grimoire_machina::GetAllJointNames(
            grimoire_machina);
    REQUIRE(joint_names.size() == 3);
    REQUIRE(joint_names[0] == "Joint1");
    REQUIRE(joint_names[1] == "Joint2");
    REQUIRE(joint_names[2] == "Joint3");
  }
}

/////////////////////////////////////////////////
/// Socket constructor tests
/////////////////////////////////////////////////

TEST_CASE("Socket constructor stores the local position",
          "[unit][Socket][MachinaFormScaffold]") {
  sf::Vector2f local_pos{15.f, 25.f};
  steamrot::Socket socket{local_pos};

  REQUIRE(socket.position == local_pos);
}

TEST_CASE("Socket constructor positions the circle at the local position",
          "[unit][Socket][MachinaFormScaffold]") {
  sf::Vector2f local_pos{30.f, 50.f};
  steamrot::Socket socket{local_pos};

  // The circle position is set to world space by the owning instance
  // constructor; a standalone Socket circle starts at the origin.
  REQUIRE(socket.circle.getPosition() == sf::Vector2f{0.f, 0.f});
}

TEST_CASE("Socket constructor uses default state and colors",
          "[unit][Socket][MachinaFormScaffold]") {
  steamrot::Socket socket{{0.f, 0.f}};

  REQUIRE(socket.state == steamrot::Socket::State::Available);
  REQUIRE(socket.is_mouse_over == false);
  REQUIRE(socket.base_color == sf::Color::White);
  REQUIRE(socket.hover_color == sf::Color::Yellow);
}

/////////////////////////////////////////////////
/// FragmentInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE("FragmentInstance constructor creates one socket per Fragment socket",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}, {50.f, 60.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.sockets.size() == 3);
}

TEST_CASE("FragmentInstance constructor stores the Fragment reference",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.name = "test_fragment";
  fragment.sockets = {{0.f, 0.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(&instance.fragment == &fragment);
}

TEST_CASE("FragmentInstance constructor stores socket local positions",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.sockets[0].position == sf::Vector2f{10.f, 20.f});
  REQUIRE(instance.sockets[1].position == sf::Vector2f{30.f, 40.f});
}

TEST_CASE("FragmentInstance constructor with identity transform positions "
          "circles at local positions",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.sockets[0].circle.getPosition() == sf::Vector2f{10.f, 20.f});
  REQUIRE(instance.sockets[1].circle.getPosition() == sf::Vector2f{30.f, 40.f});
}

TEST_CASE("FragmentInstance constructor applies initial transform to circle "
          "positions",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}};

  sf::Transform transform;
  transform.translate({100.f, 200.f});

  steamrot::FragmentInstance instance{fragment, transform};

  REQUIRE(instance.sockets[0].circle.getPosition() ==
          sf::Vector2f{110.f, 220.f});
}

TEST_CASE("FragmentInstance constructor stores the initial transform",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{0.f, 0.f}};

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::FragmentInstance instance{fragment, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("FragmentInstance constructor with no sockets creates empty sockets "
          "vector",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.sockets.empty());
}

/////////////////////////////////////////////////
/// JointInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE("JointInstance constructor creates one socket per Joint socket",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.sockets.size() == 2);
}

TEST_CASE("JointInstance constructor stores the Joint reference",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.name = "test_joint";
  joint.sockets = {{0.f, 0.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(&instance.joint == &joint);
}

TEST_CASE("JointInstance constructor stores socket local positions",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.sockets[0].position == sf::Vector2f{5.f, 10.f});
  REQUIRE(instance.sockets[1].position == sf::Vector2f{15.f, 25.f});
}

TEST_CASE("JointInstance constructor with identity transform positions circles "
          "at local positions",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.sockets[0].circle.getPosition() == sf::Vector2f{5.f, 10.f});
  REQUIRE(instance.sockets[1].circle.getPosition() == sf::Vector2f{15.f, 25.f});
}

TEST_CASE("JointInstance constructor applies initial transform to circle "
          "positions",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}};

  sf::Transform transform;
  transform.translate({100.f, 200.f});

  steamrot::JointInstance instance{joint, transform};

  REQUIRE(instance.sockets[0].circle.getPosition() ==
          sf::Vector2f{105.f, 210.f});
}

TEST_CASE("JointInstance constructor stores the initial transform",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{0.f, 0.f}};

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::JointInstance instance{joint, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("JointInstance constructor with no sockets creates empty sockets "
          "vector",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.sockets.empty());
}
