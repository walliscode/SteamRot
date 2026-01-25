
add_library(assets
    AssetManager.cpp
)

target_include_directories(assets
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(assets
    PUBLIC
  SFML::Graphics
  types
  data_providers
)



