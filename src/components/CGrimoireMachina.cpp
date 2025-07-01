/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CGrimoireMachina component
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "containers.h"
#include "fragments_generated.h"
#include "log_handler.h"
#include <SFML/Graphics/Color.hpp>
namespace steamrot {

/////////////////////////////////////////////////
const size_t CGrimoireMachina::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CGrimoireMachina, components::containers::ComponentRegister>;
  return index;
}

/////////////////////////////////////////////////
const std::string &CGrimoireMachina::Name() {
  static const std::string name = "CGrimoireMachina";
  return name;
}
/////////////////////////////////////////////////
void CGrimoireMachina::Configure(const GrimoireMachina *grimoire_data) {

  // Set component level data
  m_active = true;

  // Load all fragments from the provided data
  for (const auto *fragment_data : *grimoire_data->fragments()) {
    if (fragment_data) {
      // Configure fragments with the provided data
      ConfigureFragment(fragment_data);
    }
  }
}

/////////////////////////////////////////////////
void CGrimoireMachina::ConfigureFragment(const FragmentData *fragment_data) {
  // Guard statement to ensure data is not null
  if (!fragment_data) {
    log_handler::ProcessLog(spdlog::level::info, log_handler::LogCode::kNoCode,
                            "Configuration data is null for CGrimoireMachina");
    return;
  }
  Fragment new_fragment;

  // deal with sockets
  for (const auto *socket : *fragment_data->socket_data()->vertices()) {
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(socket->x(), socket->y());
    new_fragment.m_sockets.append(vertex);
  }

  // deal with render overlay
  // color is constant across all vertices in the render overlay
  sf::Color overlay_color(fragment_data->render_overlay_data()->color()->r(),
                          fragment_data->render_overlay_data()->color()->g(),
                          fragment_data->render_overlay_data()->color()->b(),
                          fragment_data->render_overlay_data()->color()->a());

  for (const auto *render_vertex :
       *fragment_data->render_overlay_data()->vertices()) {
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(render_vertex->x(), render_vertex->y());
    vertex.color = overlay_color;
    new_fragment.m_render_overlay.append(vertex);
  }

  // add to fragments map by name
  m_all_fragments.emplace(fragment_data->name()->str(), new_fragment);
}
} // namespace steamrot
