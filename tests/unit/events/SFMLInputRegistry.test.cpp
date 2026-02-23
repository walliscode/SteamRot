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

static steamrot::SFMLInputBinding MakeKeyboardBinding(
    sf::Keyboard::Key key,
    steamrot::InputPayload::InputAction action,
    steamrot::InputPayload::InputState trigger_state) {
  using Entry = steamrot::SFMLInputEntry;
  return steamrot::SFMLInputBinding{
      action,
      trigger_state,
      {Entry{Entry::Type::Keyboard, key, sf::Mouse::Button::Left}}};
}

static steamrot::SFMLInputBinding MakeMouseBinding(
    sf::Mouse::Button button,
    steamrot::InputPayload::InputAction action,
    steamrot::InputPayload::InputState trigger_state) {
  using Entry = steamrot::SFMLInputEntry;
  return steamrot::SFMLInputBinding{
      action,
      trigger_state,
      {Entry{Entry::Type::MouseButton, sf::Keyboard::Key::Unknown, button}}};
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

  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Space,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED);
  registry.Configure({binding});

  // Press the key so it ends up in the held set
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  REQUIRE(!registry.GetHeldKeys().empty());

  // Reconfigure — held state should be cleared
  registry.Configure({binding});
  REQUIRE(registry.GetHeldKeys().empty());
}

// ---------------------------------------------------------------------------
// Keyboard — single key, PRESSED trigger
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires PRESSED action on key press",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Space,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED);
  registry.Configure({binding});

  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::PRESSED);
}

TEST_CASE("SFMLInputRegistry does not fire PRESSED action on key release",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Space,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED);
  registry.Configure({binding});

  // Press then release
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.empty());
}

// ---------------------------------------------------------------------------
// Keyboard — single key, RELEASED trigger
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires RELEASED action on key release",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Space,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::RELEASED);
  registry.Configure({binding});

  // Must press first so the binding becomes active
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::RELEASED);
}

TEST_CASE("SFMLInputRegistry does not fire RELEASED if key was not previously held",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Space,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::RELEASED);
  registry.Configure({binding});

  // Release without pressing first — binding was never active
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));

  REQUIRE(packets.empty());
}

// ---------------------------------------------------------------------------
// Mouse button bindings
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry fires PRESSED action on mouse button press",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeMouseBinding(
      sf::Mouse::Button::Left,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED);
  registry.Configure({binding});

  const auto packets = registry.ProcessSFMLEvent(
      MakeMousePressedEvent(sf::Mouse::Button::Left));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::PRESSED);
}

TEST_CASE("SFMLInputRegistry fires RELEASED action on mouse button release",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  const auto binding = MakeMouseBinding(
      sf::Mouse::Button::Left,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::RELEASED);
  registry.Configure({binding});

  registry.ProcessSFMLEvent(MakeMousePressedEvent(sf::Mouse::Button::Left));
  const auto packets = registry.ProcessSFMLEvent(
      MakeMouseReleasedEvent(sf::Mouse::Button::Left));

  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::RELEASED);
}

// ---------------------------------------------------------------------------
// AND logic (multiple required inputs)
// ---------------------------------------------------------------------------

TEST_CASE("SFMLInputRegistry AND logic: action fires only when all required "
          "inputs are held",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  using Entry = steamrot::SFMLInputEntry;

  // Binding requires Ctrl + Space
  steamrot::SFMLInputBinding binding{
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED,
      {Entry{Entry::Type::Keyboard, sf::Keyboard::Key::LControl,
             sf::Mouse::Button::Left},
       Entry{Entry::Type::Keyboard, sf::Keyboard::Key::Space,
             sf::Mouse::Button::Left}}};
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

TEST_CASE("SFMLInputRegistry AND logic: RELEASED fires when combination breaks",
          "[unit][SFMLInputRegistry]") {
  steamrot::SFMLInputRegistry registry;
  using Entry = steamrot::SFMLInputEntry;

  // Binding requires Ctrl + Space, triggers on RELEASED
  steamrot::SFMLInputBinding binding{
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::RELEASED,
      {Entry{Entry::Type::Keyboard, sf::Keyboard::Key::LControl,
             sf::Mouse::Button::Left},
       Entry{Entry::Type::Keyboard, sf::Keyboard::Key::Space,
             sf::Mouse::Button::Left}}};
  registry.Configure({binding});

  // Activate the combination
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::LControl));
  registry.ProcessSFMLEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Space));

  // Release one key — combination breaks, should fire RELEASED
  const auto packets =
      registry.ProcessSFMLEvent(MakeKeyReleasedEvent(sf::Keyboard::Key::Space));
  REQUIRE(packets.size() == 1);
  const auto &payload = std::get<steamrot::InputPayload>(packets[0].payload);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::RELEASED);
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

  const auto binding = MakeKeyboardBinding(
      sf::Keyboard::Key::Enter,
      steamrot::InputPayload::InputAction::SELECT,
      steamrot::InputPayload::InputState::PRESSED);
  handler.ConfigureInputRegistry({binding});

  REQUIRE(handler.GetWaitingRoomEventBus().empty());

  handler.ProcessInputEvent(MakeKeyPressedEvent(sf::Keyboard::Key::Enter));

  REQUIRE(handler.GetWaitingRoomEventBus().size() == 1);
  const auto &payload =
      std::get<steamrot::InputPayload>(handler.GetWaitingRoomEventBus()[0].payload);
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(payload.state == steamrot::InputPayload::InputState::PRESSED);
}

// ---------------------------------------------------------------------------
// GetDefaultSFMLInputBindings
// ---------------------------------------------------------------------------

TEST_CASE("GetDefaultSFMLInputBindings returns non-empty bindings",
          "[unit][SFMLInputBinding]") {
  const auto bindings = steamrot::GetDefaultSFMLInputBindings();
  REQUIRE(!bindings.empty());
}
