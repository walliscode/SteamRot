////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"
#include "Logic.h"
#include "themes_generated.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace steamrot {

/**
 * @class PanelStyle
 * @brief Contains the style properties for a panel in the UI.
 *
 */
struct PanelStyle {
  sf::Color background_color;
  sf::Color border_color;
  float border_thickness;
};
/**
 * @class ButtonStyle
 * @brief Contains the style properties for a button in the UI.
 *
 */
struct ButtonStyle {
  sf::Color background_color;
  sf::Color text_color;
  sf::Color border_color;
};

class UIRenderLogic : public Logic<CUserInterface> {
private:
  /**
   * @brief Draw a panel to the UI render texture.
   *
   * @param render_texture from sources such as the Scene or Display
   */
  void DrawPanel(sf::RenderTexture &render_texture);

  ////////////////////////////////////////////////////////////
  // |brief draw drop down menu
  ////////////////////////////////////////////////////////////
  void DrawDropDownMenu(sf::RenderTexture &render_texture);

  ////////////////////////////////////////////////////////////
  // |brief add in styles from flatbuffer config
  ////////////////////////////////////////////////////////////
  void AddStyles(const themes::UIObjects *config);

  /**
   * @brief Member variable to hold the style for buttons in the UI.
   */
  ButtonStyle m_button_style;

  /**
   * @brief Member variable to hold the style for panels in the UI.
   */
  PanelStyle m_panel_style;

public:
  ////////////////////////////////////////////////////////////
  // |brief Default constructor
  ////////////////////////////////////////////////////////////
  UIRenderLogic();

  ////////////////////////////////////////////////////////////
  // |brief Constructor taking in flatbuffer config
  ////////////////////////////////////////////////////////////
  UIRenderLogic(const themes::UIObjects *config);

  /**
   * @brief Gather all logic here, to be called by the Logic.RunLogic() function
   *
   * @param entities A reference to the EntityMemoryPool containing all
   * entities.
   * @param entity_indicies A mask of entity indiced to process the
   * EnittyMemoryPool.
   */
  void ProcessLogic(components::containers::EntityMemoryPool &entities,
                    const EntityIndicies &entity_indicies) override;
  /**
   * @brief Draw a test button on the UI layer.
   *
   * @param ui_layer: The render texture where the button will be drawn.
   */
  void DrawTestButton(sf::RenderTexture &ui_layer);

  void DrawUILayer(sf::RenderTexture &ui_layer);
}; // namespace UIEngine
} // namespace steamrot
