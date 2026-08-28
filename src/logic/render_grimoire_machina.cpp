/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions related to the rendering of the
/// grimoire machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "GrimoireMachina.h"
#include "MachinaFormScaffold.h"
#include "positioning_grimoire_machina.h"
#include "render_text.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
void render_machina_form(sf::RenderTexture &texture,
                         GrimoireMachina &grimoire_machina,
                         const sf::Font &font) {
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

  if (!scaffold) {
    draw_no_machina_form_indicator(texture);
    return;
  }

  // draw the PartGraph
  for (auto &[id, part] : scaffold->parts) {

    std::visit(
        [&](const auto &part_instance) {
          part_instance.DrawInstance(texture, scaffold->are_sockets_visible);
        },
        part);
  }

  // pass the status box to the pick_and_draw_status_box function
  pick_and_draw_status_box(*scaffold, font, texture);
}

/////////////////////////////////////////////////
void draw_no_machina_form_indicator(sf::RenderTexture &texture) {
  static constexpr float k_half_size = 100.f;
  static constexpr float k_outline_thickness = 3.f;

  sf::RectangleShape box({k_half_size * 2.f, k_half_size * 2.f});
  box.setOrigin({k_half_size, k_half_size});
  box.setPosition({0.f, 0.f});
  box.setFillColor(sf::Color::Transparent);
  box.setOutlineColor(sf::Color::Red);
  box.setOutlineThickness(k_outline_thickness);

  texture.draw(box);
}

/////////////////////////////////////////////////
void draw_empty_active_machina_form(sf::RenderTexture &texture,
                                    MachinaForm &form) {}

/////////////////////////////////////////////////
void draw_status_box(sf::FloatRect box, sf::Color color,
                     const std::string &text, const sf::Font &font,
                     sf::RenderTexture &texture) {

  // set the thickness for the border
  static const float thickness{3.f};

  // create box to draw and populate variables
  sf::RectangleShape border_box{box.size};
  border_box.setPosition(box.position);
  border_box.setOutlineThickness(-thickness);
  border_box.setOutlineColor(color);
  border_box.setFillColor(sf::Color::Transparent);

  // draw to texture
  texture.draw(border_box);

  // pull out some unit of width from the box to use as increments
  const float unit_width = box.size.x * 0.1f;
  const float unit_height = box.size.y * 0.08f;

  // create a 6 point convex shape to draw the trapezium shape above the box
  sf::ConvexShape trapezium;
  trapezium.setPointCount(4);
  trapezium.setPoint(0, {box.position});
  trapezium.setPoint(
      1, {box.position.x + unit_width, box.position.y - unit_height});
  trapezium.setPoint(
      2, {box.position.x + (unit_width * 6), box.position.y - unit_height});
  trapezium.setPoint(3, {box.position.x + (unit_width * 7), box.position.y});
  trapezium.setFillColor(color);

  // draw to texture
  texture.draw(trapezium);

  // create text to draw and populate variables
  sf::Text text_to_draw{font, text};

  text_to_draw.setFillColor(sf::Color::White);
  text_to_draw.setPosition(
      {box.position.x + unit_width, box.position.y - (unit_height * 0.8f)});

  // resize the text to fit within the box square part of the trapezium
  sf::Vector2f text_box_size{unit_width * 5, unit_height};

  fit_text_to_box(text_to_draw, text_box_size, 0);
  // draw to texture
  texture.draw(text_to_draw);
}

/////////////////////////////////////////////////
void pick_and_draw_status_box(const MachinaFormScaffold &scaffold,
                              const sf::Font &font,
                              sf::RenderTexture &texture) {
  // switch on the state and draw the appropriate box
  switch (scaffold.structural_analysis_state) {

    // for NotRun and NotFound we can draw a generic box
  case StructuralAnalysisState::NotRun: {
    // draw a grey box with "Analysis Not Run" text
    auto box =
        positioning::grimoire_machina::calculate_outer_box(scaffold.parts);
    draw_status_box(box, {255, 255, 255, 50}, "No Analy.", font, texture);
    break;
  }

  case StructuralAnalysisState::NothingFound: {
    auto box =
        positioning::grimoire_machina::calculate_outer_box(scaffold.parts);
    // draw a red box with "No Archetypes Found" text
    draw_status_box(box, {168, 50, 50, 255}, "No Archetypes Found", font,
                    texture);
    break;
  }

    // now we are going to pass Subgraphs to the draw_status_box from every
    // successful analysis
  case StructuralAnalysisState::Found:

    // for each archetype result, we will draw a box around the subgraph
    for (const auto &[archetype_name, results] :
         scaffold.structural_analysis_results) {

      // we can use std::visit as each variant should have a get_unique_nodes()
      // function to get the unique nodes in the subgraph
      std::visit(
          [&](const auto &result) {
            SubGraph unique_nodes = result.get_unique_nodes();
            auto box = positioning::grimoire_machina::calculate_outer_box(
                scaffold.parts, unique_nodes);
            draw_status_box(box, {50, 168, 50, 255}, archetype_name, font,
                            texture);
          },
          results.front().result_sub_graphs);
    }
    break;
  }
}

} // namespace steamrot::logic::render::grimoire_machina
