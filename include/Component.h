#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Component {
protected:
  //  the tuple of vectors, ComponentCollectionTuple, will contain already
  //  instantiated components the m_has_component flag will be used to check if
  //  the component should be used by the entity
  bool m_has_component{false};
  Component() {};

  // set component data with json data
  virtual void LoadJSONData(const json &component_config) = 0;

public:
  const bool getHas() const { return m_has_component; };
  void setHas(const bool &has) { m_has_component = has; };
};
