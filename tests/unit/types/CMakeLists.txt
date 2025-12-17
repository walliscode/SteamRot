add_executable(test_types

  core/FailInfo.test.cpp
  core/EngineConfig.test.cpp
  core/SceneInfo.test.cpp
  core/Joint.test.cpp
  core/Fragment.test.cpp

  events/Subscriber.test.cpp
  events/UserInputBitset.test.cpp

  user_interface/ButtonElement.test.cpp
  user_interface/PanelElement.test.cpp
  user_interface/DropDownListElement.test.cpp
  user_interface/DropDownContainerElement.test.cpp
  user_interface/DropDownButtonElement.test.cpp
  user_interface/DropDownItemElement.test.cpp
)

target_link_libraries(test_types PRIVATE
  Catch2::Catch2WithMain
  types
)

catch_discover_tests(test_types)
