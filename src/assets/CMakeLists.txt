
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
  logger
  types
  data_providers
  user_interface
)



