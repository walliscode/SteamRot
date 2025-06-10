add_library(logic
  UIRenderLogic.cpp
)

target_include_directories(logic
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic PUBLIC
  components
  entity
)
