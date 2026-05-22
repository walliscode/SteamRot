/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestPartLibrary struct, PartLibraryBuilder
///        class, and CheckNodeDescriptorForAllScenarios helper.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "part_library.h"
#include "MachinaFormScaffold.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace {

/////////////////////////////////////////////////
/// @brief Build a filled 20×20 square VertexArray (Triangles primitive).
///
/// @param x     Left edge x-coordinate.
/// @param y     Top edge y-coordinate.
/// @param color Fill color.
/// @return VertexArray with 6 vertices forming two triangles.
/////////////////////////////////////////////////
sf::VertexArray MakeFilledSquare(float x, float y, sf::Color color) {
  sf::VertexArray va(sf::PrimitiveType::Triangles, 6);
  va[0] = sf::Vertex{{x, y}, color};
  va[1] = sf::Vertex{{x + 20.f, y}, color};
  va[2] = sf::Vertex{{x + 20.f, y + 20.f}, color};
  va[3] = sf::Vertex{{x, y}, color};
  va[4] = sf::Vertex{{x + 20.f, y + 20.f}, color};
  va[5] = sf::Vertex{{x, y + 20.f}, color};
  return va;
}

/////////////////////////////////////////////////
/// @brief Build a green origin-aligned triangle VertexArray.
///
/// Vertices at (0, 0), (20, 0), (10, 20).
/////////////////////////////////////////////////
sf::VertexArray MakeGreenOriginTriangle() {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, sf::Color::Green});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, sf::Color::Green});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, sf::Color::Green});
  return va;
}

/////////////////////////////////////////////////
/// @brief Build a blue origin-aligned triangle VertexArray.
///
/// Vertices at (0, 0), (20, 0), (10, 20).
/////////////////////////////////////////////////
sf::VertexArray MakeBlueOriginTriangle() {
  sf::VertexArray va(sf::PrimitiveType::Triangles);
  va.append(sf::Vertex{sf::Vector2f{0.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{20.f, 0.f}, sf::Color::Blue});
  va.append(sf::Vertex{sf::Vector2f{10.f, 20.f}, sf::Color::Blue});
  return va;
}

} // namespace

namespace steamrot::tests {

/////////////////////////////////////////////////
ConnectionEndpointSpec FragmentSocket(size_t fragment_index, uint32_t socket_id) {
  return ConnectionEndpointSpec{
      .part_kind = PartSlotKind::Fragment,
      .part_index = fragment_index,
      .socket_id = socket_id,
  };
}

/////////////////////////////////////////////////
ConnectionEndpointSpec JointSocket(size_t joint_index, uint32_t socket_id) {
  return ConnectionEndpointSpec{
      .part_kind = PartSlotKind::Joint,
      .part_index = joint_index,
      .socket_id = socket_id,
  };
}

/////////////////////////////////////////////////
EndpointConnectionSpec Connect(ConnectionEndpointSpec endpoint_a,
                               ConnectionEndpointSpec endpoint_b) {
  return EndpointConnectionSpec{
      .endpoint_a = endpoint_a,
      .endpoint_b = endpoint_b,
  };
}

/////////////////////////////////////////////////
TestPartLibrary TestPartLibrary::Create() {
  TestPartLibrary lib;

  // ── Fragments ────────────────────────────────────────────────────────── //

  // "fragment_no_socket": green triangle in Front view, no sockets
  {
    Fragment f;
    f.name = "fragment_no_socket";
    f.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                         MakeGreenOriginTriangle());
    lib.fragments.emplace("fragment_no_socket", std::move(f));
  }

  // "fragment_one_socket": green origin triangle + one socket at (5, 5)
  {
    Fragment f;
    f.name = "fragment_one_socket";
    f.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                         MakeGreenOriginTriangle());
    f.sockets = {{5.f, 5.f}};
    lib.fragments.emplace("fragment_one_socket", std::move(f));
  }

  // "fragment_two_sockets": white 20×20 square + sockets at (0, 10) and (20,
  // 10)
  {
    Fragment f;
    f.name = "fragment_two_sockets";
    f.positioning_views.insert_or_assign(
        steamrot::ViewDirection::Front,
        MakeFilledSquare(0.f, 0.f, sf::Color::White));
    f.sockets = {{0.f, 10.f}, {20.f, 10.f}};
    lib.fragments.emplace("fragment_two_sockets", std::move(f));
  }

  // "fragment_three_sockets": white 20×20 square + sockets at (0,10), (10,10)
  // and (20,10)
  {
    Fragment f;
    f.name = "fragment_three_sockets";
    f.positioning_views.insert_or_assign(
        steamrot::ViewDirection::Front,
        MakeFilledSquare(0.f, 0.f, sf::Color::White));
    f.sockets = {{0.f, 10.f}, {10.f, 10.f}, {20.f, 10.f}};
    lib.fragments.emplace("fragment_three_sockets", std::move(f));
  }

  // ── Joints ───────────────────────────────────────────────────────────── //

  // "joint_no_socket": blue triangle in Front view, no sockets configured
  {
    Joint j;
    j.name = "joint_no_socket";
    j.positioning_views.insert_or_assign(steamrot::ViewDirection::Front,
                                         MakeBlueOriginTriangle());
    lib.joints.emplace("joint_no_socket", std::move(j));
  }

  // "joint_one_socket": 1 socket at radius 10, full rotation arc
  {
    Joint j;
    j.name = "joint_one_socket";
    j.socket_config.socket_count = 1;
    j.socket_config.radius = 10.f;
    j.socket_config.rotation_arc_min = 0.f;
    j.socket_config.rotation_arc_max = 360.f;
    lib.joints.emplace("joint_one_socket", std::move(j));
  }

  // "joint_two_sockets": 2 sockets at radius 15, full rotation arc
  {
    Joint j;
    j.name = "joint_two_sockets";
    j.socket_config.socket_count = 2;
    j.socket_config.radius = 15.f;
    j.socket_config.rotation_arc_min = 0.f;
    j.socket_config.rotation_arc_max = 360.f;
    lib.joints.emplace("joint_two_sockets", std::move(j));
  }

  // "joint_three_sockets": 3 sockets at radius 15, full rotation arc
  {
    Joint j;
    j.name = "joint_three_sockets";
    j.socket_config.socket_count = 3;
    j.socket_config.radius = 15.f;
    j.socket_config.rotation_arc_min = 0.f;
    j.socket_config.rotation_arc_max = 360.f;
    lib.joints.emplace("joint_three_sockets", std::move(j));
  }

  // ── Scaffold scenarios ────────────────────────────────────────────────── //

  {
    PartLibraryBuilder builder{lib};

    // "linear_chain": fragment_two_sockets ─ joint_two_sockets ─
    // fragment_two_sockets part_ids: [0]=frag0, [1]=frag1, [2]=joint0
    // frag0.socket[1] ↔ joint0.socket[0], joint0.socket[1] ↔ frag1.socket[0]
    {
      ScaffoldResult result = builder.MakeConnectedScaffoldWithEndpoints(
          {"fragment_two_sockets", "fragment_two_sockets"},
          {"joint_two_sockets"},
          {
              Connect(FragmentSocket(0, 1), JointSocket(0, 0)),
              Connect(JointSocket(0, 1), FragmentSocket(1, 0)),
          });
      lib.scaffold_scenarios.emplace(ScaffoldScenario::LinearChain,
                                     std::move(result.scaffold));
    }

    // "ring": three joint_two_sockets in a cycle
    // part_ids: [0]=joint0, [1]=joint1, [2]=joint2
    // joint0.socket[0] ↔ joint1.socket[0]
    // joint1.socket[1] ↔ joint2.socket[0]
    // joint2.socket[1] ↔ joint0.socket[1]
    {
      ScaffoldResult result = builder.MakeConnectedScaffoldWithEndpoints(
          {},
          {"joint_two_sockets", "joint_two_sockets", "joint_two_sockets"},
          {
              Connect(JointSocket(0, 0), JointSocket(1, 0)),
              Connect(JointSocket(1, 1), JointSocket(2, 0)),
              Connect(JointSocket(2, 1), JointSocket(0, 1)),
          });
      lib.scaffold_scenarios.emplace(ScaffoldScenario::Ring,
                                     std::move(result.scaffold));
    }

    // "isolated_pair": two fragment_one_sockets, fully connected to each other
    // part_ids: [0]=frag0, [1]=frag1
    // frag0.socket[0] ↔ frag1.socket[0]
    {
      ScaffoldResult result = builder.MakeConnectedScaffoldWithEndpoints(
          {"fragment_one_socket", "fragment_one_socket"},
          {},
          {
              Connect(FragmentSocket(0, 0), FragmentSocket(1, 0)),
          });
      lib.scaffold_scenarios.emplace(ScaffoldScenario::IsolatedPair,
                                     std::move(result.scaffold));
    }

    // "simple_branch": one Joint connected to one socket each of three
    // Fragments, creating a branching point
    // part_ids:  [0]=frag0, [1]=frag1, [2]=frag2, [3]=joint0,
    //           frag0.socket[0] ↔ joint0.socket[0]
    //           frag1.socket[0] ↔ joint0.socket[1]
    //           frag2.socket[0] ↔ joint0.socket[2]
    //
    {
      ScaffoldResult result = builder.MakeConnectedScaffoldWithEndpoints(
          {"fragment_one_socket", "fragment_one_socket", "fragment_one_socket"},
          {"joint_three_sockets"},
          {
              Connect(FragmentSocket(0, 0), JointSocket(0, 0)),
              Connect(FragmentSocket(1, 0), JointSocket(0, 1)),
              Connect(FragmentSocket(2, 0), JointSocket(0, 2)),
          });
      lib.scaffold_scenarios.emplace(ScaffoldScenario::SimpleBranch,
                                     std::move(result.scaffold));
    }
  }

  return lib;
}

/////////////////////////////////////////////////
PartLibraryBuilder::PartLibraryBuilder(TestPartLibrary &library)
    : m_library{library} {}

/////////////////////////////////////////////////
FragmentInstance
PartLibraryBuilder::MakeFragmentInstance(const std::string &name,
                                         sf::Transform initial_transform) {
  auto it = m_library.fragments.find(name);
  if (it == m_library.fragments.end())
    FAIL("Fragment with name '" << name << "' not found in library");

  FragmentInstance instance{&it->second, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
JointInstance
PartLibraryBuilder::MakeJointInstance(const std::string &name,
                                      sf::Transform initial_transform) {
  auto it = m_library.joints.find(name);
  if (it == m_library.joints.end())
    FAIL("Joint with name '" << name << "' not found in library");

  JointInstance instance{&it->second, initial_transform};
  instance.id = m_next_id++;
  return instance;
}

/////////////////////////////////////////////////
PartGraph
PartLibraryBuilder::MakePartGraph(const std::vector<std::string> &fragment_names,
                                 const std::vector<std::string> &joint_names) {
  m_next_id = 0;
  PartGraph parts;

  for (const auto &name : fragment_names) {
    FragmentInstance instance = MakeFragmentInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  for (const auto &name : joint_names) {
    JointInstance instance = MakeJointInstance(name);
    parts.emplace(instance.id, std::move(instance));
  }

  return parts;
}

/////////////////////////////////////////////////
MachinaFormScaffold PartLibraryBuilder::MakeScaffoldWithParts(
    const std::vector<std::string> &fragment_names,
    const std::vector<std::string> &joint_names) {
  return BuildScaffoldWithIds(fragment_names, joint_names).scaffold;
}

/////////////////////////////////////////////////
ScaffoldResult PartLibraryBuilder::BuildScaffoldWithIds(
    const std::vector<std::string> &fragment_names,
    const std::vector<std::string> &joint_names) {
  m_next_id = 0;
  ScaffoldResult result;
  result.part_ids.reserve(fragment_names.size() + joint_names.size());

  for (const auto &name : fragment_names) {
    FragmentInstance instance = MakeFragmentInstance(name);
    result.part_ids.push_back(instance.id);
    result.scaffold.parts.emplace(instance.id, std::move(instance));
  }

  for (const auto &name : joint_names) {
    JointInstance instance = MakeJointInstance(name);
    result.part_ids.push_back(instance.id);
    result.scaffold.parts.emplace(instance.id, std::move(instance));
  }

  result.scaffold.next_id = m_next_id;
  return result;
}

/////////////////////////////////////////////////
ScaffoldResult PartLibraryBuilder::MakeConnectedScaffold(
    const std::vector<std::string> &fragment_names,
    const std::vector<std::string> &joint_names,
    const std::vector<ConnectionSpec> &connections) {
  ScaffoldResult result = BuildScaffoldWithIds(fragment_names, joint_names);

  for (const auto &spec : connections) {
    if (spec.part_index_a >= result.part_ids.size())
      FAIL("ConnectionSpec.part_index_a ("
           << spec.part_index_a
           << ") out of range (size=" << result.part_ids.size() << ")");
    if (spec.part_index_b >= result.part_ids.size())
      FAIL("ConnectionSpec.part_index_b ("
           << spec.part_index_b
           << ") out of range (size=" << result.part_ids.size() << ")");

    const uint32_t id_a = result.part_ids[spec.part_index_a];
    const uint32_t id_b = result.part_ids[spec.part_index_b];

    auto set_connected = [&result](uint32_t part_id, uint32_t socket_id,
                                   uint32_t peer_part_id,
                                   uint32_t peer_socket_id) {
      auto &variant = result.scaffold.parts.at(part_id);
      std::visit(
          [part_id, socket_id, peer_part_id,
           peer_socket_id](auto &instance) {
            if (!instance.sockets.count(socket_id))
              FAIL("socket_id ("
                   << socket_id << ") not found for part " << part_id
                   << " (sockets=" << instance.sockets.size() << ")");
            instance.sockets.at(socket_id).state = SocketState::Connected;
            instance.sockets.at(socket_id).connected_to =
                SocketConnection{peer_part_id, peer_socket_id};
            ++instance.connection_count;
          },
          variant);
    };

    set_connected(id_a, spec.socket_id_a, id_b, spec.socket_id_b);
    set_connected(id_b, spec.socket_id_b, id_a, spec.socket_id_a);
  }

  return result;
}

/////////////////////////////////////////////////
ScaffoldResult PartLibraryBuilder::MakeConnectedScaffoldWithEndpoints(
    const std::vector<std::string> &fragment_names,
    const std::vector<std::string> &joint_names,
    const std::vector<EndpointConnectionSpec> &connections) {
  std::vector<ConnectionSpec> translated_connections;
  translated_connections.reserve(connections.size());

  auto to_insertion_order_index =
      [&fragment_names, &joint_names](const ConnectionEndpointSpec &endpoint)
      -> size_t {
    if (endpoint.part_kind == PartSlotKind::Fragment) {
      if (endpoint.part_index >= fragment_names.size())
        FAIL("Fragment endpoint index (" << endpoint.part_index
                                         << ") out of range (size="
                                         << fragment_names.size() << ")");
      return endpoint.part_index;
    }

    if (endpoint.part_index >= joint_names.size())
      FAIL("Joint endpoint index (" << endpoint.part_index
                                    << ") out of range (size="
                                    << joint_names.size() << ")");
    return fragment_names.size() + endpoint.part_index;
  };

  for (const auto &connection : connections) {
    translated_connections.push_back(ConnectionSpec{
        .part_index_a = to_insertion_order_index(connection.endpoint_a),
        .socket_id_a = connection.endpoint_a.socket_id,
        .part_index_b = to_insertion_order_index(connection.endpoint_b),
        .socket_id_b = connection.endpoint_b.socket_id,
    });
  }

  return MakeConnectedScaffold(fragment_names, joint_names, translated_connections);
}

/////////////////////////////////////////////////
const MachinaFormScaffold &
PartLibraryBuilder::GetScenarioForAnalysis(ScaffoldScenario scenario) const {
  auto it = m_library.scaffold_scenarios.find(scenario);
  if (it == m_library.scaffold_scenarios.end())
    FAIL("ScaffoldScenario '" << static_cast<int>(scenario)
                              << "' not found in library");
  return it->second;
}

/////////////////////////////////////////////////
MachinaFormScaffold &
PartLibraryBuilder::GetScenarioForModification(ScaffoldScenario scenario) {
  auto it = m_library.scaffold_scenarios.find(scenario);
  if (it == m_library.scaffold_scenarios.end())
    FAIL("ScaffoldScenario '" << static_cast<int>(scenario)
                              << "' not found in library");
  return it->second;
}

/////////////////////////////////////////////////
void CheckNodeDescriptorForAllScenarios(
    const logic::descriptors::NodeDescriptor &descriptor,
    const ScaffoldScenarioExpectations &expected, const TestPartLibrary &lib) {

  {
    INFO("ScaffoldScenario::LinearChain");
    const steamrot::PartGraph &parts =
        lib.scaffold_scenarios.at(ScaffoldScenario::LinearChain).parts;
    REQUIRE(parts.size() == expected.linear_chain.size());
    size_t i = 0;
    for (const auto &[id, variant] : parts) {
      INFO("node index " << i);
      CHECK(descriptor(parts, id) == expected.linear_chain[i]);
      ++i;
    }
  }

  {
    INFO("ScaffoldScenario::Ring");
    const steamrot::PartGraph &parts =
        lib.scaffold_scenarios.at(ScaffoldScenario::Ring).parts;
    REQUIRE(parts.size() == expected.ring.size());
    size_t i = 0;
    for (const auto &[id, variant] : parts) {
      INFO("node index " << i);
      CHECK(descriptor(parts, id) == expected.ring[i]);
      ++i;
    }
  }

  {
    INFO("ScaffoldScenario::IsolatedPair");
    const steamrot::PartGraph &parts =
        lib.scaffold_scenarios.at(ScaffoldScenario::IsolatedPair).parts;
    REQUIRE(parts.size() == expected.isolated_pair.size());
    size_t i = 0;
    for (const auto &[id, variant] : parts) {
      INFO("node index " << i);
      CHECK(descriptor(parts, id) == expected.isolated_pair[i]);
      ++i;
    }
  }

  {
    INFO("ScaffoldScenario::SimpleBranch");
    const steamrot::PartGraph &parts =
        lib.scaffold_scenarios.at(ScaffoldScenario::SimpleBranch).parts;
    REQUIRE(parts.size() == expected.simple_branch.size());
    size_t i = 0;
    for (const auto &[id, variant] : parts) {
      INFO("node index " << i);
      CHECK(descriptor(parts, id) == expected.simple_branch[i]);
      ++i;
    }
  }
}

/////////////////////////////////////////////////
TEST_CASE("PartLibraryBuilder resets IDs between scaffold and graph builds",
          "[unit][part_library]") {
  TestPartLibrary lib = TestPartLibrary::Create();
  PartLibraryBuilder builder{lib};

  SECTION("MakeScaffoldWithParts starts IDs at zero for each call") {
    MachinaFormScaffold scaffold_a = builder.MakeScaffoldWithParts(
        {"fragment_one_socket"}, {"joint_one_socket"});
    MachinaFormScaffold scaffold_b = builder.MakeScaffoldWithParts(
        {"fragment_one_socket"}, {"joint_one_socket"});

    REQUIRE(scaffold_a.parts.contains(0));
    REQUIRE(scaffold_b.parts.contains(0));
    REQUIRE(scaffold_a.next_id == 2u);
    REQUIRE(scaffold_b.next_id == 2u);
  }

  SECTION("MakeConnectedScaffold starts IDs at zero for each call") {
    ScaffoldResult result_a = builder.MakeConnectedScaffold(
        {"fragment_one_socket"}, {"joint_one_socket"}, {{0, 0, 1, 0}});
    ScaffoldResult result_b = builder.MakeConnectedScaffold(
        {"fragment_one_socket"}, {"joint_one_socket"}, {{0, 0, 1, 0}});

    REQUIRE(result_a.part_ids[0] == 0u);
    REQUIRE(result_b.part_ids[0] == 0u);
    REQUIRE(result_a.scaffold.next_id == 2u);
    REQUIRE(result_b.scaffold.next_id == 2u);
  }

  SECTION("MakePartGraph starts IDs at zero for each call") {
    PartGraph graph_a =
        builder.MakePartGraph({"fragment_one_socket"}, {"joint_one_socket"});
    PartGraph graph_b =
        builder.MakePartGraph({"fragment_one_socket"}, {"joint_one_socket"});

    REQUIRE(graph_a.contains(0));
    REQUIRE(graph_b.contains(0));
    REQUIRE(graph_a.contains(1));
    REQUIRE(graph_b.contains(1));
  }
}

/////////////////////////////////////////////////
TEST_CASE("PartLibraryBuilder endpoint helpers improve custom scaffold wiring",
          "[unit][part_library]") {
  TestPartLibrary lib = TestPartLibrary::Create();
  PartLibraryBuilder builder{lib};

  SECTION("MakeConnectedScaffoldWithEndpoints maps fragment and joint indices") {
    ScaffoldResult result = builder.MakeConnectedScaffoldWithEndpoints(
        {"fragment_two_sockets", "fragment_two_sockets"},
        {"joint_two_sockets"},
        {
            Connect(FragmentSocket(0, 1), JointSocket(0, 0)),
            Connect(JointSocket(0, 1), FragmentSocket(1, 0)),
        });

    REQUIRE(result.part_ids.size() == 3u);
    REQUIRE(result.part_ids[0] == 0u);
    REQUIRE(result.part_ids[1] == 1u);
    REQUIRE(result.part_ids[2] == 2u);

    auto check_endpoint = [&result](uint32_t part_id, uint32_t socket_id,
                                    uint32_t peer_part_id,
                                    uint32_t peer_socket_id) {
      std::visit(
          [socket_id, peer_part_id, peer_socket_id](const auto &instance) {
            REQUIRE(instance.sockets.count(socket_id) == 1u);
            const SocketData &socket = instance.sockets.at(socket_id);
            REQUIRE(socket.state == SocketState::Connected);
            REQUIRE(socket.connected_to.has_value());
            CHECK(socket.connected_to->peer_part_id == peer_part_id);
            CHECK(socket.connected_to->peer_socket_id == peer_socket_id);
          },
          result.scaffold.parts.at(part_id));
    };

    check_endpoint(0, 1, 2, 0);
    check_endpoint(2, 0, 0, 1);
    check_endpoint(2, 1, 1, 0);
    check_endpoint(1, 0, 2, 1);
  }

  SECTION("MakeConnectedScaffoldWithEndpoints fails for bad fragment indices") {
    REQUIRE_THROWS(builder.MakeConnectedScaffoldWithEndpoints(
        {"fragment_one_socket"},
        {"joint_one_socket"},
        {
            Connect(FragmentSocket(1, 0), JointSocket(0, 0)),
        }));
  }

  SECTION("MakeConnectedScaffoldWithEndpoints fails for bad joint indices") {
    REQUIRE_THROWS(builder.MakeConnectedScaffoldWithEndpoints(
        {"fragment_one_socket"},
        {"joint_one_socket"},
        {
            Connect(FragmentSocket(0, 0), JointSocket(1, 0)),
        }));
  }
}

} // namespace steamrot::tests
