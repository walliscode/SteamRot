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

///////////////////////////////////////////////
/// @brief Build named part declarations from plain catalog names.
///////////////////////////////////////////////
std::vector<steamrot::tests::NamedPartSpec>
MakeNamedSpecs(const std::vector<std::string> &part_names,
               const std::string &alias_prefix) {
  std::vector<steamrot::tests::NamedPartSpec> specs;
  specs.reserve(part_names.size());
  for (size_t i = 0; i < part_names.size(); ++i) {
    specs.push_back(
        steamrot::tests::NamedPartSpec{alias_prefix + std::to_string(i),
                                       part_names[i]});
  }
  return specs;
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
          {{"frag0", "fragment_two_sockets"}, {"frag1", "fragment_two_sockets"}},
          {{"joint0", "joint_two_sockets"}},
          {{"frag0", 1, "joint0", 0}, {"joint0", 1, "frag1", 0}});
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
          {},
          {{"joint0", "joint_two_sockets"},
           {"joint1", "joint_two_sockets"},
           {"joint2", "joint_two_sockets"}},
          {{"joint0", 0, "joint1", 0},
           {"joint1", 1, "joint2", 0},
           {"joint2", 1, "joint0", 1}});
      lib.scaffold_scenarios.emplace(ScaffoldScenario::Ring,
                                     std::move(result.scaffold));
    }

    // "isolated_pair": two fragment_one_sockets, fully connected to each other
    // part_ids: [0]=frag0, [1]=frag1
    // frag0.socket[0] ↔ frag1.socket[0]
    {
      ScaffoldResult result = builder.MakeConnectedScaffold(
          {{"frag0", "fragment_one_socket"}, {"frag1", "fragment_one_socket"}},
          {}, {{"frag0", 0, "frag1", 0}});
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
          {{"frag0", "fragment_one_socket"},
           {"frag1", "fragment_one_socket"},
           {"frag2", "fragment_one_socket"}},
          {{"joint0", "joint_three_sockets"}},
          {{"frag0", 0, "joint0", 0},
           {"frag1", 0, "joint0", 1},
           {"frag2", 0, "joint0", 2}});
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
  return BuildScaffoldWithIds(MakeNamedSpecs(fragment_names, "fragment_"),
                              MakeNamedSpecs(joint_names, "joint_"))
      .scaffold;
}

/////////////////////////////////////////////////
ScaffoldResult PartLibraryBuilder::BuildScaffoldWithIds(
    const std::vector<NamedPartSpec> &fragment_specs,
    const std::vector<NamedPartSpec> &joint_specs) {
  m_next_id = 0;
  ScaffoldResult result;
  const size_t part_count = fragment_specs.size() + joint_specs.size();
  result.part_ids.reserve(part_count);
  result.ordered_aliases.reserve(part_count);

  auto register_alias = [&result](const std::string &alias, uint32_t id) {
    if (alias.empty())
      FAIL("Part alias cannot be empty");
    if (result.alias_to_id.contains(alias))
      FAIL("Duplicate part alias '" << alias << "'");
    result.alias_to_id.emplace(alias, id);
    result.id_to_alias.emplace(id, alias);
    result.ordered_aliases.push_back(alias);
    result.part_ids.push_back(id);
  };

  for (const auto &spec : fragment_specs) {
    FragmentInstance instance = MakeFragmentInstance(spec.part_name);
    register_alias(spec.alias, instance.id);
    result.scaffold.parts.emplace(instance.id, std::move(instance));
  }

  for (const auto &spec : joint_specs) {
    JointInstance instance = MakeJointInstance(spec.part_name);
    register_alias(spec.alias, instance.id);
    result.scaffold.parts.emplace(instance.id, std::move(instance));
  }

  result.scaffold.next_id = m_next_id;
  return result;
}

/////////////////////////////////////////////////
ScaffoldResult PartLibraryBuilder::MakeConnectedScaffold(
    const std::vector<NamedPartSpec> &fragment_specs,
    const std::vector<NamedPartSpec> &joint_specs,
    const std::vector<ConnectionSpec> &connections) {
  ScaffoldResult result = BuildScaffoldWithIds(fragment_specs, joint_specs);

  for (const auto &spec : connections) {
    if (spec.from_alias.empty() || spec.to_alias.empty())
      FAIL("ConnectionSpec alias fields cannot be empty");
    if (spec.from_alias == spec.to_alias)
      FAIL("ConnectionSpec self-connection is not allowed for alias '"
           << spec.from_alias << "'");

    auto alias_to_id = [&result](const std::string &alias) -> uint32_t {
      auto it = result.alias_to_id.find(alias);
      if (it == result.alias_to_id.end())
        FAIL("ConnectionSpec alias '" << alias << "' not found in scaffold");
      return it->second;
    };

    const uint32_t id_a = alias_to_id(spec.from_alias);
    const uint32_t id_b = alias_to_id(spec.to_alias);

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
        {{"frag0", "fragment_one_socket"}}, {{"joint0", "joint_one_socket"}},
        {{"frag0", 0, "joint0", 0}});
    ScaffoldResult result_b = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_one_socket"}}, {{"joint0", "joint_one_socket"}},
        {{"frag0", 0, "joint0", 0}});

    REQUIRE(result_a.alias_to_id.at("frag0") == 0u);
    REQUIRE(result_b.alias_to_id.at("frag0") == 0u);
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

///////////////////////////////////////////////
TEST_CASE("PartLibraryBuilder validates alias-based scaffold specs",
          "[unit][part_library]") {
  TestPartLibrary lib = TestPartLibrary::Create();
  PartLibraryBuilder builder{lib};

  SECTION("Build output exposes alias mappings") {
    const ScaffoldResult result = builder.MakeConnectedScaffold(
        {{"frag0", "fragment_one_socket"}, {"frag1", "fragment_one_socket"}},
        {{"joint0", "joint_two_sockets"}},
        {{"frag0", 0, "joint0", 0}, {"joint0", 1, "frag1", 0}});

    REQUIRE(result.ordered_aliases ==
            std::vector<std::string>{"frag0", "frag1", "joint0"});
    REQUIRE(result.alias_to_id.at("frag0") == 0u);
    REQUIRE(result.alias_to_id.at("frag1") == 1u);
    REQUIRE(result.alias_to_id.at("joint0") == 2u);
    REQUIRE(result.id_to_alias.at(0u) == "frag0");
    REQUIRE(result.id_to_alias.at(1u) == "frag1");
    REQUIRE(result.id_to_alias.at(2u) == "joint0");
  }

  SECTION("Duplicate aliases fail fast") {
    REQUIRE_THROWS(builder.MakeConnectedScaffold(
        {{"dup", "fragment_one_socket"}, {"dup", "fragment_one_socket"}},
        {{"joint0", "joint_one_socket"}}, {}));
  }

  SECTION("Unknown aliases in connections fail fast") {
    REQUIRE_THROWS(builder.MakeConnectedScaffold(
        {{"frag0", "fragment_one_socket"}}, {{"joint0", "joint_one_socket"}},
        {{"frag0", 0, "missing", 0}}));
  }

  SECTION("Self-connections fail fast") {
    REQUIRE_THROWS(builder.MakeConnectedScaffold(
        {{"frag0", "fragment_two_sockets"}}, {},
        {{"frag0", 0, "frag0", 1}}));
  }
}

} // namespace steamrot::tests
