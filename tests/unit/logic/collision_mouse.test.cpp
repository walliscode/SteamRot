/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CheckMouseOver overloads, IsMouseOverBounds,
///        ClearMouseOver, and AnyMouseOver in collision::mouse
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_mouse.h"
#include "MachinaFormScaffold.h"
#include "PanelElement.h"
#include "catch2/generators/catch_generators.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CheckMouseOver GrowthPoint sets is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(105, 105);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      growth_point);
    REQUIRE(growth_point.is_mouse_over == true);
  }

  SECTION("Mouse is not over growth point") {
    steamrot::GrowthPoint growth_point;
    growth_point.origin.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      growth_point);
    REQUIRE(growth_point.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver Socket sets is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over socket") {
    steamrot::Socket socket{sf::Vector2f{100.f, 100.f}};
    socket.circle.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, socket);
    REQUIRE(socket.is_mouse_over == true);
  }

  SECTION("Mouse is not over socket") {
    steamrot::Socket socket{sf::Vector2f{100.f, 100.f}};
    socket.circle.setPosition({100.f, 100.f});
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, socket);
    REQUIRE(socket.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver FragmentInstance sets socket is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over a socket on the fragment") {
    steamrot::Fragment fragment;
    fragment.sockets = {{100.f, 100.f}};
    steamrot::FragmentInstance fragment_instance{fragment};
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      fragment_instance);
    REQUIRE(fragment_instance.sockets[0].is_mouse_over == true);
  }

  SECTION("Mouse is not over any socket on the fragment") {
    steamrot::Fragment fragment;
    fragment.sockets = {{100.f, 100.f}};
    steamrot::FragmentInstance fragment_instance{fragment};
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      fragment_instance);
    REQUIRE(fragment_instance.sockets[0].is_mouse_over == false);
  }

  SECTION("Fragment with no sockets does not crash") {
    steamrot::Fragment fragment;
    steamrot::FragmentInstance fragment_instance{fragment};
    sf::Vector2i mouse_position(100, 100);
    REQUIRE_NOTHROW(steamrot::logic::collision::mouse::CheckMouseOver(
        mouse_position, fragment_instance));
  }
}

TEST_CASE("CheckMouseOver JointInstance sets socket is_mouse_over correctly",
          "[unit][collision][mouse]") {

  SECTION("Mouse is over a socket on the joint") {
    steamrot::Joint joint;
    joint.sockets = {{100.f, 100.f}};
    steamrot::JointInstance joint_instance{joint};
    sf::Vector2i mouse_position(103, 103);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      joint_instance);
    REQUIRE(joint_instance.sockets[0].is_mouse_over == true);
  }

  SECTION("Mouse is not over any socket on the joint") {
    steamrot::Joint joint;
    joint.sockets = {{100.f, 100.f}};
    steamrot::JointInstance joint_instance{joint};
    sf::Vector2i mouse_position(200, 200);
    steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                      joint_instance);
    REQUIRE(joint_instance.sockets[0].is_mouse_over == false);
  }

  SECTION("Joint with no sockets does not crash") {
    steamrot::Joint joint;
    steamrot::JointInstance joint_instance{joint};
    sf::Vector2i mouse_position(100, 100);
    REQUIRE_NOTHROW(steamrot::logic::collision::mouse::CheckMouseOver(
        mouse_position, joint_instance));
  }
}

// ---------------------------------------------------------------------------
// UIElement collision tests
// ---------------------------------------------------------------------------

TEST_CASE("IsMouseOverBounds returns false for point outside bounds",
          "[unit][collision]") {
  sf::Vector2i mouse_position(150, 150);
  sf::FloatRect bounds({0, 0}, {100, 100});

  bool result = steamrot::logic::collision::mouse::IsMouseOverBounds(
      mouse_position, bounds);
  REQUIRE(result == false);
}

// Helper for easy construction
struct IsMouseOverTestCase {
  sf::Vector2i mouse_pos;
  sf::FloatRect bounds;
  bool expected;
};

TEST_CASE("IsMouseOverBounds returns correct results for various bounds and "
          "mouse positions",
          "[unit][collision]") {
  auto cases = GENERATE(
      IsMouseOverTestCase{{50, 50}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{0, 0}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{99, 99}, sf::FloatRect({0, 0}, {100, 100}), true},
      IsMouseOverTestCase{{100, 100}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{-1, 50}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{50, -1}, sf::FloatRect({0, 0}, {100, 100}), false},
      IsMouseOverTestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      IsMouseOverTestCase{{125, 25}, sf::FloatRect({100, 0}, {50, 50}), true},
      IsMouseOverTestCase{{149, 49}, sf::FloatRect({100, 0}, {50, 50}), true},
      IsMouseOverTestCase{{150, 50}, sf::FloatRect({100, 0}, {50, 50}), false},
      IsMouseOverTestCase{{0, 0}, sf::FloatRect({10, 10}, {30, 30}), false},
      IsMouseOverTestCase{{15, 15}, sf::FloatRect({10, 10}, {30, 30}), true},
      IsMouseOverTestCase{{40, 40}, sf::FloatRect({10, 10}, {30, 30}), false},
      IsMouseOverTestCase{{39, 39}, sf::FloatRect({10, 10}, {30, 30}), true},
      IsMouseOverTestCase{
          {200, 200}, sf::FloatRect({190, 190}, {10, 10}), false},
      IsMouseOverTestCase{
          {199, 199}, sf::FloatRect({190, 190}, {10, 10}), true},
      IsMouseOverTestCase{
          {189, 189}, sf::FloatRect({190, 190}, {10, 10}), false},
      IsMouseOverTestCase{
          {300, 400}, sf::FloatRect({250, 350}, {60, 70}), true},
      IsMouseOverTestCase{
          {310, 420}, sf::FloatRect({250, 350}, {60, 70}), false},
      IsMouseOverTestCase{
          {320, 430}, sf::FloatRect({250, 350}, {60, 70}), false});

  bool result = steamrot::logic::collision::mouse::IsMouseOverBounds(
      cases.mouse_pos, cases.bounds);
  REQUIRE(result == cases.expected);
}

TEST_CASE("CheckMouseOver UIElement (no children) toggles is_mouse_over",
          "[unit][collision]") {

  // create Panel Element and set position and size
  steamrot::PanelElement panel_element;
  panel_element.position = {0, 0};
  panel_element.size = {100, 100};

  // ensure is_mouse_over is false initially
  REQUIRE(panel_element.is_mouse_over == false);
  // check mouse position inside bounds
  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    panel_element);
  REQUIRE(panel_element.is_mouse_over == true);
  // now move mouse outside bounds
  mouse_position = sf::Vector2i(150, 150);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    panel_element);
  REQUIRE(panel_element.is_mouse_over == false);
}

TEST_CASE("CheckMouseOver UIElement toggles nested Panel Elements",
          "[unit][collision]") {
  // create parent Panel Element
  steamrot::PanelElement parent_element;
  parent_element.position = {0, 0};
  parent_element.size = {200, 200};

  // create child Panel Element
  auto child_element_to_move = std::make_unique<steamrot::PanelElement>();
  child_element_to_move->position = {50, 50};
  child_element_to_move->size = {100, 100};

  // add child to parent
  parent_element.child_elements.push_back(std::move(child_element_to_move));

  // get reference to the moved child for easy access
  auto &child_element = *static_cast<steamrot::PanelElement *>(
      parent_element.child_elements[0].get());

  // activate children so that they can receive hover
  parent_element.children_active = true;

  // ensure both are not hovered initially
  REQUIRE(parent_element.is_mouse_over == false);
  REQUIRE(child_element.is_mouse_over == false);

  // move mouse over child
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == true);
  REQUIRE(parent_element.is_mouse_over == false);

  // move mouse outside both
  mouse_position = sf::Vector2i(250, 250);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == false);

  // move mouse over parent but outside child
  mouse_position = sf::Vector2i(175, 175);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position,
                                                    parent_element);
  REQUIRE(child_element.is_mouse_over == false);
  REQUIRE(parent_element.is_mouse_over == true);
}

TEST_CASE("ClearMouseOver clears is_mouse_over on element and all children",
          "[unit][collision]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.is_mouse_over = true;

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};
  child->is_mouse_over = true;

  auto grandchild = std::make_unique<steamrot::PanelElement>();
  grandchild->position = {60, 60};
  grandchild->size = {20, 20};
  grandchild->is_mouse_over = true;

  child->child_elements.push_back(std::move(grandchild));
  parent.child_elements.push_back(std::move(child));

  // all hovered before clear
  REQUIRE(parent.is_mouse_over == true);
  REQUIRE(parent.child_elements[0]->is_mouse_over == true);
  REQUIRE(parent.child_elements[0]->child_elements[0]->is_mouse_over == true);

  steamrot::logic::collision::mouse::ClearMouseOver(parent);

  REQUIRE(parent.is_mouse_over == false);
  REQUIRE(parent.child_elements[0]->is_mouse_over == false);
  REQUIRE(parent.child_elements[0]->child_elements[0]->is_mouse_over == false);
}

TEST_CASE("AnyMouseOver returns true when any element in tree is hovered",
          "[unit][collision]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};

  parent.child_elements.push_back(std::move(child));

  SECTION("No elements hovered returns false") {
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == false);
  }

  SECTION("Root element hovered returns true") {
    parent.is_mouse_over = true;
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == true);
  }

  SECTION("Child element hovered returns true") {
    parent.child_elements[0]->is_mouse_over = true;
    REQUIRE(steamrot::logic::collision::mouse::AnyMouseOver(parent) == true);
  }
}

TEST_CASE(
    "CheckMouseOver UIElement respects priority among overlapping sibling "
    "children",
    "[unit][collision]") {

  // Parent that covers the whole test area
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};

  // Both siblings occupy the same bounds so without priority they would both
  // be candidates for hover on the same mouse position
  auto low_priority = std::make_unique<steamrot::PanelElement>();
  low_priority->position = {50, 50};
  low_priority->size = {100, 100};
  low_priority->priority = 0;

  auto high_priority = std::make_unique<steamrot::PanelElement>();
  high_priority->position = {50, 50};
  high_priority->size = {100, 100};
  high_priority->priority = 10;

  // Insert low-priority first so that insertion order alone would give it
  // hover; priority must override this
  parent.child_elements.push_back(std::move(low_priority));
  parent.child_elements.push_back(std::move(high_priority));

  // activate children so that they can receive hover
  parent.children_active = true;

  auto &low =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());
  auto &high =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[1].get());

  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Higher priority sibling receives hover") {
    REQUIRE(high.is_mouse_over == true);
  }

  SECTION("Lower priority sibling does not receive hover") {
    REQUIRE(low.is_mouse_over == false);
  }

  SECTION("Parent does not receive hover when a child is hovered") {
    REQUIRE(parent.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver UIElement selects lower priority sibling when higher "
          "priority is not under mouse",
          "[unit][collision]") {

  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {300, 300};

  // High priority child placed to the right - mouse will NOT be over it
  auto high_priority = std::make_unique<steamrot::PanelElement>();
  high_priority->position = {150, 0};
  high_priority->size = {100, 100};
  high_priority->priority = 10;

  // Low priority child on the left - mouse WILL be over it
  auto low_priority = std::make_unique<steamrot::PanelElement>();
  low_priority->position = {0, 0};
  low_priority->size = {100, 100};
  low_priority->priority = 0;

  parent.child_elements.push_back(std::move(low_priority));
  parent.child_elements.push_back(std::move(high_priority));

  // activate children so that they can receive hover
  parent.children_active = true;

  auto &low =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());
  auto &high =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[1].get());

  // Mouse is over the low-priority child only
  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  REQUIRE(high.is_mouse_over == false);
  REQUIRE(low.is_mouse_over == true);
}

TEST_CASE("CheckMouseOver UIElement with high-priority container having nested "
          "hovered child blocks low-priority sibling",
          "[unit][collision]") {

  // Simulate the dropdown-over-exit-button scenario:
  //   root
  //   ├── exit_button       (priority 0)
  //   └── dropdown_container (priority 10)
  //         └── dropdown_item (position overlaps exit_button)
  //
  // When the mouse is over the dropdown_item, exit_button must NOT be hovered.

  steamrot::PanelElement root;
  root.position = {0, 0};
  root.size = {300, 300};

  // Exit button at the same position that the dropdown item will cover
  auto exit_button = std::make_unique<steamrot::PanelElement>();
  exit_button->position = {50, 50};
  exit_button->size = {100, 100};
  exit_button->priority = 0;

  // High-priority dropdown container that overlaps the exit button area
  auto dropdown_container = std::make_unique<steamrot::PanelElement>();
  dropdown_container->position = {0, 0};
  dropdown_container->size = {200, 200};
  dropdown_container->priority = 10;
  dropdown_container->children_active = true;

  // Dropdown item nested inside the container, overlapping the exit button
  auto dropdown_item = std::make_unique<steamrot::PanelElement>();
  dropdown_item->position = {50, 50};
  dropdown_item->size = {100, 100};
  dropdown_container->child_elements.push_back(std::move(dropdown_item));

  // Insert exit_button first (insertion order would give it hover without
  // priority)
  root.child_elements.push_back(std::move(exit_button));
  root.child_elements.push_back(std::move(dropdown_container));

  // activate children so that they can receive hover
  root.children_active = true;

  auto &exit_btn =
      *static_cast<steamrot::PanelElement *>(root.child_elements[0].get());
  auto &container =
      *static_cast<steamrot::PanelElement *>(root.child_elements[1].get());
  auto &item =
      *static_cast<steamrot::PanelElement *>(container.child_elements[0].get());

  // Mouse is over the area shared by exit_button and dropdown_item
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, root);

  SECTION("Nested dropdown item receives hover") {
    REQUIRE(item.is_mouse_over == true);
  }

  SECTION("Exit button (lower-priority sibling of container) is not hovered") {
    REQUIRE(exit_btn.is_mouse_over == false);
  }

  SECTION("Dropdown container itself is not hovered (descendant is)") {
    REQUIRE(container.is_mouse_over == false);
  }

  SECTION("Root is not hovered") { REQUIRE(root.is_mouse_over == false); }
}

// ---------------------------------------------------------------------------
// children_active gating tests
// ---------------------------------------------------------------------------

TEST_CASE("CheckMouseOver UIElement does not hover inactive children",
          "[unit][collision]") {
  // Parent with children_active = false: even though the mouse is directly
  // over a child's bounds, the child must NOT receive hover (it is not
  // visible).  The parent itself should receive hover because no active child
  // claimed it.

  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.children_active = false; // children are invisible

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};
  parent.child_elements.push_back(std::move(child));

  auto &child_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());

  // Mouse directly over the child's bounds
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Inactive child does not receive hover") {
    REQUIRE(child_ref.is_mouse_over == false);
  }

  SECTION("Parent receives hover because no active child claimed it") {
    REQUIRE(parent.is_mouse_over == true);
  }
}

TEST_CASE("CheckMouseOver UIElement clears stale hover on inactive children",
          "[unit][collision]") {
  // Simulate a stale hover state: a child was hovered in a previous frame,
  // then children_active is set to false (e.g. a dropdown collapses).
  // CheckMouseOver must clear the stale is_mouse_over on the child so that
  // action processing cannot fire for an invisible element.

  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.children_active = true; // active initially

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};
  parent.child_elements.push_back(std::move(child));

  auto &child_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());

  // Frame 1: children are active; child is hovered
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);
  REQUIRE(child_ref.is_mouse_over == true);

  // Frame 2: children become inactive (dropdown collapses)
  parent.children_active = false;
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Stale hover is cleared on now-inactive child") {
    REQUIRE(child_ref.is_mouse_over == false);
  }

  SECTION("Parent is now hovered (no active child claimed it)") {
    REQUIRE(parent.is_mouse_over == true);
  }
}

TEST_CASE("CheckMouseOver UIElement respects children_active on nested "
          "grandchild containers",
          "[unit][collision]") {
  // Multi-level: root → container (children_active=true) →
  //                     list (children_active=false) → item
  //
  // Mouse is over the item's bounds. Because the list's children are inactive
  // the item must NOT be hovered. The list element itself can be hovered
  // (its own bounds contain the mouse).

  steamrot::PanelElement root;
  root.position = {0, 0};
  root.size = {400, 400};
  root.children_active = true;

  auto container = std::make_unique<steamrot::PanelElement>();
  container->position = {0, 0};
  container->size = {300, 300};
  container->children_active = true;

  auto list = std::make_unique<steamrot::PanelElement>();
  list->position = {0, 100};
  list->size = {200, 200};
  list->children_active = false; // list is collapsed

  auto item = std::make_unique<steamrot::PanelElement>();
  item->position = {0, 100}; // same area as list, but list is inactive
  item->size = {200, 50};
  list->child_elements.push_back(std::move(item));

  container->child_elements.push_back(std::move(list));
  root.child_elements.push_back(std::move(container));

  auto &container_ref =
      *static_cast<steamrot::PanelElement *>(root.child_elements[0].get());
  auto &list_ref = *static_cast<steamrot::PanelElement *>(
      container_ref.child_elements[0].get());
  auto &item_ref =
      *static_cast<steamrot::PanelElement *>(list_ref.child_elements[0].get());

  // Mouse is over item's bounds (and also the list's bounds)
  sf::Vector2i mouse_position(50, 150);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, root);

  SECTION("Inactive item does not receive hover") {
    REQUIRE(item_ref.is_mouse_over == false);
  }

  SECTION("List element itself is hovered (mouse is within its bounds)") {
    REQUIRE(list_ref.is_mouse_over == true);
  }

  SECTION("Root and container are not hovered (list claimed it)") {
    REQUIRE(container_ref.is_mouse_over == false);
    REQUIRE(root.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver UIElement with inactive high-priority sibling does "
          "not block lower-priority sibling",
          "[unit][collision]") {
  // A high-priority sibling with children_active=false should NOT prevent a
  // lower-priority sibling from being hovered, since the inactive element's
  // bounds are treated as a leaf (tested against the mouse directly).  Here
  // the mouse is NOT over the high-priority sibling, so the lower-priority
  // sibling should win.

  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {400, 400};
  parent.children_active = true;

  // High-priority sibling – positioned to the right, mouse will not be over it
  auto high_pri = std::make_unique<steamrot::PanelElement>();
  high_pri->position = {200, 0};
  high_pri->size = {100, 100};
  high_pri->priority = 10;
  high_pri->children_active = false;

  // High-priority also has a child whose bounds DO cover the mouse position –
  // because children_active=false, this child must NOT steal the hover
  auto high_pri_child = std::make_unique<steamrot::PanelElement>();
  high_pri_child->position = {0, 0};
  high_pri_child->size = {100, 100};
  high_pri->child_elements.push_back(std::move(high_pri_child));

  // Low-priority sibling – mouse IS over this one
  auto low_pri = std::make_unique<steamrot::PanelElement>();
  low_pri->position = {0, 0};
  low_pri->size = {100, 100};
  low_pri->priority = 0;
  low_pri->children_active = true;

  parent.child_elements.push_back(std::move(high_pri));
  parent.child_elements.push_back(std::move(low_pri));

  auto &high_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());
  auto &high_child_ref =
      *static_cast<steamrot::PanelElement *>(high_ref.child_elements[0].get());
  auto &low_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[1].get());

  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("High-priority inactive child does not receive hover") {
    REQUIRE(high_child_ref.is_mouse_over == false);
  }

  SECTION("High-priority element itself is not hovered (mouse not over it)") {
    REQUIRE(high_ref.is_mouse_over == false);
  }

  SECTION("Low-priority element receives hover") {
    REQUIRE(low_ref.is_mouse_over == true);
  }
}

// ---------------------------------------------------------------------------
// is_disabled gating tests
// ---------------------------------------------------------------------------

TEST_CASE("CheckMouseOver disabled UIElement never becomes hovered",
          "[unit][collision][disabled]") {
  steamrot::PanelElement panel;
  panel.position = {0, 0};
  panel.size = {100, 100};
  panel.is_disabled = true;

  // Mouse is clearly inside the element bounds
  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, panel);

  REQUIRE(panel.is_mouse_over == false);
}

TEST_CASE("CheckMouseOver disabled UIElement clears stale hover state",
          "[unit][collision][disabled]") {
  steamrot::PanelElement panel;
  panel.position = {0, 0};
  panel.size = {100, 100};
  // Simulate stale hover from a previous frame
  panel.is_mouse_over = true;
  panel.is_disabled = true;

  sf::Vector2i mouse_position(50, 50);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, panel);

  REQUIRE(panel.is_mouse_over == false);
}

TEST_CASE("CheckMouseOver disabled UIElement clears stale hover on children",
          "[unit][collision][disabled]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.children_active = true;
  parent.is_disabled = true;

  auto child = std::make_unique<steamrot::PanelElement>();
  child->position = {50, 50};
  child->size = {100, 100};
  // Simulate stale hover on the child from a previous frame
  child->is_mouse_over = true;
  parent.child_elements.push_back(std::move(child));

  auto &child_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());

  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Parent is not hovered") {
    REQUIRE(parent.is_mouse_over == false);
  }

  SECTION("Child stale hover is cleared") {
    REQUIRE(child_ref.is_mouse_over == false);
  }
}

TEST_CASE("CheckMouseOver disabled child does not receive hover even when "
          "parent is enabled",
          "[unit][collision][disabled]") {
  steamrot::PanelElement parent;
  parent.position = {0, 0};
  parent.size = {200, 200};
  parent.children_active = true;

  auto disabled_child = std::make_unique<steamrot::PanelElement>();
  disabled_child->position = {50, 50};
  disabled_child->size = {100, 100};
  disabled_child->is_disabled = true;
  parent.child_elements.push_back(std::move(disabled_child));

  auto &child_ref =
      *static_cast<steamrot::PanelElement *>(parent.child_elements[0].get());

  // Mouse directly over the disabled child's bounds
  sf::Vector2i mouse_position(75, 75);
  steamrot::logic::collision::mouse::CheckMouseOver(mouse_position, parent);

  SECTION("Disabled child does not receive hover") {
    REQUIRE(child_ref.is_mouse_over == false);
  }

  SECTION("Parent receives hover because disabled child did not claim it") {
    REQUIRE(parent.is_mouse_over == true);
  }
}
