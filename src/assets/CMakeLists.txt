
add_library(assets
    AssetManager.cpp
)

target_include_directories(assets
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(assets
    PRIVATE
  SFML::Graphics
  types
  interfaces
  user_interface
  data_providers
)



