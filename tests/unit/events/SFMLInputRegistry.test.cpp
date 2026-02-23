/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SFMLInputRegistry class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SFMLInputBinding.h"
#include "SFMLInputRegistry.h"
#include "EventHandler.h"
#include "EventPayload.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

using Entry     = steamrot::SFMLInputEntry;
using EntryType = steamrot::SFMLInputEntry::Type;
using TriggerOn = steamrot::SFMLInputEntry::TriggerOn;
using Action    = steamrot::InputPayload::InputAction;

static steamrot::SFMLInputBinding MakeKeyboardBinding(
    sf::Keyboard::Key key,
    Action action,
    TriggerOn trigger_on) {
  return steamrot::SFMLInputBinding{
      action,
      {Entry{EntryType::Keyboard, trigger_on, key, sf::Mouse::Button::Left}}};
}

static steamrot::SFMLInputBinding MakeMouseBinding(
    sf::Mouse::Button button,
    Action action,
    TriggerOn trigger_on) {
  return steamrot::SFMLInputBinding{
      action,
      {Entry{EntryType::MouseButton, trigger_on,
             sf::Keyboard::Key::Unknown, button}}};
}

static sf::Event MakeKeyPressedEvent(sf::Keyboard::Key key) {
  sf::Event::KeyPressed data{};
  data.code = key;
  return sf::Event{data};
}

static sf::Event MakeKeyReleasedEvent(sf::Keyboard::Key key) {
  sf::Event::KeyReleased data{};
  data.code = key;
  return sf::Event{data};
}

static sf::Event MakeMousePressedEvent(sf::Mouse::Button button) {
  sf::Event::MouseButtonPressed data{};
  data.button = button;
  return sf::Event{data};
}

static sf::Event MakeMouseReleasedEvent(sf::Mouse::Button button) {
  sf::Event::MouseButtonReleased data{};
  data.button = button;
  return sf::Event{data};
}

// ---------------------------------------------------------------------------
// Configure
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry::Configure stores bindings",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  REQUIRE(registry.GetBindings().empty());

  const auto bindings = steamrot::GetDefaultSFMLInputBindings();
  registry.Configure(bindings);

  REQUIRE(registry.GetBindings().size() == bindings.size());
}

TEST_CASE("SFMLInputRegistry::Configure resets held state",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;

  const auto binding =
      MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                          TriggerOn::Pressed);
  registry.Configure({binding});

  // Press the key so it ends up in the held set
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  REQUIRE(!registry.GetHeldKeys().empty());

  // Reconfigure — held state should be cleared
  registry.Configure({binding});
  REQUIRE(registry.GetHeldKeys().empty());
}

// ---------------------------------------------------------------------------
// Keyboard — TriggerOn::Pressed
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires action when Pressed key entry is pressed",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Pressed)});

  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == Action::SELECT);
}

TEST_CASE("SFMLInputRegistry does not re-fire while Pressed key remains held",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Pressed)});

  // Press the key (fires once)
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  // Another unrelated event — binding stays active but should not re-fire
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::A));
  REQUIRE(packets.empty());
}

TEST_CASE("SFMLInputRegistry does not fire Pressed binding on key release",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Pressed)});

  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.empty());
}

// ---------------------------------------------------------------------------
// Keyboard — TriggerOn::Released
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires action when Released key entry is released",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Released)});

  // Press then release — should fire on release
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == Action::SELECT);
}

TEST_CASE("SFMLInputRegistry Released binding does not fire on key press",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Released)});

  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.empty());
}

TEST_CASE("SFMLInputRegistry Released binding fires only on the release event",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeKeyboardBinding(sf::Keyboard::Key::Space, Action::SELECT,
                           TriggerOn::Released)});

  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  // Release fires
  auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));
  REQUIRE(packets.size() == 1);

  // Unrelated event after release — binding is no longer satisfied (Space not
  // in just-released), should NOT fire again
  const auto packets2 =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::A));
  REQUIRE(packets2.empty());
}

// ---------------------------------------------------------------------------
// Mouse button bindings
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires action on mouse button press (Pressed entry)",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeMouseBinding(sf::Mouse::Button::Left, Action::SELECT,
                        TriggerOn::Pressed)});

  const auto packets =
      registry.ProcessSFMLEvent(MakeMousePressedEvent(sf::Mouse::Button::Left));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == Action::SELECT);
}

TEST_CASE("SFMLInputRegistry fires action on mouse button release (Released entry)",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(
      {MakeMouseBinding(sf::Mouse::Button::Left, Action::SELECT,
                        TriggerOn::Released)});

  registry.ProcessSFMLEvent(MakeMousePressedEvent(sf::Mouse::Button::Left));
  const auto packets =
      registry.ProcessSFMLEvent(MakeMouseReleasedEvent(sf::Mouse::Button::Left));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == Action::SELECT);
}

// ---------------------------------------------------------------------------
// AND logic — all Pressed entries
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry AND logic: action fires only when all required "
          "Pressed entries are held",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;

  // Binding requires Ctrl + Space both pressed
  steamrot::SFMLInputBinding binding{
      Action::SELECT,
      {Entry{EntryType::Keyboard, TriggerOn::Pressed,
             sf::Keyboard::Key::LControl, sf::Mouse::Button::Left},
       Entry{EntryType::Keyboard, TriggerOn::Pressed,
             sf::Keyboard::Key::Space, sf::Mouse::Button::Left}}};
  registry.Configure({binding});

  // Press only Ctrl — should NOT fire
  auto packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::LControl));
  REQUIRE(packets.empty());

  // Press Space too — now both are held, should fire
  packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  REQUIRE(packets.size() == 1);
}

// ---------------------------------------------------------------------------
// AND logic — mixed Pressed + Released entries
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry AND logic: mixed Pressed+Released entries fire "
          "only when all conditions met simultaneously",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;

  // Binding: Ctrl held AND Space released
  steamrot::SFMLInputBinding binding{
      Action::SELECT,
      {Entry{EntryType::Keyboard, TriggerOn::Pressed,
             sf::Keyboard::Key::LControl, sf::Mouse::Button::Left},
       Entry{EntryType::Keyboard, TriggerOn::Released,
             sf::Keyboard::Key::Space, sf::Mouse::Button::Left}}};
  registry.Configure({binding});

  // Press Ctrl
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::LControl));
  // Press Space
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  // Release Space while Ctrl is held — should fire
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));
  REQUIRE(packets.size() == 1);

  // Release Ctrl — Ctrl Released entry not in binding, Space not in
  // just-released → no fire
  const auto packets2 =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::LControl));
  REQUIRE(packets2.empty());
}

TEST_CASE("SFMLInputRegistry AND logic: mixed Pressed+Released does not fire "
          "when Pressed condition not met",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;

  // Binding: Ctrl held AND Space released
  steamrot::SFMLInputBinding binding{
      Action::SELECT,
      {Entry{EntryType::Keyboard, TriggerOn::Pressed,
             sf::Keyboard::Key::LControl, sf::Mouse::Button::Left},
       Entry{EntryType::Keyboard, TriggerOn::Released,
             sf::Keyboard::Key::Space, sf::Mouse::Button::Left}}};
  registry.Configure({binding});

  // Press and release Space WITHOUT holding Ctrl — should NOT fire
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));
  REQUIRE(packets.empty());
}

// ---------------------------------------------------------------------------
// Held state tracking
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry tracks held keyboard keys correctly",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure({});

  REQUIRE(registry.GetHeldKeys().empty());

  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::A));
  REQUIRE(registry.GetHeldKeys().contains(sf::Keyboard::Key::A));

  registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::A));
  REQUIRE(registry.GetHeldKeys().empty());
}

TEST_CASE("SFMLInputRegistry tracks held mouse buttons correctly",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure({});

  REQUIRE(registry.GetHeldButtons().empty());

  registry.ProcessSFMLEvent(MakeMousePressedEvent(sf::Mouse::Button::Right));
  REQUIRE(registry.GetHeldButtons().contains(sf::Mouse::Button::Right));

  registry.ProcessSFMLEvent(MakeMouseReleasedEvent(sf::Mouse::Button::Right));
  REQUIRE(registry.GetHeldButtons().empty());
}

// ---------------------------------------------------------------------------
// Non-input events are ignored
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry ignores non-input SFML events",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  registry.Configure(steamrot::GetDefaultSFMLInputBindings());

  // A window-closed event should produce no packets
  const auto packets =
      registry.ProcessSFMLEvent(sf::Event{sf::Event::Closed{}});
  REQUIRE(packets.empty());
}

// ---------------------------------------------------------------------------
// EventHandler integration
// ---------------------------------------------------------------------------

TEST_CASE("EventHandler::ConfigureInputRegistry stores bindings in registry",
          "[unit][SFMLInputRegistry][EventHandler]") {
  steamrot::EventHandler handler;
  REQUIRE(handler.GetInputRegistry().GetBindings().empty());

  handler.ConfigureInputRegistry(steamrot::GetDefaultSFMLInputBindings());
  REQUIRE(!handler.GetInputRegistry().GetBindings().empty());
}

TEST_CASE("EventHandler::ProcessInputEvent adds InputPayload events to the "
          "waiting room bus",
          "[unit][SFMLInputRegistry][EventHandler]") {
  steamrot::EventHandler handler;

  handler.ConfigureInputRegistry(
      {MakeKeyboardBinding(sf::Keyboard::Key::Enter, Action::SELECT,
                           TriggerOn::Pressed)});

  REQUIRE(handler.GetWaitingRoomEventBus().empty());

  handler.ProcessInputEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Enter));

  REQUIRE(handler.GetWaitingRoomEventBus().size() == 1);
  const auto &payload =
      std::get<steamrot::InputPayload>(handler.GetWaitingRoomEventBus()[0].payload);
  REQUIRE(payload.action == Action::SELECT);
}

// ---------------------------------------------------------------------------
// GetDefaultSFMLInputBindings
// ---------------------------------------------------------------------------

TEST_CASE("GetDefaultSFMLInputBindings returns non-empty bindings",
          "[unit][SFMLInputBinding]") {
  const auto bindings = steamrot::GetDefaultSFMLInputBindings();
  REQUIRE(!bindings.empty());
}
