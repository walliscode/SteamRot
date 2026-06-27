add_library(machina_form_scaffolds
  machina_form_scaffold_library.cpp
)

target_include_directories(machina_form_scaffolds
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(machina_form_scaffolds
  PUBLIC
  types
  SFML::Graphics
)
