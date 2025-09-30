/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for UIElementFactory unit tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_element_factory_helpers.h"
#include <string>
#include <variant>

namespace steamrot {
namespace tests {

void TestUIELementProperites(const UIElement &element,
                             const UIElementData &data) {
  // Check position
  REQUIRE(element.position.x == data.position()->x());
  REQUIRE(element.position.y == data.position()->y());
  // Check size
  REQUIRE(element.size.x == data.size()->x());
  REQUIRE(element.size.y == data.size()->y());

  // check the Subscriber data
  if (data.subscriber_data()) {
    REQUIRE(element.subscription);
    // check the EventType matches
    REQUIRE(element.subscription->GetRegistrationInfo().first ==
            data.subscriber_data()->event_type_data());
  }

  // check the EventPacket data
  if (data.response_event_data()) {
    REQUIRE(element.response_event.has_value());
    REQUIRE(element.response_event.value().m_event_type ==
            data.response_event_data()->event_type());
    // checking EventData matches the flatbuffers needs some proper work on it
    REQUIRE(std::holds_alternative<std::monostate>(
        element.response_event.value().m_event_data));
  }
  // Check spacing strategy
  REQUIRE(element.spacing_strategy == data.spacing_strategy());
  // Check layout
  REQUIRE(element.layout == data.layout());
  // Check children_active
  REQUIRE(element.children_active == data.children_active());
}

void TestPanelElementProperties(const PanelElement &element,
                                const PanelData &data) {
  // Currently no specific properties to test
  SUCCEED("PanelElement currently has no specific properties to test.");
}

void TestButtonElementProperties(const ButtonElement &element,
                                 const ButtonData &data) {
  REQUIRE(element.label == (data.label() ? data.label()->str() : ""));
}

void TestDropDownListElementProperties(const DropDownListElement &element,
                                       const DropDownListData &data) {
  REQUIRE(element.unexpanded_label ==
          (data.label() ? data.label()->str() : ""));
  REQUIRE(element.expanded_label ==
          (data.expanded_label() ? data.expanded_label()->str() : ""));
}

void TestDropDownContainerElementProperties(
    const DropDownContainerElement &element,
    const DropDownContainerData &data) {
  // No specific properties for DropDownContainerElement
  SUCCEED(
      "DropDownContainerElement currently has no specific properties to test.");
}

void TestDropDownItemElementProperties(const DropDownItemElement &element,
                                       const DropDownItemData &data) {
  REQUIRE(element.label == (data.label() ? data.label()->str() : ""));
}

void TestDropDownButtonElementProperties(const DropDownButtonElement &element,
                                         const DropDownButtonData &data) {
  REQUIRE(element.is_expanded == data.is_expanded());
}

/////////////////////////////////////////////////
/// Template specializations for TestFlatbufferElement, templates off of
/// Flatbuffers UIElementDataUnion
/////////////////////////////////////////////////

template <typename DataT>
void TestFlatbufferElement(const UIElement &element, const DataT &data);
template <>
void TestFlatbufferElement<PanelData>(const UIElement &element,
                                      const PanelData &data) {
  auto derived = dynamic_cast<const PanelElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestPanelElementProperties(*derived, data);

  auto children_data = data.base_data()->children();
  REQUIRE(children_data);
  REQUIRE(children_data->size() == derived->child_elements.size());
  for (size_t i = 0; i < children_data->size(); ++i) {
    const auto *child = children_data->Get(i);
    const auto &child_element = *(derived->child_elements[i]);
    auto child_union_type = child->element_type();
    auto child_union_ptr = child->element();
    TestNestedElementProperties(child_element, child_union_ptr,
                                child_union_type);
  }
}

// --- ButtonData specialization --- //
template <>
void TestFlatbufferElement<ButtonData>(const UIElement &element,
                                       const ButtonData &data) {
  auto derived = dynamic_cast<const ButtonElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestButtonElementProperties(*derived, data);

  auto children_data = data.base_data()->children();
  if (children_data && children_data->size() > 0) {
    REQUIRE(children_data->size() == derived->child_elements.size());
    for (size_t i = 0; i < children_data->size(); ++i) {
      const auto *child = children_data->Get(i);
      const auto &child_element = *(derived->child_elements[i]);
      auto child_union_type = child->element_type();
      auto child_union_ptr = child->element();
      TestNestedElementProperties(child_element, child_union_ptr,
                                  child_union_type);
    }
  }
}

// --- DropDownContainerData specialization --- //
template <>
void TestFlatbufferElement<DropDownContainerData>(
    const UIElement &element, const DropDownContainerData &data) {
  auto derived = dynamic_cast<const DropDownContainerElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestDropDownContainerElementProperties(*derived, data);

  auto children_data = data.base_data()->children();
  if (children_data && children_data->size() > 0) {
    REQUIRE(children_data->size() == derived->child_elements.size());
    for (size_t i = 0; i < children_data->size(); ++i) {
      const auto *child = children_data->Get(i);
      const auto &child_element = *(derived->child_elements[i]);
      auto child_union_type = child->element_type();
      auto child_union_ptr = child->element();
      TestNestedElementProperties(child_element, child_union_ptr,
                                  child_union_type);
    }
  }
}

// --- DropDownListData specialization --- //
template <>
void TestFlatbufferElement<DropDownListData>(const UIElement &element,
                                             const DropDownListData &data) {
  auto derived = dynamic_cast<const DropDownListElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestDropDownListElementProperties(*derived, data);

  auto children_data = data.base_data()->children();
  if (children_data && children_data->size() > 0) {
    REQUIRE(children_data->size() == derived->child_elements.size());
    for (size_t i = 0; i < children_data->size(); ++i) {
      const auto *child = children_data->Get(i);
      const auto &child_element = *(derived->child_elements[i]);
      auto child_union_type = child->element_type();
      auto child_union_ptr = child->element();
      TestNestedElementProperties(child_element, child_union_ptr,
                                  child_union_type);
    }
  }
}

// --- DropDownItemData specialization --- //
template <>
void TestFlatbufferElement<DropDownItemData>(const UIElement &element,
                                             const DropDownItemData &data) {
  auto derived = dynamic_cast<const DropDownItemElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestDropDownItemElementProperties(*derived, data);
  // Typically no children
}

// --- DropDownButtonData specialization --- //
template <>
void TestFlatbufferElement<DropDownButtonData>(const UIElement &element,
                                               const DropDownButtonData &data) {
  auto derived = dynamic_cast<const DropDownButtonElement *>(&element);
  REQUIRE(derived != nullptr);

  TestUIELementProperites(*derived, *data.base_data());
  TestDropDownButtonElementProperties(*derived, data);
  // Typically no children
}
void TestNestedElementProperties(const UIElement &element, const void *data,
                                 UIElementDataUnion type) {
  using namespace steamrot;

  switch (type) {
  case UIElementDataUnion_PanelData:
    TestFlatbufferElement(element, *reinterpret_cast<const PanelData *>(data));
    break;
  case UIElementDataUnion_ButtonData:
    TestFlatbufferElement(element, *reinterpret_cast<const ButtonData *>(data));
    break;
  case UIElementDataUnion_DropDownContainerData:
    TestFlatbufferElement(
        element, *reinterpret_cast<const DropDownContainerData *>(data));
    break;
  case UIElementDataUnion_DropDownListData:
    TestFlatbufferElement(element,
                          *reinterpret_cast<const DropDownListData *>(data));
    break;
  case UIElementDataUnion_DropDownItemData:
    TestFlatbufferElement(element,
                          *reinterpret_cast<const DropDownItemData *>(data));
    break;
  case UIElementDataUnion_DropDownButtonData:
    TestFlatbufferElement(element,
                          *reinterpret_cast<const DropDownButtonData *>(data));
    break;
  default:
    FAIL("Unknown FlatBuffer UIElementDataUnion type in nested check.");
  }
}
} // namespace tests
} // namespace steamrot
