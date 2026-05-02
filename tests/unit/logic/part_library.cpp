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
#include "PartGraph.h"
#include "ViewDirection.h"
#include "descriptors_general.h"
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
      ScaffoldResult result = builder.MakeConnectedScaffold(
          {"fragment_two_sockets", "fragment_two_sockets"},
          {"joint_two_sockets"}, {{0, 1, 2, 0}, {2, 1, 1, 0}});
      lib.scaffold_scenarios.emplace(ScaffoldScenario::LinearChain,
                                     std::move(result.scaffold));
    }

    // "ring": three joint_two_sockets in a cycle
    // part_ids: [0]=joint0, [1]=joint1, [2]=joint2
    // joint0.socket[0] ↔ joint1.socket[0]
    // joint1.socket[1] ↔ joint2.socket[0]
    // joint2.socket[1] ↔ joint0.socket[1]
    {
      ScaffoldResult result = builder.MakeConnectedScaffold(
          {}, {"joint_two_sockets", "joint_two_sockets", "joint_two_sockets"},
          {{0, 0, 1, 0}, {1, 1, 2, 0}, {2, 1, 0, 1}});
      lib.scaffold_scenarios.emplace(ScaffoldScenario::Ring,
                                     std::move(result.scaffold));
    }

    // "isolated_pair": two fragment_one_sockets, fully connected to each other
    // part_ids: [0]=frag0, [1]=frag1
    // frag0.socket[0] ↔ frag1.socket[0]
    {
      ScaffoldResult result = builder.MakeConnectedScaffold(
          {"fragment_one_socket", "fragment_one_socket"}, {}, {{0, 0, 1, 0}});
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
      ScaffoldResult result = builder.MakeConnectedScaffold(
          {"fragment_one_socket", "fragment_one_socket", "fragment_one_socket"},
          {"joint_three_sockets"}, {{0, 0, 3, 0}, {1, 0, 3, 1}, {2, 0, 3, 2}});
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
PartMap
PartLibraryBuilder::MakePartMap(const std::vector<std::string> &fragment_names,
                                const std::vector<std::string> &joint_names) {
  PartMap parts;

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

    auto set_connected = [&result](uint32_t part_id, size_t socket_index,
                                   uint32_t peer_part_id,
                                   size_t peer_socket_index) {
      auto &variant = result.scaffold.parts.at(part_id);
      std::visit(
          [part_id, socket_index, peer_part_id,
           peer_socket_index](auto &instance) {
            if (socket_index >= instance.sockets.size())
              FAIL("socket_index ("
                   << socket_index << ") out of range for part " << part_id
                   << " (sockets=" << instance.sockets.size() << ")");
            instance.sockets[socket_index].state = SocketState::Connected;
            instance.sockets[socket_index].connected_to =
                SocketConnection{peer_part_id, peer_socket_index};
          },
          variant);
    };

    set_connected(id_a, spec.socket_index_a, id_b, spec.socket_index_b);
    set_connected(id_b, spec.socket_index_b, id_a, spec.socket_index_a);
  }

  return result;
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
    const steamrot::MachinaFormScaffold &scaffold =
        lib.scaffold_scenarios.at(ScaffoldScenario::LinearChain);
    steamrot::PartGraph graph = logic::descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == expected.linear_chain.size());
    for (size_t i = 0; i < expected.linear_chain.size(); ++i) {
      INFO("node index " << i);
      CHECK(descriptor(graph.nodes[i]) == expected.linear_chain[i]);
    }
  }

  {
    INFO("ScaffoldScenario::Ring");
    const steamrot::MachinaFormScaffold &scaffold =
        lib.scaffold_scenarios.at(ScaffoldScenario::Ring);
    steamrot::PartGraph graph = logic::descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == expected.ring.size());
    for (size_t i = 0; i < expected.ring.size(); ++i) {
      INFO("node index " << i);
      CHECK(descriptor(graph.nodes[i]) == expected.ring[i]);
    }
  }

  {
    INFO("ScaffoldScenario::IsolatedPair");
    const steamrot::MachinaFormScaffold &scaffold =
        lib.scaffold_scenarios.at(ScaffoldScenario::IsolatedPair);
    steamrot::PartGraph graph = logic::descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == expected.isolated_pair.size());
    for (size_t i = 0; i < expected.isolated_pair.size(); ++i) {
      INFO("node index " << i);
      CHECK(descriptor(graph.nodes[i]) == expected.isolated_pair[i]);
    }
  }

  {
    INFO("ScaffoldScenario::SimpleBranch");
    const steamrot::MachinaFormScaffold &scaffold =
        lib.scaffold_scenarios.at(ScaffoldScenario::SimpleBranch);
    steamrot::PartGraph graph = logic::descriptors::build_part_graph(scaffold);
    REQUIRE(graph.nodes.size() == expected.simple_branch.size());
    for (size_t i = 0; i < expected.simple_branch.size(); ++i) {
      INFO("node index " << i);
      CHECK(descriptor(graph.nodes[i]) == expected.simple_branch[i]);
    }
  }
}

} // namespace steamrot::tests
