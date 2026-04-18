/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the functions related to the rendering of the grimoire
/// machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "Fragment.h"
#include "GrimoireMachina.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include "action_grimoire_machina.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Helper: build a Views object with a single coloured triangle in the
/// Front slot.
/////////////////////////////////////////////////
steamrot::Views MakeViewsWithFrontTriangle(sf::Color colour = sf::Color::Red) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, colour});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Front, std::move(va));
  return views;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Fragment with a coloured triangle in the Front slot.
/////////////////////////////////////////////////
steamrot::Fragment
MakeFragmentWithFrontView(sf::Color colour = sf::Color::Green) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{5.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{25.f, 5.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{15.f, 25.f}, colour});

  steamrot::Fragment fragment;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  return fragment;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Joint with a coloured triangle in the Front slot.
/////////////////////////////////////////////////
steamrot::Joint MakeJointWithFrontView(sf::Color colour = sf::Color::Blue) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{30.f, 10.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 30.f}, colour});

  steamrot::Joint joint;
  joint.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                        std::move(va));
  return joint;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Fragment with a socket at (0,0) and a coloured
/// triangle in the Front slot, placed at origin.
///
/// Vertices at (0,0),(20,0),(10,20): bounds position={0,0} size={20,20},
/// true centroid = (10, 6.67) ≈ pixel (10, 7).
/////////////////////////////////////////////////
steamrot::Fragment
MakeFragmentWithOriginTriangle(sf::Color colour = sf::Color::Green) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, colour});

  steamrot::Fragment fragment;
  fragment.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                           std::move(va));
  fragment.sockets = {{5.f, 5.f}};
  return fragment;
}

/////////////////////////////////////////////////
/// @brief Helper: build a Joint with a single socket at local (5,0) and a
/// colored triangle in the Front slot, placed at origin.
///
/// SocketConfig: socket_count=1, radius=5, arc_min=arc_max=0 (angle 0°) so the
/// socket lands at local (5, 0). Callers that need a specific world position
/// can translate by (world_x - 5, world_y).
/////////////////////////////////////////////////
steamrot::Joint
MakeJointWithOriginTriangle(sf::Color colour = sf::Color::Blue) {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, colour});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, colour});

  steamrot::Joint joint;
  joint.movement_views.insert_or_assign(steamrot::ViewDirection::Front,
                                        std::move(va));
  joint.socket_config.socket_count = 1;
  joint.socket_config.radius = 5.f;
  joint.socket_config.has_fixed_socket = false;
  return joint;
}

} // anonymous namespace

/////////////////////////////////////////////////
/// draw_view tests
/////////////////////////////////////////////////

TEST_CASE("draw_view draws a populated view without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto views = MakeViewsWithFrontTriangle();

  REQUIRE_NOTHROW(steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("draw_view draws an empty VertexArray without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::Views views; // no entries populated

  REQUIRE_NOTHROW(steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front));
}

TEST_CASE("draw_view produces pixels for a Front-direction triangle",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto views = MakeViewsWithFrontTriangle(sf::Color::Red);
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();

  // The triangle spans roughly x=[10,20], y=[10,20]; the centroid pixel
  // (15,15) should be red after drawing.
  sf::Color centroid_pixel = image.getPixel({15, 15});
  REQUIRE(centroid_pixel == sf::Color::Red);
}

TEST_CASE("draw_view selects the correct direction slot",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  // Populate only Back slot
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{10.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{20.f, 10.f}, sf::Color::White});
  va.append(sf::Vertex{sf::Vector2f{15.f, 20.f}, sf::Color::White});

  steamrot::Views views;
  views.insert_or_assign(steamrot::ViewDirection::Back, std::move(va));

  // Drawing Front (unpopulated) should produce no visible pixels
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Front);
  texture.display();
  sf::Image image_front = texture.getTexture().copyToImage();
  REQUIRE(image_front.getPixel({15, 15}) == sf::Color::Black);

  // Drawing Back should produce white pixels
  texture.clear(sf::Color::Black);
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, views, steamrot::ViewDirection::Back);
  texture.display();
  sf::Image image_back = texture.getTexture().copyToImage();
  REQUIRE(image_back.getPixel({15, 15}) == sf::Color::White);
}

TEST_CASE("draw_view with RenderStates draws Fragment movement_views at "
          "transformed position",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto fragment = MakeFragmentWithFrontView(sf::Color::Green);

  sf::RenderStates states;
  states.transform.translate({10.f, 10.f});
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, fragment.movement_views, steamrot::ViewDirection::Front, states);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Centroid of (5,5),(25,5),(15,25) shifted by (10,10) →
  // (15,15),(35,15),(25,35) centroid pixel (25, 22) should be green.
  REQUIRE(image.getPixel({25, 22}) == sf::Color::Green);
}

TEST_CASE("draw_view with RenderStates draws Joint movement_views at "
          "transformed position",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto joint = MakeJointWithFrontView(sf::Color::Blue);

  sf::RenderStates states;
  states.transform.translate({10.f, 10.f});
  steamrot::logic::render::grimoire_machina::draw_view(
      texture, joint.movement_views, steamrot::ViewDirection::Front, states);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Centroid of (10,10),(30,10),(20,30) shifted by (10,10) → centroid (30,27)
  REQUIRE(image.getPixel({30, 27}) == sf::Color::Blue);
}

/////////////////////////////////////////////////
/// draw_socket tests
/////////////////////////////////////////////////

TEST_CASE("draw_socket draws a non-hovered socket without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::SocketState socket_state; // default: Available, not hovered

  REQUIRE_NOTHROW(steamrot::logic::render::grimoire_machina::draw_socket(
      texture, {50.f, 50.f}, socket_state));
}

TEST_CASE("draw_socket produces white pixels at world_pos when not hovered",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::SocketState socket_state;
  socket_state.is_mouse_over = false;
  steamrot::logic::render::grimoire_machina::draw_socket(texture, {50.f, 50.f},
                                                         socket_state);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Circle center is at (50,50), radius 5 — center pixel should be white.
  REQUIRE(image.getPixel({50, 50}) == sf::Color::White);
}

TEST_CASE("draw_socket produces yellow pixels at world_pos when hovered",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  steamrot::SocketState socket_state;
  socket_state.is_mouse_over = true;
  steamrot::logic::render::grimoire_machina::draw_socket(texture, {50.f, 50.f},
                                                         socket_state);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({50, 50}) == sf::Color::Blue);
}

/////////////////////////////////////////////////
/// draw_fragment_instance_sockets tests
/////////////////////////////////////////////////

TEST_CASE(
    "draw_fragment_instance_sockets draws socket circles without throwing",
    "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto fragment = MakeFragmentWithOriginTriangle();

  sf::Transform t;
  t.translate({50.f, 50.f});
  steamrot::FragmentInstance instance{fragment, t};

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_fragment_instance_sockets(
          texture, instance));
}

TEST_CASE(
    "draw_fragment_instance_sockets renders socket at the transform-mapped "
    "world pos",
    "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  auto fragment = MakeFragmentWithOriginTriangle();
  // socket is at local (5,5); translate by (40,40) → world pos (45,45)
  sf::Transform t;
  t.translate({40.f, 40.f});
  steamrot::FragmentInstance instance{fragment, t};

  steamrot::logic::render::grimoire_machina::draw_fragment_instance_sockets(
      texture, instance);
  texture.display();

  const sf::Image image = texture.getTexture().copyToImage();
  // Circle centre at (45,45) should be white.
  REQUIRE(image.getPixel({45, 45}) == sf::Color::White);
}

/////////////////////////////////////////////////
/// draw_joint_instance_sockets tests
/////////////////////////////////////////////////

TEST_CASE("draw_joint_instance_sockets draws socket circles without throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto joint = MakeJointWithOriginTriangle();

  sf::Transform t;
  t.translate({50.f, 50.f});

  steamrot::JointInstance instance{joint, t};

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_joint_instance_sockets(
          texture, instance));
}

/////////////////////////////////////////////////
/// draw_fragment_instance tests
/////////////////////////////////////////////////

TEST_CASE("draw_fragment_instance draws fragment geometry and sockets without "
          "throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto fragment = MakeFragmentWithOriginTriangle();

  sf::Transform t;
  t.translate({10.f, 10.f});
  steamrot::FragmentInstance instance{fragment, t};

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_fragment_instance(
          texture, instance, true));
}

TEST_CASE("draw_fragment_instance draws fragment but no sockets when "
          "draw_sockets is false",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto fragment = MakeFragmentWithOriginTriangle();
  sf::Transform t;
  t.translate({10.f, 10.f});
  steamrot::FragmentInstance instance{fragment, t};
  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_fragment_instance(
          texture, instance, false));

  // test the Fragment View
  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({20, 17}) == sf::Color::Green);
  // socket at local (5,5) → world (15,15) should be green when sockets not
  // drawn due to the triangle's green fill, not white.
  REQUIRE(image.getPixel({15, 15}) == sf::Color::Green);
}

TEST_CASE("draw_fragment_instance renders fragment view and sockets at "
          "transformed position when draw_sockets is true",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  texture.clear(sf::Color::Black);

  // Triangle at (0,0),(20,0),(10,20); translate by (10,10) →
  // (10,10),(30,10),(20,30). True centroid = (20, 16.67); integer pixel (20,17)
  // should be green.
  auto fragment = MakeFragmentWithOriginTriangle(sf::Color::Green);

  sf::Transform t;
  t.translate({10.f, 10.f});
  steamrot::FragmentInstance instance{fragment, t};

  steamrot::logic::render::grimoire_machina::draw_fragment_instance(
      texture, instance, true);
  texture.display();

  // test the Fragment View
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({20, 17}) == sf::Color::Green);

  // test the socket rendering
  REQUIRE(image.getPixel({15, 15}) == sf::Color::White);
}

/////////////////////////////////////////////////
/// draw_joint_instance tests
/////////////////////////////////////////////////

TEST_CASE("draw_joint_instance draws joint geometry and sockets without "
          "throwing",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto joint = MakeJointWithOriginTriangle();

  sf::Transform t;
  t.translate({10.f, 10.f});
  steamrot::JointInstance instance{joint, t};

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_joint_instance(
          texture, instance, true));
}

TEST_CASE("draw_joint_instance draws joint but no sockets when draw_sockets is "
          "false",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  auto joint = MakeJointWithOriginTriangle();
  sf::Transform t;
  t.translate({10.f, 10.f});
  steamrot::JointInstance instance{joint, t};
  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::draw_joint_instance(
          texture, instance, false));
  // test the Joint View
  texture.display();
  const sf::Image image = texture.getTexture().copyToImage();
  REQUIRE(image.getPixel({20, 17}) == sf::Color::Blue);
  // socket at local (5,0) → world (15,10) should be blue when sockets not drawn
  // due to the triangle's blue fill, not white.
  REQUIRE(image.getPixel({15, 10}) == sf::Color::Blue);
}

// TEST_CASE("draw_joint_instance renders joint view geometry at transformed "
//           "position when draw_sockets is true",
//           "[unit][render_grimoire_machina]") {
//   sf::RenderTexture texture{{100, 100}};
//   texture.clear(sf::Color::Black);
//
//   // Triangle at (0,0),(20,0),(10,20); translate by (10,10) →
//   // (10,10),(30,10),(20,30). True centroid = (20, 16.67); integer pixel
//   (20,17)
//   // should be blue.
//   auto joint = MakeJointWithOriginTriangle(sf::Color::Blue);
//
//   sf::Transform t;
//   t.translate({10.f, 10.f});
//   steamrot::JointInstance instance{joint, t};
//
//   steamrot::logic::render::grimoire_machina::draw_joint_instance(
//       texture, instance, true);
//   texture.display();
//
//   const sf::Image image = texture.getTexture().copyToImage();
//   REQUIRE(image.getPixel({20, 17}) == sf::Color::Blue);
//
//   // test the socket rendering: socket at local (5,0) → world (15,10) should
//   be
//   // white when drawn on top of the blue triangle.
//   REQUIRE(image.getPixel({15, 10}) == sf::Color::White);
// }

/////////////////////////////////////////////////
/// render_machina_form tests
/////////////////////////////////////////////////

TEST_CASE("render_machina_form draws without throwing when no scaffold is "
          "active",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};
  steamrot::GrimoireMachina grimoire_machina;
  // m_scaffold_form is null by default

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina));
}

TEST_CASE("render_machina_form draws without throwing when scaffold has placed "
          "fragments",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = MakeFragmentWithOriginTriangle();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina));
}

TEST_CASE("render_machina_form draws without throwing when scaffold has placed "
          "joints",
          "[unit][render_grimoire_machina]") {
  sf::RenderTexture texture{{100, 100}};

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_joints["joint"] = MakeJointWithOriginTriangle();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE_NOTHROW(
      steamrot::logic::render::grimoire_machina::render_machina_form(
          texture, grimoire_machina));
}
